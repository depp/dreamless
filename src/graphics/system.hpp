/* Copyright 2014 Dietrich Epp.
   This file is part of Dreamless.  Dreamless is licensed under the terms
   of the 2-clause BSD license.  For more information, see LICENSE.txt. */
#ifndef LD_GRAPHICS_SYSTEM_HPP
#define LD_GRAPHICS_SYSTEM_HPP
#include <memory>
#include <string>
namespace Base {
enum class Orientation;
struct IVec;
}
namespace Graphics {
enum class Layer;
class AnySprite;
struct Color;

enum class HAlign { LEFT, CENTER, RIGHT };
enum class VAlign { BOTTOM, CENTER, TOP };

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
    /// Set the noise offsets.
    void set_noise(float noise[4]);
    /// Add a sprite to the world.
    void add_sprite(AnySprite sp, Base::IVec pos, Layer layer,
                    Base::Orientation orientation);
    /// Add a sprite to the world.
    void add_sprite(AnySprite sp, Base::IVec pos, Layer layer);
    /// Put text on the screen.  Width is measured in pixels, use -1
    /// for unlimited width.  The point specified is on the edge of
    /// the text's bounding box.
    void put_text(Base::IVec pos, HAlign halign, VAlign valign, int width,
                  Color color, const std::string &str);
};

}
#endif
