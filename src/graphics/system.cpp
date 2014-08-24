/* Copyright 2014 Dietrich Epp.
   This file is part of Dreamless.  Dreamless is licensed under the terms
   of the 2-clause BSD license.  For more information, see LICENSE.txt. */
#include "system.hpp"

#include "color.hpp"
#include "layer.hpp"
#include "shader.hpp"
#include "sprite.hpp"

#include "base/array.hpp"
#include "base/image.hpp"
#include "base/log.hpp"
#include "base/shader.hpp"
#include "base/sprite.hpp"
#include "base/vec.hpp"

#include "sg/util.h"

#include <cstring>

namespace Graphics {

using Base::IVec;
using Base::IRect;
using Base::FVec;
using Base::FRect;
using Base::Log;
using Base::Array;
using Base::Program;
using Base::SpriteSheet;
using Base::SpriteArray;
using Base::Orientation;
using Base::Texture;

namespace {

/// Render targets
static const int TARGET_COUNT = 5;
enum class Target {
    TILE_1,
    TILE_2,
    SPRITE_1,
    SPRITE_2,
    COMPOSITE
};

void make_xform(float *xform, IVec origin, int width, int height) {
    xform[0] = 2.0 / width;
    xform[1] = 2.0 / height;
    xform[2] = -1.0 - xform[0] * origin.x;
    xform[3] = -1.0 - xform[1] * origin.y;
}

void make_fullscreen_quad(Array<float[4]> &arr, FRect tex) {
    arr.clear();
    float (*data)[4] = arr.insert(6);
    float u0 = tex.x0, u1 = tex.x1, v0 = tex.y0, v1 = tex.y1;
    data[0][0] = -1.0f; data[0][1] = -1.0f; data[0][2] = u0; data[0][3] = v0;
    data[1][0] = +1.0f; data[1][1] = -1.0f; data[1][2] = u1; data[1][3] = v0;
    data[2][0] = -1.0f; data[2][1] = +1.0f; data[2][2] = u0; data[2][3] = v1;
    data[3][0] = -1.0f; data[3][1] = +1.0f; data[3][2] = u0; data[3][3] = v1;
    data[4][0] = +1.0f; data[4][1] = -1.0f; data[4][2] = u1; data[4][3] = v0;
    data[5][0] = +1.0f; data[5][1] = +1.0f; data[5][2] = u1; data[5][3] = v1;
    arr.upload(GL_DYNAMIC_DRAW);
}

}

struct System::Data {
    // Shader programs
    Program<Shader::Sprite> m_prog_sprite;
    Program<Shader::Composite> m_prog_composite;
    Program<Shader::Scale> m_prog_scale;

    // Target textures and framebuffers.
    int m_target_width, m_target_height;
    GLuint m_target_tex[TARGET_COUNT];
    GLuint m_target_fbuf[TARGET_COUNT];

    // Scale from layer texture coordinates to pixel coordinates.
    float m_pixscale[2];
    // Scale for the blending effect.
    float m_blendscale[4];

    // Sprite transformation from screen or world coordinates.
    float m_xform_screen[4];
    float m_xform_world[4];

    // Array for layer composition.
    Array<float[4]> m_array_composite;
    // Array for scaling the composite to the screen.
    Array<float[4]> m_array_scale;

    // Sprite data
    SpriteSheet m_sprite_sheet;
    SpriteArray m_sprite_array[LAYER_COUNT];

    // The blend effect color.
    Color m_blendcolor;

    // The final width and height.
    int m_width, m_height;

    // The camera position.
    IVec m_camera;

    // The current world.
    float m_world;

    // Textures
    Texture m_pattern;

    Data();

    // ============================================================

    // Set up all layer targets.
    void target_finalize();

    // Set the current rendering target.
    void target_set(Target target);

    // Get the target's texture.
    GLuint target_texture(Target target);

    // ============================================================

    // Get a sprite array.
    SpriteArray &sprite_array(Layer layer);

    // Clear sprite layers.
    void sprite_clear(bool all);

    // Add a sprite.
    void sprite_add(AnySprite sp, IVec pos,
                    Orientation orientation,
                    Layer layer);

    // Upload sprites.
    void sprite_finalize();

    // Draw a sprite layer.
    void sprite_draw(Layer layer);

    // ============================================================

    void draw_layers();

    void draw_composite();

    void draw_scaled();

