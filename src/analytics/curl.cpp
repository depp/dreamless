/* Copyright 2014 Dietrich Epp.
   This file is part of Dreamless.  Dreamless is licensed under the terms
   of the 2-clause BSD license.  For more information, see LICENSE.txt. */
#include "curl.hpp"
#include "sg/log.h"
#include <curl/curl.h>

namespace {

const int MAX_RESPONSE = 1024;

sg_logger *logger;

/// Safe wrapper for curl header list.
class CurlHeader {
private:
    curl_slist *m_head;

public:
    CurlHeader() : m_head(nullptr) { }
    CurlHeader(const CurlHeader &) = delete;
    CurlHeader &operator=(const CurlHeader &) = delete;
    ~CurlHeader() {
        curl_slist_free_all(m_head);
    }
    void append(const char *value) {
        m_head = curl_slist_append(m_head, value);
    }
    operator curl_slist *() const { return m_head; }
};

/// Safe wrapper for CURL handle.
class CurlHandle {
private:
    CURL *m_handle;

public:
    explicit CurlHandle(CURL *handle) : m_handle(handle) { }
    CurlHandle(const CurlHandle &) = delete;
    CurlHandle &operator=(const CurlHandle &) = delete;
    ~CurlHandle() {
        if (m_handle)
            curl_easy_cleanup(m_handle);
    }
    operator CURL *() const { return m_handle; }
};

std::pair<bool, std::string> fail() {
    return std::make_pair(false, std::string());
}

extern size_t writefunc(void *ptr, size_t size, size_t nmemb,
                        void *userdata) {
    // FIXME: We don't catch exceptions here.  Should we?
    std::string *result = reinterpret_cast<std::string *>(userdata);
    if (size * nmemb > MAX_RESPONSE - result->size())
        return 0;
    result->append(reinterpret_cast<const char *>(ptr), size * nmemb);
    return size * nmemb;
}

}

namespace Analytics {

void Curl::init() {
    logger = sg_logger_get("analytics");
    curl_global_init(CURL_GLOBAL_ALL);
}

std::pair<bool, std::string> Curl::post(const std::string &uri,
                                        const std::string &data) {
    std::string result;
    CurlHandle curl(curl_easy_init());
    CurlHeader header;

    if (!curl) {
        sg_logs(logger, SG_LOG_ERROR, "Failed to get curl handle.");
        return fail();
    }

    header.append("Content-Type: application/json");

    curl_easy_setopt(curl, CURLOPT_URL, uri.c_str());
    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, data.c_str());
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, (curl_slist *) header);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writefunc);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &result);
    CURLcode r = curl_easy_perform(curl);
    long status;
    CURLcode r2 = curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &status);

    if (r2 == CURLE_OK && !(status >= 200 && status < 300)) {
        sg_logf(logger, SG_LOG_ERROR,
                "HTTP status: %ld", status);
        return fail();
    }

    if (r != CURLE_OK) {
        switch (r) {
        case CURLE_WRITE_ERROR:
            sg_logs(logger, SG_LOG_ERROR,
                    "HTTP response too long");
            break;
        default:
            sg_logf(logger, SG_LOG_ERROR,
                    "curl request failed: %s",
                    curl_easy_strerror(r));
            break;
        }
        return fail();
    }

    return std::make_pair(true, std::move(result));
}

}
