/* Copyright 2013-2014 Dietrich Epp.
   This file is part of Dreamless.  Dreamless is licensed under the terms
   of the 2-clause BSD license.  For more information, see LICENSE.txt. */
#include "sg/opengl.h"
#include "sprite.hpp"
#include "pack.hpp"
#include "image.hpp"
#include <unordered_map>
#include <cstdio>
#include <string>
namespace Base {

SpriteSheet::SpriteSheet()
    : m_sprites(), m_texture(0), m_width(0), m_height(0) {
    m_texscale[0] = m_texscale[1] = 0.0f;
}

SpriteSheet::SpriteSheet(const std::string &dirname, const Sprite *sprites)
    : m_sprites(), m_texture(0), m_width(0), m_height(0) {
    m_texscale[0] = m_texscale[1] = 0.0f;
    std::vector<Image> images;
    std::vector<std::size_t> spriteimages;
    std::size_t count = 0;

    {
        std::string dirpath(dirname);
        if (!dirpath.empty())
            dirpath += '/';
        std::unordered_map<std::string, std::size_t> imagenames;
        for (int i = 0; sprites[i].name; i++) {
            count++;
            std::string name(sprites[i].name);
            auto x = imagenames.insert(
                std::unordered_map<std::string, std::size_t>::value_type(
                    name, images.size()));
            if (x.second) {
                std::string path = dirpath;
                path += name;
                Image image;
                image.load(path);
                images.push_back(std::move(image));
            }
            spriteimages.push_back(x.first->second);
        }
    }

    std::vector<Packing::Size> imagesizes;
    imagesizes.reserve(images.size());
    for (auto &image : images) {
        Packing::Size sz = { image->iwidth, image->iheight };
        imagesizes.push_back(sz);
    }

    Packing packing = Packing::pack(imagesizes);

    std::fprintf(stderr, "Packing %zu sprites into a %dx%d sheet\n",
                 count, packing.packsize.width, packing.packsize.height);

    m_sprites.reserve(count);
    for (std::size_t i = 0; i < count; i++) {
        auto loc = packing.locations[spriteimages[i]];
        SpriteRect rect;
        rect.x = sprites[i].x + loc.x;
        rect.y = sprites[i].y + loc.y;
        rect.w = sprites[i].w;
        rect.h = sprites[i].h;
        rect.cx = sprites[i].cx;
        rect.cy = sprites[i].cy;
        m_sprites.push_back(rect);
    }

    m_width = packing.packsize.width;
    m_height = packing.packsize.height;
    m_texscale[0] = 1.0 / m_width;
    m_texscale[1] = 1.0 / m_height;

    Image tileimage;
    tileimage.set(SG_RGBA, packing.packsize.width, packing.packsize.height);
    tileimage.calloc();

    for (std::size_t i = 0; i < images.size(); i++) {
        tileimage.copy_from(
            images[i],
            packing.locations[i].x,
            packing.locations[i].y);
    }

    glGenTextures(1, &m_texture);
    glBindTexture(GL_TEXTURE_2D, m_texture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexImage2D(
        GL_TEXTURE_2D,
        0,
        GL_RGBA8,
        packing.packsize.width,
        packing.packsize.height,
        0,
        GL_BGRA,
        GL_UNSIGNED_INT_8_8_8_8_REV,
        tileimage->data);
    glBindTexture(GL_TEXTURE_2D, 0);
}

SpriteSheet::SpriteSheet(SpriteSheet &&other)
    : m_sprites(std::move(other.m_sprites)), m_texture(other.m_texture),
      m_width(other.m_width), m_height(other.m_height) {
    other.m_sprites.clear();
    other.m_texture = 0;
    other.m_width = 0;
    other.m_height = 0;
    other.m_texscale[0] = 0.0f;
    other.m_texscale[1] = 0.0f;
}

SpriteSheet &SpriteSheet::operator=(SpriteSheet &&other) {
    if (this == &other)
        return *this;
    if (m_texture != 0)
        glDeleteTextures(1, &m_texture);

    m_sprites = std::move(other.m_sprites);
    m_texture = other.m_texture;
    m_width = other.m_width;
    m_height = other.m_height;
    m_texscale[0] = other.m_texscale[0];
    m_texscale[1] = other.m_texscale[1];
    other.m_sprites.clear();
    other.m_texture = 0;
    other.m_width = 0;
    other.m_height = 0;
    other.m_texscale[0] = 0.0f;
    other.m_texscale[1] = 0.0f;

    return *this;
}

SpriteSheet::~SpriteSheet() {
    if (m_texture != 0)
        glDeleteTextures(1, &m_texture);
}

}
