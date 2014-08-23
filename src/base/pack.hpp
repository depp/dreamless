/* Copyright 2013-2014 Dietrich Epp.
   This file is part of Dreamless.  Dreamless is licensed under the terms
   of the 2-clause BSD license.  For more information, see LICENSE.txt. */
#ifndef LD_BASE_PACK_HPP
#define LD_BASE_PACK_HPP
#include <stddef.h>
#include <vector>
namespace Base {

/// A complete packing of a set of rectangles.
struct Packing {
    struct Rect;
    struct State;
    struct Metrics;

    /// The size of a rectangle to be packed.
    struct Size {
        int width;
        int height;
    };

    /// The location of a packed rectangle.
    struct Location {
        int x;
        int y;
    };

    /// The size of the rectangle enclosing the packed rectangles.
    Size packsize;

    /// The location of the packed rectangles.
    std::vector<Location> locations;

    /// Efficiently pack a set of rectangles in a larger rectangle.
    static Packing pack(const std::vector<Size> &rects);
};

}
#endif
