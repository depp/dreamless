/* Copyright 2014 Dietrich Epp.
   This file is part of Dreamless.  Dreamless is licensed under the terms
   of the 2-clause BSD license.  For more information, see LICENSE.txt. */
#include "random.hpp"
namespace Base {

Random Random::global = {
    0x20cc842bu,
    0xe2db5f92u,
    0x62bb2830u,
    0xda65a90bu
};

void Random::init() {
    x = 0x20cc842bu;
    y = 0xe2db5f92u;
    z = 0x62bb2830u;
    w = 0xda65a90bu;
}

}
