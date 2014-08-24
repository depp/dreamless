/* Copyright 2014 Dietrich Epp.
   This file is part of Dreamless.  Dreamless is licensed under the terms
   of the 2-clause BSD license.  For more information, see LICENSE.txt. */
#include "level.hpp"
#include "defs.hpp"
#include "base/file.hpp"
#include "graphics/layer.hpp"
#include "graphics/sprite.hpp"
#include "graphics/system.hpp"
#include <cstring>
#include <cstdio>
#include <vector>
namespace Game {

bool Level::is_initialized;

const Level::SpawnInfo Level::SPAWN[] = {
    { 'P', SpawnType::PLAYER },
    { 'M', SpawnType::MINION },

    { '\0', SpawnType::PLAYER }
};

const Level::TileInfo Level::TILES_RAW[] = {
    { ' ', Tile::NONE,   TileType::OPEN    },
    { '#', Tile::REAL_1, TileType::SOLID   },
    { 'a', Tile::REAL_7, TileType::RAMP_R1 },
    { 'b', Tile::REAL_8, TileType::RAMP_R2 },
    { 'c', Tile::REAL_6, TileType::RAMP_L1 },
    { 'd', Tile::REAL_5, TileType::RAMP_L2 },

    { '\0', Tile::NONE,  TileType::OPEN  }
};

const Level::TileInfo Level::TILE_SOLID = {
    '\0', Tile::NONE, TileType::SOLID
};

Level::TileInfo Level::TILES[256];

float Level::tile_floor(TileType type, float relx) {
    (void) relx;
    switch (type) {
    case TileType::OPEN:
        return 0.0f;
    case TileType::SOLID:
        return 32.0f;
    case TileType::RAMP_R1:
        return relx * 0.5f;
    case TileType::RAMP_R2:
        return relx * 0.5f + 16.0f;
    case TileType::RAMP_L1:
        return relx * -0.5f + 32.0f;
    case TileType::RAMP_L2:
        return relx * -0.5f + 16.0f;
    }
    Log::abort("invalid tile type");
    return 0.0f;
}

float Level::tile_floor(IVec pos, float relx) const {
    auto &info = tile_at(pos);
    return tile_floor(info.type, relx);
}

Level::Level()
    : m_width(0), m_height(0), m_data(nullptr) {
    if (!is_initialized) {
        const TileInfo *p = TILES_RAW;
        for (; p->c != '\0'; p++)
            TILES[(unsigned char) p->c] = *p;
        is_initialized = true;
    }
}

Level::Level(Level &&other)
    : m_width(other.m_width),
      m_height(other.m_height),
      m_data(other.m_data) {
    other.m_width = 0;
    other.m_height = 0;
    other.m_data = nullptr;
}

Level::~Level() {
    if (m_data)
        delete[] m_data;
}

Level &Level::operator=(Level &&other) {
    int width = other.m_width, height = other.m_height;
    unsigned char *data = other.m_data;
    other.m_width = 0;
    other.m_height = 0;
    other.m_data = nullptr;
    if (m_data)
        delete[] m_data;
    m_width = width;
    m_height = height;
    m_data = data;
    return *this;
}

void Level::load(const std::string &name) {
    Log::info("loading level %s", name.c_str());

    static size_t MAX_SIZE = 1024 * 32;
    std::string path("level/");
    path += name;
    path += ".txt";
    Base::Data filedata;
    filedata.read(path, MAX_SIZE);

    typedef std::pair<const char *, int> Line;
    std::vector<Line> lines;
    {
        const char *ptr = static_cast<const char *>(filedata.ptr());
        const char *end = ptr + filedata.size();
        while (ptr != end) {
            const char *eol = static_cast<const char *>(
                memchr(ptr, '\n', end - ptr));
            if (!eol) {
                lines.push_back(Line(ptr, end - ptr));
                break;
            }
            lines.push_back(Line(ptr, eol - ptr));
            ptr = eol + 1;
        }
    }

    int height = lines.size();
    int width = 0;
    for (size_t i = 0; i < lines.size(); i++) {
        if (lines[i].second > 0 && lines[i].first[0] == '-') {
            height = i;
            break;
        }
        if (lines[i].second > width)
            width = lines[i].second;
    }

    Log::info("level size: %d x %d", width, height);

    if (m_data)
        delete[] m_data;
    m_width = 0;
    m_height = 0;
    m_data = nullptr;

    unsigned char *data = new unsigned char[height * width];
    bool errors = false;
    m_width = width;
    m_height = height;
    m_data = data;
    for (int y = 0; y < height; y++) {
        Line line = lines[height - 1 - y];
        int x = 0;
        for (; x < line.second; x++) {
            unsigned char c = line.first[x];
            auto &info = tile_info(c);
            if (info.c == '\0') {
                const SpawnInfo *pi = SPAWN;
                for (; pi->c != '\0' && pi->c != c; pi++) { }
                if (pi->c == c) {
                    SpawnPoint pt;
                    pt.type = pi->type;
                    pt.pos = IVec(
                        x * Defs::TILESZ + Defs::TILESZ / 2,
                        y * Defs::TILESZ + Defs::TILESZ / 2);
                    m_spawn.push_back(pt);
                } else {
                    Log::error("bad tile at (%d, %d): '%c'", x, y, c);
                    errors = true;
                }
                c = ' ';
            }
            data[y * width + x] = c;
        }
        for (; x < width; x++)
            data[y * width + x] = ' ';
    }
    if (errors)
        Log::abort("level contains errors");
}

void Level::draw(::Graphics::System &gr) const {
    (void) &gr;
    int width = m_width, height = m_height;
    const unsigned char *data = m_data;
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            auto &info = tile_info(data[y * width + x]);
            auto tile = info.tile;
            IVec pos(32 * x + 16, 32 * y + 16);
            if (tile != Tile::NONE)
                gr.add_sprite(tile, pos, Orientation::NORMAL, Layer::World1);
        }
    }
}

bool Level::hit_test(FVec pos) const {
    IVec tile = Defs::tile_pos(pos);
    FVec rel = Defs::tile_relpos(pos);
    float floor = tile_floor(tile, rel.x);
    return floor > rel.y;
}

float Level::find_floor(FVec pos) const {
    IVec tile = Defs::tile_pos(pos);
    FVec rel = Defs::tile_relpos(pos);
    float floor = tile_floor(tile, rel.x);
    if (floor <= 0)
        floor = tile_floor(tile + IVec(0, -1), rel.x) - Defs::TILESZ;
    else if (floor >= Defs::TILESZ)
        floor = tile_floor(tile + IVec(0, 1), rel.x) + Defs::TILESZ;
    return floor + Defs::TILESZ * tile.y;
}

}
