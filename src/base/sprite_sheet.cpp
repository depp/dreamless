/* Copyright 2013-2014 Dietrich Epp.
   This file is part of Dreamless.  Dreamless is licensed under the terms
   of the 2-clause BSD license.  For more information, see LICENSE.txt. */
#include "sg/opengl.h"
#include "sprite.hpp"
#include "pack.hpp"
#include "image.hpp"
#include "log.hpp"
#include <unordered_map>
#include <string>
namespace Base {

SpriteSheet::SpriteSheet()
    : m_sprites(), m_texture()
{ }

SpriteSheet::SpriteSheet(const std::string &dirname, const Sprite *sprites)
    : m_sprites(), m_texture() {
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

    Log::info("Packing %zu sprites into a %dx%d sheet",
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

    Image tileimage;
    tileimage.set(SG_RGBA, packing.packsize.width, packing.packsize.height);
    tileimage.calloc();

    for (std::size_t i = 0; i < images.size(); i++) {
        tileimage.copy_from(
            images[i],
            packing.locations[i].x,
            packing.locations[i].y);
    }

    m_texture = Texture::load(tileimage);
}

SpriteSheet::~SpriteSheet()
{ }

}
