/* Copyright 2014 Dietrich Epp.
   This file is part of Dreamless.  Dreamless is licensed under the terms
   of the 2-clause BSD license.  For more information, see LICENSE.txt. */
#ifndef LD_GRAPHICS_SYSTEM_HPP
#define LD_GRAPHICS_SYSTEM_HPP
#include <memory>
namespace Base {
enum class Orientation;
class IVec;
}
namespace Graphics {
class AnySprite;
class CommonData;
class SpriteLayer;
class Scaler;
enum class Layer;

class System {
private:
    std::unique_ptr<CommonData> m_common;
    std::unique_ptr<SpriteLayer> m_sprite;
    std::unique_ptr<Scaler> m_scaler;
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
    void add_sprite(AnySprite sp, Base::IVec pos,
                    Base::Orientation orientation,
                    Layer layer);
};

}
#endif
