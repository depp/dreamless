/* Copyright 2014 Dietrich Epp.
   This file is part of Dreamless.  Dreamless is licensed under the terms
   of the 2-clause BSD license.  For more information, see LICENSE.txt. */
#include "analytics.hpp"
#include "curl.hpp"
#include "json.hpp"
#include "sg/atomic.h"
#include "sg/cvar.h"
#include "sg/entry.h"
#include "sg/log.h"
#include "sg/thread.h"
#include "sg/version.h"
#include <vector>
#include <memory>

#if defined SG_THREAD_PTHREAD
# include <pthread.h>
# include <sys/time.h>
# include <unistd.h>
# include <stdlib.h>
# include <errno.h>
#elif defined SG_THREAD_WINDOWS
# include <windows.h>
#else
# error "Unknown threading system"
#endif

namespace Analytics {

namespace {

// Only send analytics if at least 5 seconds were spent in the level.
const int MIN_LEVEL_TIME = 5 * 1000;

// Max retries before the analytics are shut down.
const int MAX_RETRIES = 2;

// Delay between retries, in seconds.
const int RETRY_DELAY = 10;

// Wait at most five seconds to shut down.
const int SHUTDOWN_WAIT = 5;

sg_logger *logger;
std::vector<Level> queue;

struct Start {
    std::string computer_id;
    std::string game_version;
    std::string os_version;
};

std::string to_json(const Start &s) {
    JSON j;
    j.put_string("computerId", s.computer_id);
    j.put_string("gameVersion", s.game_version);
    j.put_string("osVersion", s.os_version);
    return j.to_string();
}

const char STATUS_NAME[STATUS_COUNT][12] = {
    "inProgress", "skipNext", "skipPrev", "restart", "success"
};

std::string status_name(Status status) {
    int idx = static_cast<int>(status);
    if (idx < 0 || idx >= STATUS_COUNT)
        sg_sys_abort("invalid level status");
    return STATUS_NAME[idx];
}

std::string to_json(const Level &l, const std::string &sid) {
    JSON j;
    j.put_string("sessionId", sid);
    j.put_int("index", l.index);
    j.put_int("level", l.level);
    j.put_int("timeStart", l.time_start);
    if (l.time_wake == -1)
        j.put_null("timeWake");
    else
        j.put_int("timeWake", l.time_wake);
    j.put_int("timeEnd", l.time_end);
    j.put_int("actionCount", l.action_count);
    j.put_bool("talkedToShadow", l.talked_to_shadow);
    j.put_string("status", status_name(l.status));
    return j.to_string();
}

typedef std::pair<std::string, Start> Arg;

#if defined SG_THREAD_PTHREAD

namespace PThread {
pthread_mutex_t mutex;
pthread_cond_t cond;
pthread_t thread;
bool is_started, request_stop, is_stopped;

void *worker(void *arg) {
    int r;

    std::string endpoint, session_id;

    {
        std::unique_ptr<Arg> a(reinterpret_cast<Arg *>(arg));
        endpoint = a->first;
        auto json = to_json(a->second);
        for (int retry = 0; ; retry++) {
            if (retry > 0) {
                sg_logs(logger, SG_LOG_WARN, "Retrying request...");
                sleep(RETRY_DELAY);
            }

            auto result = Curl::post(endpoint + "start", json);
            if (result.first) {
                session_id = result.second;
                break;
            }
            if (retry >= MAX_RETRIES) {
                sg_logs(logger, SG_LOG_ERROR, "Too many analytics failures.");
                is_stopped = true;
                return nullptr;
            }
        }
    }

    r = pthread_mutex_lock(&mutex);
    if (r) abort();
    is_started = true;
    r = pthread_mutex_unlock(&mutex);
    if (r) abort();

    sg_logs(logger, SG_LOG_INFO, "Analytics startup successful.");

    while (true) {
        r = pthread_mutex_lock(&mutex);
        if (r) abort();
        while (queue.empty() && !request_stop) {
            r = pthread_cond_wait(&cond, &mutex);
            if (r) abort();
        }

        Level level;
        bool done = queue.empty();
        if (!done) {
            level = queue.front();
            queue.erase(queue.begin());
        }

        r = pthread_mutex_unlock(&mutex);
        if (r) abort();

        if (done)
            break;
        auto json = to_json(level, session_id);

        int i;
        for (i = 0; i < MAX_RETRIES; i++) {
            if (i > 0) {
                sg_logs(logger, SG_LOG_WARN, "Retrying request...");
                sleep(RETRY_DELAY);
            }

            auto result = Curl::post(endpoint + "level", json);
            if (result.first && result.second == "ok")
                break;
        }
        if (i == MAX_RETRIES) {
            sg_logs(logger, SG_LOG_ERROR, "Too many analytics failures.");
            break;
        }

        sg_logs(logger, SG_LOG_INFO, "Post successful.");
    }

    sg_logs(logger, SG_LOG_INFO, "Shutting down analytics.");

    r = pthread_mutex_lock(&mutex);
    if (r) abort();
    is_stopped = true;
    pthread_cond_broadcast(&cond);
    r = pthread_mutex_unlock(&mutex);
    if (r) abort();

    return nullptr;
}

void start_worker(const std::string &endpoint, const Start &start) {
    int r;

    pthread_mutexattr_t mattr;
    r = pthread_mutexattr_init(&mattr);
    if (r) abort();
    r = pthread_mutexattr_settype(&mattr, PTHREAD_MUTEX_NORMAL);
    if (r) abort();
    r = pthread_mutex_init(&mutex, &mattr);
    if (r) abort();
    r = pthread_mutexattr_destroy(&mattr);
    if (r) abort();

    r = pthread_cond_init(&cond, nullptr);
    if (r) abort();

    pthread_attr_t threadattr;
    r = pthread_attr_init(&threadattr);
    if (r) abort();
    r = pthread_attr_setdetachstate(&threadattr, PTHREAD_CREATE_DETACHED);
    if (r) abort();
    Arg *arg = new Arg(endpoint, start);
    r = pthread_create(&thread, &threadattr,
                       worker, reinterpret_cast<void *>(arg));
    if (r) abort();
    pthread_attr_destroy(&threadattr);
}

void stop_worker() {
    int r;

    r = pthread_mutex_lock(&mutex);
    if (r) abort();

    if (is_started) {
        request_stop = true;
        r = pthread_cond_broadcast(&cond);
        if (r) abort();

        timeval tv;
        gettimeofday(&tv, nullptr);
        timespec ts;
        ts.tv_sec = tv.tv_sec + SHUTDOWN_WAIT;
        ts.tv_nsec = tv.tv_usec * 1000;
        while (!is_stopped) {
            r = pthread_cond_timedwait(&cond, &mutex, &ts);
            if (r == ETIMEDOUT) {
                sg_logs(logger, SG_LOG_ERROR, "Analytics timed out.");
                break;
            }
            if (r) abort();
        }
    }

    r = pthread_mutex_unlock(&mutex);
    if (r) abort();
}

void submit_level(const Level &level) {
    int r;

    r = pthread_mutex_lock(&mutex);
    if (r) abort();

    if (!is_stopped) {
        if (!queue.empty() && queue.back().index == level.index)
            queue.back() = level;
        else
            queue.push_back(level);
        r = pthread_cond_broadcast(&cond);
        if (r) abort();
    }

    r = pthread_mutex_unlock(&mutex);
    if (r) abort();
}

}

using PThread::start_worker;
using PThread::stop_worker;
using PThread::submit_level;

#elif defined SG_THREAD_WINDOWS

namespace Windows {

CRITICAL_SECTION lock;
bool is_started, request_stop, is_stopped;
HANDLE has_data, has_stopped;

static DWORD WINAPI worker(void *param) {
    BOOL r;
    std::string endpoint, session_id;

    {
        std::unique_ptr<Arg> a(reinterpret_cast<Arg *>(param));
        endpoint = a->first;
        auto json = to_json(a->second);
        for (int retry = 0;; retry++) {
            if (retry > 0) {
                sg_logs(logger, SG_LOG_WARN, "Retrying request...");
                Sleep(RETRY_DELAY * 1000);
            }

            auto result = Curl::post(endpoint + "start", json);
            if (result.first) {
                session_id = result.second;
                break;
            }
            if (retry >= MAX_RETRIES) {
                sg_logs(logger, SG_LOG_ERROR, "Too many analytics failures.");
                is_stopped = true;
                r = SetEvent(has_stopped);
                if (!r) abort();
                return 0;
            }
        }
    }

    EnterCriticalSection(&lock);
    is_started = true;
    LeaveCriticalSection(&lock);

    sg_logs(logger, SG_LOG_INFO, "Analytics startup successful.");

    while (true) {
        EnterCriticalSection(&lock);
        while (queue.empty() && !request_stop) {
            LeaveCriticalSection(&lock);
            r = WaitForSingleObject(has_data, INFINITE);
            EnterCriticalSection(&lock);
        }

        Level level;
        bool done = queue.empty();
        if (!done) {
            level = queue.front();
            queue.erase(queue.begin());
        }

        LeaveCriticalSection(&lock);

        if (done)
            break;
        auto json = to_json(level, session_id);

        int i;
        for (i = 0; i < MAX_RETRIES; i++) {
            if (i > 0) {
                sg_logs(logger, SG_LOG_WARN, "Retrying request...");
                Sleep(RETRY_DELAY * 1000);
            }

            auto result = Curl::post(endpoint + "level", json);
            if (result.first && result.second == "ok")
                break;
        }
        if (i == MAX_RETRIES) {
            sg_logs(logger, SG_LOG_ERROR, "Too many analytics failures.");
            break;
        }

        sg_logs(logger, SG_LOG_INFO, "Post successful.");
    }

    sg_logs(logger, SG_LOG_INFO, "Shutting down analytics.");

    EnterCriticalSection(&lock);
    is_stopped = true;
    r = SetEvent(has_stopped);
    if (!r) abort();
    LeaveCriticalSection(&lock);

    return 0;
}

void start_worker(const std::string &endpoint, const Start &start) {
    InitializeCriticalSection(&lock);
    has_data = CreateEvent(nullptr, TRUE, FALSE, nullptr);
    if (!has_data) abort();
    has_stopped = CreateEvent(nullptr, TRUE, FALSE, nullptr);
    if (!has_stopped) abort();

    Arg *arg = new Arg(endpoint, start);
    HANDLE thread = CreateThread(nullptr, 0, worker,
                                 reinterpret_cast<void *>(arg), 0, NULL);
}

void stop_worker() {
    BOOL r;
    bool do_wait;
    EnterCriticalSection(&lock);
    do_wait = is_started;
    if (do_wait) {
        request_stop = true;
        r = SetEvent(has_data);
    }
    LeaveCriticalSection(&lock);

    WaitForSingleObject(has_stopped, SHUTDOWN_WAIT * 1000);
}

void submit_level(const Level &level) {
    EnterCriticalSection(&lock);

    if (!is_stopped) {
        if (!queue.empty() && queue.back().index == level.index)
            queue.back() = level;
        else
            queue.push_back(level);
        BOOL r = SetEvent(has_data);
        if (!r) abort();
    }

    LeaveCriticalSection(&lock);
}

}

using Windows::start_worker;
using Windows::stop_worker;
using Windows::submit_level;

#else
# error "Unknown threading system"
#endif

}

void Level::submit() const {
    if (time_end < MIN_LEVEL_TIME)
        return;

    submit_level(*this);
}

void Analytics::init() {
    Curl::init();
    logger = sg_logger_get("analytics");

    std::string endpoint;
    {
        const char *uri;
        if (!sg_cvar_gets("analytics", "uri", &uri))
            uri = "http://analytics.moria.us/dreamless/";
        endpoint = uri;
    }

    Start s;
    {
        char buf[64];

        sg_version_machineid(buf, sizeof(buf));
        s.computer_id = buf;

        s.game_version = "Dreamless/";
        s.game_version += SG_APP_VERSION;
        s.game_version += " (SGLib/";
        s.game_version += SG_SG_VERSION;
        s.game_version += ')';

        buf[0] = '\0';
        sg_version_os(buf, sizeof(buf));
        s.os_version = buf;
    }

    start_worker(endpoint, s);
}

void Analytics::finish() {
    stop_worker();
}

}