    void draw();
};

// ============================================================

System::Data::Data()
    : m_prog_sprite("sprite", "sprite"),
      m_prog_composite("composite", "composite"),
      m_prog_scale("scale", "scale"),
      m_target_width(-1), m_target_height(-1),
      m_sprite_sheet("", SPRITES),
      m_blendcolor(Color::transparent()),
      m_width(-1), m_height(-1),
      m_camera(IVec::zero()),
      m_world(0.0f) {
    std::memset(m_target_tex, 0, sizeof(m_target_tex));
    std::memset(m_target_fbuf, 0, sizeof(m_target_fbuf));
    m_pattern = Texture::load("misc/hilbert");
}

// ============================================================

void System::Data::target_finalize() {
    static const int MARGIN = 64;
    int width = m_width / 2, height = m_height / 2;

    if (width + MARGIN * 2 > m_target_width ||
        height + MARGIN * 2 > m_target_height) {
        glDeleteTextures(TARGET_COUNT, m_target_tex);
        glDeleteFramebuffers(TARGET_COUNT, m_target_fbuf);
        int fwidth = sg_round_up_pow2_32(width + MARGIN * 2);
        int fheight = sg_round_up_pow2_32(height + MARGIN * 2);
        glGenTextures(TARGET_COUNT, m_target_tex);
        glGenFramebuffers(TARGET_COUNT, m_target_fbuf);

        for (int i = 0; i < TARGET_COUNT; i++) {
            glBindTexture(GL_TEXTURE_2D, m_target_tex[i]);
            glTexImage2D(
                GL_TEXTURE_2D,
                0,
                GL_RGBA,
                fwidth,
                fheight,
                0,
                GL_RGBA,
                GL_UNSIGNED_BYTE,
                nullptr);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

            glBindFramebuffer(GL_FRAMEBUFFER, m_target_fbuf[i]);
            glFramebufferTexture2D(
                GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D,
                m_target_tex[i], 0);
            GLenum draw_buffers[1] = { GL_COLOR_ATTACHMENT0 };
            glDrawBuffers(1, draw_buffers);
            if (glCheckFramebufferStatus(GL_FRAMEBUFFER) !=
                GL_FRAMEBUFFER_COMPLETE)
                Log::abort("cannot render to framebuffer");
        }

        glBindTexture(GL_TEXTURE_2D, 0);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        m_target_width = fwidth;
        m_target_height = fheight;
        m_pixscale[0] = m_target_width;
        m_pixscale[1] = m_target_height;
    }

    IVec texpos((m_target_width - width) / 2,
                (m_target_height - height) / 2);
    make_xform(m_xform_world, m_camera - texpos,
               m_target_width, m_target_height);
    make_xform(m_xform_screen, IVec::zero() - texpos,
               m_target_width, m_target_height);

    make_fullscreen_quad(
        m_array_composite,
        FRect(0.0f, 0.0f, 1.0f, 1.0f));

    float xs = 1.0f / m_target_width, ys = 1.0 / m_target_height;
    make_fullscreen_quad(
        m_array_scale,
        FRect(texpos.x * xs,
             texpos.y * ys,
             (texpos.x + width) * xs,
             (texpos.y + height) * ys));

    {
        float x = 0.5f * xs * (float) width;
        float y = 0.5f * ys * (float) height;
        float sc = std::sqrt(1.0f / (x * x + y * y));
        m_blendscale[0] = (float) texpos.x * xs + x;
        m_blendscale[1] = (float) texpos.y * ys + y;
        m_blendscale[2] = sc;
        m_blendscale[3] = sc;
    }

    sg_opengl_checkerror("TargetManager::set_camera");
}

void System::Data::target_set(Target target) {
    glBindFramebuffer(GL_FRAMEBUFFER,
                      m_target_fbuf[static_cast<int>(target)]);
    glViewport(0, 0, m_target_width, m_target_height);
}

GLuint System::Data::target_texture(Target target) {
    return m_target_tex[static_cast<int>(target)];
}

// ============================================================

SpriteArray &System::Data::sprite_array(Layer layer) {
    return m_sprite_array[static_cast<int>(layer)];
}

void System::Data::sprite_clear(bool all) {
    if (all) {
        sprite_array(Layer::TILE_1).clear();
        sprite_array(Layer::TILE_2).clear();
    }
    sprite_array(Layer::SPRITE_1).clear();
    sprite_array(Layer::SPRITE_2).clear();
    sprite_array(Layer::INTERFACE).clear();
}

void System::Data::sprite_add(AnySprite sp, IVec pos,
                              Orientation orientation,
                              Layer layer) {
    sprite_array(layer).add(
        m_sprite_sheet.get(static_cast<int>(sp)),
        pos.x, pos.y, orientation);
}

void System::Data::sprite_finalize() {
    for (int i = 0; i < LAYER_COUNT; i++)
        m_sprite_array[i].upload(GL_DYNAMIC_DRAW);
}

void System::Data::sprite_draw(Layer layer) {
    auto &prog = m_prog_sprite;
    auto &arr = sprite_array(layer);

    if (arr.empty())
        return;

    glUseProgram(prog.prog());
    glEnableVertexAttribArray(prog->a_vert);
    glEnable(GL_BLEND);
    glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, m_sprite_sheet.texture());

