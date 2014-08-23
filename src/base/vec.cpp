/* Copyright 2014 Dietrich Epp.
   This file is part of Dreamless.  Dreamless is licensed under the terms
   of the 2-clause BSD license.  For more information, see LICENSE.txt. */
#include "vec.hpp"
namespace Base {

bool FRect::test_intersect(const FRect &a, const FRect &b) {
    return a.x0 <= b.x1 && a.x1 >= b.x0 &&
        a.y0 <= b.y1 && a.y1 >= b.y0;
}

FVec FRect::center() const {
    return FVec(0.5f * (x0 + x1), 0.5f * (y0 + y1));
}

FRect FRect::offset(FVec v) const {
    return FRect(x0 + v.x, y0 + v.y, x1 + v.x, y1 + v.y);
}

FVec FRect::nearest(FVec v) const {
    FVec r;

    if (v.x < x0)
        r.x = x0;
    else if (v.x > x1)
        r.x = x1;
    else
        r.x = v.x;

    if (v.y < y0)
        r.y = y0;
    else if (v.y > y1)
        r.y = y1;
    else
        r.y = v.y;

    return r;
}

IRect IRect::offset(IVec v) const {
    return IRect(x0 + v.x, y0 + v.y, x1 + v.x, y1 + v.y);
}

IRect IRect::expand(int amt) const {
    return IRect(x0 - amt, y0 - amt, x1 + amt, y1 + amt);
}

IRect IRect::expand(const IRect &r) const {
    return IRect(x0 + r.x0, y0 + r.y0, x1 + r.x1, y1 + r.y1);
}

bool IRect::contains(IVec v) const {
    return x0 <= v.x && v.x < x1 && y0 <= v.y && v.y < y1;
}

bool IRect::test_intersect(const IRect &a, const IRect &b) {
    return a.x0 < b.x1 && a.x1 > b.x0 && a.y0 < b.y1 && a.y1 > b.y0;
}

IVec IRect::center() const {
    return IVec(x0 + (x1 - x0) / 2, y0 + (y1 - y0) / 2);
}

IRect IRect::centered(int w, int h) {
    return IRect(-(w/2), -(h/2), w - (w/2), h - (h/2));
}

}
