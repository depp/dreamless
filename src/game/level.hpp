/* Copyright 2014 Dietrich Epp.
   This file is part of Dreamless.  Dreamless is licensed under the terms
   of the 2-clause BSD license.  For more information, see LICENSE.txt. */
#ifndef LD_GAME_LEVEL_HPP
#define LD_GAME_LEVEL_HPP
#include <string>
#include "defs.hpp"
namespace Graphics {
class System;
}
namespace Game {

/// Level data.
class Level {
    struct TileInfo {
        unsigned char c;
        Tile tile;
    };

    static bool is_initialized;
    static const TileInfo TILES_RAW[];
    static TileInfo TILES[256];

    int m_width;
    int m_height;
    unsigned char *m_data;

    static const TileInfo &tile_info(unsigned char tile) {
        return TILES[tile];
    }

public:
    Level();
    Level(const Level &) = delete;
    Level(Level &&other);
    ~Level();
    Level &operator=(const Level &) = delete;
    Level &operator=(Level &&other);

    void load(const std::string &name);
    void draw(::Graphics::System &gr) const;
};

}
#endif
