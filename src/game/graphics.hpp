/* Copyright 2014 Dietrich Epp.
   This file is part of Dreamless.  Dreamless is licensed under the terms
   of the 2-clause BSD license.  For more information, see LICENSE.txt. */
#ifndef LD_GAME_GRAPHICS_HPP
#define LD_GAME_GRAPHICS_HPP
#include <memory>
namespace Base {
enum class Orientation;
}
namespace Graphics {
class AnySprite;
struct CommonData;
struct SpriteData;

class System {
private:
    std::unique_ptr<CommonData> m_common;
    std::unique_ptr<SpriteData> m_sprite;
    int m_width, m_height;

public:
    System();
    System(const System &) = delete;
    System(System &&) = delete;
    ~System();
    System &operator=(const System &) = delete;
    System &operator=(System &&) = delete;

    /// Begin updates to the graphics state.
    void begin(int width, int height);
    /// End updates to the graphics state.
    void end();
    /// Draw the world.
    void draw();
    /// Add a sprite to the world.
    void add_sprite(AnySprite sp, int x, int y,
                    Base::Orientation orientation,
                    bool screen=false);
};

}
#endif
