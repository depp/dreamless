/* Copyright 2014 Dietrich Epp.
   This file is part of Dreamless.  Dreamless is licensed under the terms
   of the 2-clause BSD license.  For more information, see LICENSE.txt. */
#ifndef LD_BASE_VEC_HPP
#define LD_BASE_VEC_HPP
#include <cmath>
namespace Base {

/// Two-dimensional vector.
struct FVec {
    float x, y;

    FVec() { }
    FVec(float x, float y) : x(x), y(y) { }
    static FVec zero() { return FVec(0, 0); }

    /// Get the magnitude of the vector, squared.
    float mag2() const;
    /// Get the magnitude of the vector.
    float mag() const;
    /// Get the dot product of two vectors.
    static float dot(FVec u, FVec v);
    /// Get the distance between two vectors, squared.
    static float dist2(FVec u, FVec v);
    /// Get the distance between two vectors.
    static float dist(FVec u, FVec v);
};

inline FVec operator+(FVec u, FVec v) { return FVec(u.x + v.x, u.y + v.y); }
inline FVec &operator+=(FVec &u, FVec v) { u.x += v.x; u.y += v.y; return u; }
inline FVec operator-(FVec u, FVec v) { return FVec(u.x - v.x, u.y - v.y); }
inline FVec &operator-=(FVec &u, FVec v) { u.x -= v.x; u.y -= v.y; return u; }
inline FVec operator*(float a, FVec v) { return FVec(a * v.x, a * v.y); }
inline FVec operator*(FVec v, float a) { return FVec(a * v.x, a * v.y); }
inline FVec &operator*=(FVec &v, float a) { v.x *= a; v.y *= a; return v; }

inline float FVec::mag2() const { return x * x + y * y; }
inline float FVec::mag() const { return std::sqrt(mag()); }
inline float FVec::dot(FVec u, FVec v) { return u.x * v.x + u.y * v.y; }
inline float FVec::dist2(FVec u, FVec v) { return (u - v).mag2(); }
inline float FVec::dist(FVec u, FVec v) { return std::sqrt(dist2(u, v)); }

// Floating-point rectangle.
struct FRect {
    float x0, y0, x1, y1;

    FRect() { }
    FRect(float x0, float y0, float x1, float y1)
        : x0(x0), y0(y0), x1(x1), y1(y1)
    { }

    static FRect zero() { return FRect(0, 0, 0, 0); }
    static bool test_intersect(const FRect &a, const FRect &b);
    FVec center() const;
    FRect offset(FVec v) const;
    FRect expand(float amt) const;
    FRect expand(float horiz, float vert) const;
    FRect expand(const FRect &r) const;
    FVec nearest(FVec v) const;
    float width() const { return x1 - x0; }
    float height() const { return y1 - y0; }
};

/// Integer vector.
struct IVec {
    int x, y;
    IVec() { };
    IVec(int x, int y) : x(x), y(y) { }
    explicit IVec(FVec v)
        : x((int)std::floor(v.x)),
          y((int)std::floor(v.y))
    { }

    static IVec zero() { return IVec(0, 0); }
    explicit operator FVec() const { return FVec(x, y); }
};

inline IVec operator+(IVec u, IVec v) { return IVec(u.x + v.x, u.y + v.y); }
inline IVec &operator+=(IVec &u, IVec v) { u.x += v.x; u.y += v.y; return u; }
inline IVec operator-(IVec u, IVec v) { return IVec(u.x - v.x, u.y - v.y); }
inline IVec &operator-=(IVec &u, IVec v) { u.x -= v.x; u.y -= v.y; return u; }
inline IVec operator*(int a, IVec v) { return IVec(a * v.x, a * v.y); }
inline IVec operator*(IVec v, int a) { return IVec(a * v.x, a * v.y); }
inline IVec &operator*=(IVec &v, int a) { v.x *= a; v.y *= a; return v; }

/// Integer rectangle.
struct IRect {
    int x0, y0, x1, y1;

    IRect() { }
    IRect(int x0, int y0, int x1, int y1)
        : x0(x0), y0(y0), x1(x1), y1(y1)
    { }

    static IRect zero() { return IRect(0, 0, 0, 0); }
    static IRect centered(int w, int h);
    static bool test_intersect(const IRect &a, const IRect &b);
    IVec center() const;
    IRect offset(IVec v) const;
    IRect expand(int amt) const;
    IRect expand(int horiz, int vert) const;
    IRect expand(const IRect &r) const;
    bool contains(IVec v) const;
    int width() const { return x1 - x0; }
    int height() const { return y1 - y0; }
};

}
#endif
