/* Copyright 2014 Dietrich Epp.
   This file is part of Dreamless.  Dreamless is licensed under the terms
   of the 2-clause BSD license.  For more information, see LICENSE.txt. */
#include "analytics.hpp"
#include "curl.hpp"
#include "json.hpp"
#include "sg/cvar.h"
#include "sg/entry.h"
#include "sg/log.h"
#include "sg/thread.h"
#include "sg/version.h"
#include <vector>

#if defined SG_THREAD_PTHREAD
# include <pthread.h>
# include <unistd.h>
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
const int MAX_RETRIES = 5;

// Delay between retries, in seconds.
const int RETRY_DELAY = 10;

sg_logger *logger;
std::string endpoint;
std::string session_id;
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

#if defined SG_THREAD_PTHREAD

namespace PThread {
pthread_mutex_t mutex;
pthread_cond_t cond;
pthread_t thread;
bool running;

void *worker(void *arg) {
    (void) arg;
    int r;
    std::string uri = endpoint + "level";

    while (true) {
        r = pthread_mutex_lock(&mutex);
        if (r) abort();
        while (queue.empty() && !session_id.empty()) {
            r = pthread_cond_wait(&cond, &mutex);
            if (r) abort();
        }

        std::string sid = session_id;
        Level level;
        if (!queue.empty()) {
            level = queue.front();
            queue.erase(queue.begin());
        }

        r = pthread_mutex_unlock(&mutex);
        if (r) abort();

        if (sid.empty())
            break;
        auto json = to_json(level, sid);

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

    queue.clear();
    session_id.clear();

    r = pthread_mutex_unlock(&mutex);
    if (r) abort();

    return nullptr;
}

void start_worker() {
    pthread_mutexattr_t mattr;
    int r;

    r = pthread_mutexattr_init(&mattr);
    if (r) abort();
    r = pthread_mutexattr_settype(&mattr, PTHREAD_MUTEX_NORMAL);
    if (r) abort();
    r = pthread_mutex_init(&mutex, &mattr);
    if (r) abort();
    r = pthread_mutexattr_destroy(&mattr);
    if (r) abort();

    r = pthread_cond_init(&cond, NULL);
    if (r) abort();

    if (!session_id.empty()) {
        r = pthread_create(&thread, NULL, worker, nullptr);
        if (r) abort();
        running = true;
    } else {
        running = false;
    }
}

void stop_worker() {
    if (!running)
        return;

    int r;

    r = pthread_mutex_lock(&mutex);
    if (r) abort();

    session_id.clear();
    r = pthread_cond_broadcast(&cond);
    if (r) abort();

    r = pthread_mutex_unlock(&mutex);
    if (r) abort();

    void *retval;
    r = pthread_join(thread, &retval);
    if (r) abort();
}

void submit_level(const Level &level) {
    int r;

    r = pthread_mutex_lock(&mutex);
    if (r) abort();

    if (!session_id.empty()) {
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

    {
        const char *uri;
        if (!sg_cvar_gets("analytics", "uri", &uri))
            uri = "http://analytics.moria.us/dreamless/";
        endpoint = uri;
    }

    logger = sg_logger_get("analytics");

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

    auto result = Curl::post(endpoint + "start", to_json(s));
    if (!result.first) {
        start_worker(); // We still want to initialize the mutex.
        sg_logs(logger, SG_LOG_ERROR, "failed to report to analytics server");
        return;
    }
    session_id = std::move(result.second);
    sg_logs(logger, SG_LOG_INFO, "have session ID");
    start_worker();
}

void Analytics::finish() {
    stop_worker();
}

}
