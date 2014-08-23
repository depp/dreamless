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

enum TileType {
    /// Free movement.
    OPEN,

    /// Completely solid.
    SOLID
};

/// Level data.
class Level {
    struct TileInfo {
        unsigned char c;
        Tile tile;
        TileType type;
    };

    static bool is_initialized;
    static const TileInfo TILES_RAW[];
    static const TileInfo TILE_SOLID;
    static TileInfo TILES[256];

    int m_width;
    int m_height;
    unsigned char *m_data;

    static const TileInfo &tile_info(unsigned char tile) {
        return TILES[tile];
    }

    const TileInfo &tile_at(IVec pos) const {
        if (pos.x < 0 || pos.y < 0 || pos.x >= m_width || pos.y >= m_width)
            return TILE_SOLID;
        return tile_info(m_data[m_width * pos.y + pos.x]);
    }

    static float tile_floor(TileType type, float relx);
    float tile_floor(IVec pos, float relx);

public:
    Level();
    Level(const Level &) = delete;
    Level(Level &&other);
    ~Level();
    Level &operator=(const Level &) = delete;
    Level &operator=(Level &&other);

    void load(const std::string &name);
    void draw(::Graphics::System &gr) const;

    /// Get the Y position of the nearest floor.
    float find_floor(FVec pos);
};

}
#endif