    glUniform2fv(prog->u_texscale, 1, m_sprite_sheet.texscale());
    glUniform1i(prog->u_texture, 0);

    const float *xform = nullptr;
    switch (layer) {
    case Layer::TILE_1:
    case Layer::TILE_2:
    case Layer::SPRITE_1:
    case Layer::SPRITE_2:
        xform = m_xform_world;
        break;
    case Layer::INTERFACE:
        xform = m_xform_screen;
        break;
    }
    glUniform4fv(prog->u_vertxform, 1, xform);

    arr.set_attrib(prog->a_vert);

    glDrawArrays(GL_TRIANGLES, 0, arr.size());

    glUseProgram(0);
    sg_opengl_checkerror("System::Data::sprite_draw");
}

// ============================================================

void System::Data::draw_layers() {
    target_set(Target::TILE_1);
    glClear(GL_COLOR_BUFFER_BIT);
    sprite_draw(Layer::TILE_1);

    target_set(Target::TILE_2);
    glClear(GL_COLOR_BUFFER_BIT);
    sprite_draw(Layer::TILE_2);

    target_set(Target::SPRITE_1);
    glClear(GL_COLOR_BUFFER_BIT);
    sprite_draw(Layer::SPRITE_1);

    target_set(Target::SPRITE_2);
    glClear(GL_COLOR_BUFFER_BIT);
    sprite_draw(Layer::SPRITE_2);
}

void System::Data::draw_composite() {
    auto &prog = m_prog_composite;
    auto &arr = m_array_composite;

    target_set(Target::COMPOSITE);

    glUseProgram(prog.prog());
    glEnableVertexAttribArray(prog->a_vert);
    glDisable(GL_BLEND);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, target_texture(Target::TILE_1));
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, target_texture(Target::TILE_2));
    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, target_texture(Target::SPRITE_1));
    glActiveTexture(GL_TEXTURE3);
    glBindTexture(GL_TEXTURE_2D, target_texture(Target::SPRITE_2));

    glUniform1i(prog->u_tile1, 0);
    glUniform1i(prog->u_tile2, 1);
    glUniform1i(prog->u_sprite1, 2);
    glUniform1i(prog->u_sprite2, 3);

    glUniform1f(prog->u_world, m_world);
    glUniform4fv(prog->u_color, 1, m_blendcolor.v);
    glUniform4fv(prog->u_blendscale, 1, m_blendscale);

    arr.set_attrib(prog->a_vert);

    glDrawArrays(GL_TRIANGLES, 0, arr.size());

    glUseProgram(0);
    sg_opengl_checkerror("System::Data::draw_composite");

    sprite_draw(Layer::INTERFACE);
}

void System::Data::draw_scaled() {
    auto &prog = m_prog_scale;
    auto &arr = m_array_scale;

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glViewport(0, 0, m_width, m_height);

    glUseProgram(prog.prog());
    glEnableVertexAttribArray(prog->a_vert);
    glDisable(GL_BLEND);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, target_texture(Target::COMPOSITE));
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, m_pattern.tex);

    glUniform1i(prog->u_picture, 0);
    glUniform1i(prog->u_pattern, 1);
    glUniform2fv(prog->u_pixscale, 1, m_pixscale);

    arr.set_attrib(prog->a_vert);

    glDrawArrays(GL_TRIANGLES, 0, arr.size());

    glUseProgram(0);
    sg_opengl_checkerror("System::Data::draw_scaled");
}

void System::Data::draw() {
}

// ============================================================

System::System()
    : m_data(new Data)
{ }

System::~System()
{ }

void System::clear(bool all) {
    auto &d = *m_data;
    d.sprite_clear(all);
}

void System::finalize() {
    auto &d = *m_data;
    d.target_finalize();
    d.sprite_finalize();
}

void System::draw() {
    auto &d = *m_data;
    d.draw_layers();
    d.draw_composite();
    d.draw_scaled();
}

void System::set_size(int width, int height) {
    auto &d = *m_data;
    d.m_width = width;
    d.m_height = height;
}

void System::set_camera(IVec pos) {
    auto &d = *m_data;
    d.m_camera = pos;
}

void System::set_world(float world) {
    auto &d = *m_data;
    d.m_world = world;
}

void System::add_sprite(AnySprite sp, IVec pos,
                        Orientation orientation, Layer layer) {
    m_data->sprite_add(sp, pos, orientation, layer);
}

}
