/* Copyright 2013-2014 Dietrich Epp.
   This file is part of Dreamless.  Dreamless is licensed under the terms
   of the 2-clause BSD license.  For more information, see LICENSE.txt. */
#ifndef LD_BASE_SPRITE_HPP
#define LD_BASE_SPRITE_HPP
#include "array.hpp"
#include "sg/opengl.h"
#include <array>
#include <cstdlib>
#include <vector>
#include <string>
namespace Base {

/// Orthogonal orientations for 2D sprites.
enum class Orientation {
    NORMAL, ROTATE_90, ROTATE_180, ROTATE_270,
    FLIP_VERTICAL, TRANSPOSE_2, FLIP_HORIZONTAL, TRANSPOSE
};

/// Compose two orientations.
Orientation operator*(Orientation x, Orientation y);

/// A 2D integer rectangle.
struct SpriteRect {
    short x, y, w, h, cx, cy;
};

/// A sprite to add to a sprite sheet.
struct Sprite {
    const char *name;
    short x, y, w, h, cx, cy;
};

/// A sprite sheet.
class SpriteSheet {
private:
    std::vector<SpriteRect> m_sprites;
    GLuint m_texture;
    int m_width, m_height;
    float m_texscale[2];

public:
    SpriteSheet();
    SpriteSheet(const std::string &dirname, const Sprite *sprites);
    SpriteSheet(const SpriteSheet &other) = delete;
    SpriteSheet(SpriteSheet &&other);
    ~SpriteSheet();
    SpriteSheet &operator=(const SpriteSheet &other) = delete;
    SpriteSheet &operator=(SpriteSheet &&other);

    /// Get the texture object containing the sprites.
    GLuint texture() const { return m_texture; }
    /// Get the width of the texture.
    int width() const { return m_width; }
    /// Get the height of the texture.
    int height() const { return m_height; }
    /// Get the factor to convert pixel coordinates to texture coordinates.
    const float *texscale() const { return m_texscale; }
    /// Get the rectangle containing the given sprite.
    SpriteRect get(int index) const { return m_sprites.at(index); }
};

// Array of sprite rectangles with texture coordinates.
// Draw with GL_TRIANGLES.
class SpriteArray {
private:
    Array<short[4]> m_array;

public:
    SpriteArray();
    SpriteArray(const SpriteArray &other) = delete;
    SpriteArray(SpriteArray &&other);
    ~SpriteArray();
    SpriteArray &operator=(const SpriteArray &other) = delete;
    SpriteArray &operator=(SpriteArray &&other) = delete;

    /// Clear the array.
    void clear();
    /// Add a sprite (tex) at the given lower-left coordinate.
    void add(SpriteRect tex, int x, int y);
    /// Add a sprite (tex) at the given lower-left coordinate.
    void add(SpriteRect tex, int x, int y, Orientation orient);
    /// Upload the array data.
    void upload(GLuint usage);
    /// Bind the OpenGL attribute.
    void set_attrib(GLint attrib);
    /// Get the number of vertexes.
    int size() const { return m_array.size(); }
    /// Determine whether the array is empty.
    bool empty() const { return m_array.empty(); }
};

}
#endif
