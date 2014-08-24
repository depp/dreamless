/* Copyright 2014 Dietrich Epp.
   This file is part of Dreamless.  Dreamless is licensed under the terms
   of the 2-clause BSD license.  For more information, see LICENSE.txt. */
#ifndef LD_GAME_LEVEL_HPP
#define LD_GAME_LEVEL_HPP
#include "defs.hpp"
#include "action.hpp"
#include <string>
#include <vector>
namespace Graphics {
class System;
}
namespace Game {

/// Level data.
class Level {
public:
    enum class SpawnType {
        PLAYER,
        MINION,
        MINION_LEFT,
        DOOR_CLOSED,
        DOOR_LOCKED,
        KEY,
        GATEWAY
    };

    struct SpawnPoint {
        SpawnType type;
        IVec pos;
    };

private:
    struct SpawnInfo {
        unsigned char c;
        int height;
        SpawnType type;
    };

    enum class TileType {
        /// Free movement.
        OPEN,

        /// Completely solid.
        SOLID,
        RAMP_R1, RAMP_R2, RAMP_L1, RAMP_L2
    };

    struct TileInfo {
        unsigned char c;
        Tile tile;
        TileType type;
    };

    static bool is_initialized;
    static const SpawnInfo SPAWN[];
    static const TileInfo TILES_RAW[];
    static const TileInfo TILE_SOLID;
    static TileInfo TILES[256];

    int m_width;
    int m_height;
    unsigned char *m_data;
    std::vector<SpawnPoint> m_spawn;
    bool m_action[ACTION_COUNT];

public:
    Level();
    Level(const Level &) = delete;
    Level(Level &&other);
    ~Level();
    Level &operator=(const Level &) = delete;
    Level &operator=(Level &&other);

    void load(const std::string &name);
    void draw(::Graphics::System &gr) const;

    /// Test whether a point hits the level.
    bool hit_test(FVec pos) const;
    /// Get the Y position of the nearest floor.
    float find_floor(FVec pos) const;

    /// Get the pixel bounds of the level.
    IRect bounds() const {
        return IRect(0, 0, m_width * Defs::TILESZ, m_height * Defs::TILESZ);
    }

    /// Get a list of all spawn points.
    const std::vector<SpawnPoint> &spawn_points() const { return m_spawn; }

    bool is_action_allowed(Action action) const {
        return m_action[static_cast<int>(action)];
    }

private:
    static const TileInfo &tile_info(unsigned char tile) {
        return TILES[tile];
    }

    const TileInfo &tile_at(IVec pos) const {
        if (pos.x < 0 || pos.y < 0 || pos.x >= m_width || pos.y >= m_height)
            return TILE_SOLID;
        return tile_info(m_data[m_width * pos.y + pos.x]);
    }

    static float tile_floor(TileType type, float relx);
    float tile_floor(IVec pos, float relx) const;
};

}
#endif
