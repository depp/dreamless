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
enum class Layer;
class AnySprite;

class System {
private:
    struct Data;
    std::unique_ptr<Data> m_data;

public:
    System();
    System(const System &) = delete;
    System(System &&) = delete;
    ~System();
    System &operator=(const System &) = delete;
    System &operator=(System &&) = delete;

    /// Clear graphics state.
    void clear(bool all);
    /// Finalize changes to the graphics state.
    void finalize();
    /// Draw the world.
    void draw();

    /// Set the render size.
    void set_size(int width, int height);
    /// Set the lower-left corner of the camera.
    void set_camera(Base::IVec pos);
    /// Set the current world, or in between.
    void set_world(float world);
    /// Add a sprite to the world.
    void add_sprite(AnySprite sp, Base::IVec pos,
                    Base::Orientation orientation,
                    Layer layer);
};

}
#endif
