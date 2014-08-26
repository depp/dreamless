/* Copyright 2014 Dietrich Epp.
   This file is part of Dreamless.  Dreamless is licensed under the terms
   of the 2-clause BSD license.  For more information, see LICENSE.txt. */
#include "json.hpp"
namespace Analytics {

JSON::JSON() : m_value("{"), m_hasdata(false) { }

void JSON::write_string(const std::string &s) {
    m_value += '"';
    for (auto c : s) {
        if (c >= 32 && c <= 126) {
            if (c == '"' || c == '\\')
                m_value += '\\';
            m_value += c;
        } else {
            switch (c) {
            case '\n': m_value += "\\n"; break;
            case '\t': m_value += "\\t"; break;
            case '\r': m_value += "\\r"; break;
            }
            // Discard everything else, we don't care.
        }
    }
    m_value += '"';
}

void JSON::write_key(const std::string &key) {
    if (m_hasdata)
        m_value += ',';
    m_hasdata = true;
    write_string(key);
    m_value += ':';
}

void JSON::put_string(const std::string &key, const std::string &value) {
    write_key(key);
    write_string(value);
}

void JSON::put_int(const std::string &key, int value) {
    write_key(key);
    m_value += std::to_string(value);
}

void JSON::put_bool(const std::string &key, bool value) {
    write_key(key);
    m_value += value ? "true" : "false";
}

void JSON::put_null(const std::string &key) {
    write_key(key);
    m_value += "null";
}

std::string JSON::to_string() const {
    return m_value + '}';
}

}
