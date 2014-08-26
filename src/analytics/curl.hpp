/* Copyright 2014 Dietrich Epp.
   This file is part of Dreamless.  Dreamless is licensed under the terms
   of the 2-clause BSD license.  For more information, see LICENSE.txt. */
#ifndef LD_ANALYTICS_CURL_HPP
#define LD_ANALYTICS_CURL_HPP
#include <string>
#include <tuple>
namespace Analytics {

struct Curl {
    /// Initialize curl.
    static void init();

    /// Post data and return the result.  The first member of the
    /// pair is false if the request failed.
    static std::pair<bool, std::string> post(const std::string &uri,
                                             const std::string &data);
};

}
#endif
