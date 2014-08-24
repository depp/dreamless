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
    { 'P', 32, SpawnType::PLAYER },
    { 'M', 32, SpawnType::MINION },
    { 'N', 32, SpawnType::MINION_LEFT },
    { 'D', 48, SpawnType::DOOR_CLOSED },
    { 'L', 48, SpawnType::DOOR_LOCKED },
    { 'K', 32, SpawnType::KEY },
    { 'G', 32, SpawnType::GATEWAY },

    { '\0', 0, SpawnType::PLAYER }
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

namespace {

typedef std::pair<const char *, int> Line;

std::vector<Line> read_lines(Base::Data &filedata) {
    std::vector<Line> lines;
    const char *ptr = static_cast<const char *>(filedata.ptr());
    const char *end = ptr + filedata.size();
    while (ptr != end) {
        const char *nl = static_cast<const char *>(
            memchr(ptr, '\n', end - ptr));
        const char *eol = nl ? nl : end;
        while (eol != ptr && eol[-1] == ' ')
            eol--;
        lines.push_back(Line(ptr, eol - ptr));
        if (nl)
            ptr = nl +1;
        else
            break;
    }
    return lines;
}

std::vector<Line>::const_iterator find_break(
    const std::vector<Line> &lines) {
    auto lp = lines.begin(), le = lines.end();
    for (; lp != le; lp++) {
        if (lp->second > 1 && lp->first[0] == '-')
            return lp;
    }
    Log::abort("Could not find level delimiter");
    return le;
}

}

void Level::load(const std::string &name) {
    Log::info("loading level %s", name.c_str());

    static size_t MAX_SIZE = 1024 * 32;
    std::string path("level/");
    path += name;
    path += ".txt";
    Base::Data filedata;
    filedata.read(path, MAX_SIZE);

    auto lines = read_lines(filedata);

    if (m_data)
        delete[] m_data;
    m_width = 0;
    m_height = 0;
    m_data = nullptr;

    {
        for (int i = 0; i < ACTION_COUNT; i++)
            m_action[i] = false;
        auto first_break = find_break(lines);
        std::string white(" ");
        for (auto lp = lines.begin(); lp != first_break; lp++) {
            std::string line(lp->first, lp->second);
            auto i = line.find(':');
            if (i == std::string::npos)
                Log::abort("invalid line: '%s'", line.c_str());
            auto name = line.substr(0, i);
            auto j = line.find_first_not_of(white, i + 1);
            std::string data;
            if (j != std::string::npos)
                data = line.substr(j);
            if (name == "actions") {
                for (auto c : data) {
                    Action action;
                    switch (c) {
                    case 'j': case 'J': action = Action::JUMP; break;
                    case 'b': case 'B': action = Action::JUMP_BACK; break;
                    case 't': case 'T': action = Action::TURN; break;
                    case 'd': case 'D': action = Action::DROP; break;
                    default:
                        Log::abort("unknown action: '%c'", c);
                        return;
                    }
                    m_action[static_cast<int>(action)] = true;
                }
            } else {
                Log::abort("unknown property: %s", name.c_str());
            }
        }
        lines.erase(lines.begin(), first_break + 1);
        lines.erase(find_break(lines), lines.end());
    }

    int height = lines.size();
    int width = 0;
    for (auto &line : lines) {
        if (line.second > width)
            width = line.second;
    }

    Log::info("level size: %d x %d", width, height);

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
                    pt.pos = IVec(x, y);
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

    for (auto &sp : m_spawn) {
        IVec pos = sp.pos;
        int dy;
        switch (tile_at(pos + IVec(0, -1)).type) {
        case TileType::RAMP_R1:
        case TileType::RAMP_L2:
            dy = -24;
            break;
        case TileType::RAMP_R2:
        case TileType::RAMP_L1:
            dy = -8;
            break;
        default:
            dy = 0;
            break;
        }
        const SpawnInfo *pi = SPAWN;
        for (; pi->c != '\0' && pi->type != sp.type; pi++) { }
        dy += pi->height / 2;
        sp.pos = IVec(
            sp.pos.x * Defs::TILESZ + Defs::TILESZ / 2,
            sp.pos.y * Defs::TILESZ + dy);
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
                gr.add_sprite(tile, pos, Layer::TILE);
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
