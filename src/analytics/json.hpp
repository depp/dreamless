/* Copyright 2014 Dietrich Epp.
   This file is part of Dreamless.  Dreamless is licensed under the terms
   of the 2-clause BSD license.  For more information, see LICENSE.txt. */
#ifndef LD_ANALYTICS_JSON_HPP
#define LD_ANALYTICS_JSON_HPP
#include <string>
namespace Analytics {

class JSON {
private:
    std::string m_value;
    bool m_hasdata;

    void write_string(const std::string &s);
    void write_key(const std::string &key);

public:
    JSON();

    void put_string(const std::string &key, const std::string &value);
    void put_int(const std::string &key, int value);
    void put_bool(const std::string &key, bool value);
    void put_null(const std::string &key);

    std::string to_string() const;
};

}
#endif
