/* Copyright 2014 Dietrich Epp.
   This file is part of Dreamless.  Dreamless is licensed under the terms
   of the 2-clause BSD license.  For more information, see LICENSE.txt. */
#include "scaler.hpp"
#include "common.hpp"
#include "sg/entry.h"
#include "sg/util.h"
namespace Graphics {

Scaler::Scaler()
    : m_width(0), m_height(0), m_tex(0), m_fbuf(0) {
    m_pattern = Base::Texture::load("misc/hilbert");
}

void Scaler::begin(int dest_width, int dest_height,
                   int src_width, int src_height) {
    if (src_width > m_fwidth || src_height > m_fheight) {
        if (m_tex) {
            glDeleteTextures(1, &m_tex);
            m_tex = 0;
        }

        if (m_fbuf) {
            glDeleteFramebuffers(1, &m_fbuf);
            m_fbuf = 0;
        }

        int fwidth = sg_round_up_pow2_32(src_width);
        int fheight = sg_round_up_pow2_32(src_height);

        glGenTextures(1, &m_tex);
        glBindTexture(GL_TEXTURE_2D, m_tex);
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
        glBindTexture(GL_TEXTURE_2D, 0);

        glGenFramebuffers(1, &m_fbuf);
        glBindFramebuffer(GL_FRAMEBUFFER, m_fbuf);
        glFramebufferTexture2D(
            GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_tex, 0);
        GLenum draw_buffers[1] = { GL_COLOR_ATTACHMENT0 };
        glDrawBuffers(1, draw_buffers);
        if (glCheckFramebufferStatus(GL_FRAMEBUFFER) !=
            GL_FRAMEBUFFER_COMPLETE)
            sg_sys_abort("cannot render to framebuffer");
        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        m_fwidth = fwidth;
        m_fheight = fheight;
        m_scale[0] = 1.0f / fwidth;
        m_scale[1] = 1.0f / fheight;
    }

    m_width = dest_width;
    m_height = dest_height;

    m_array.clear();
    float (*data)[4] = m_array.insert(6);
    float u = (float) dest_width * 0.5f, v = (float) dest_height * 0.5f;
    data[0][0] = -1.0f; data[0][1] = -1.0f; data[0][2] = 0; data[0][3] = 0;
    data[1][0] = +1.0f; data[1][1] = -1.0f; data[1][2] = u; data[1][3] = 0;
    data[2][0] = -1.0f; data[2][1] = +1.0f; data[2][2] = 0; data[2][3] = v;
    data[3][0] = -1.0f; data[3][1] = +1.0f; data[3][2] = 0; data[3][3] = v;
    data[4][0] = +1.0f; data[4][1] = -1.0f; data[4][2] = u; data[4][3] = 0;
    data[5][0] = +1.0f; data[5][1] = +1.0f; data[5][2] = u; data[5][3] = v;
    m_array.upload(GL_DYNAMIC_DRAW);

    glBindFramebuffer(GL_FRAMEBUFFER, m_fbuf);
    glViewport(0, 0, src_width, src_height);
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    sg_opengl_checkerror("Scaler::begin");
}

void Scaler::end(const CommonData &com) {
    auto &prog = com.m_television;

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glViewport(0, 0, m_width, m_height);
    glClearColor(1.0f, 0.0f, 1.0f, 0.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    glUseProgram(prog.prog());
    glEnableVertexAttribArray(prog->a_vert);
    glDisable(GL_BLEND);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, m_tex);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, m_pattern.tex);

    glUniform1i(prog->u_picture, 0);
    glUniform1i(prog->u_pattern, 1);
    glUniform2fv(prog->u_texscale, 1, m_scale);
    glUniform4f(prog->u_color, 0.0f, 0.0f, 0.0f, 0.0f);
    m_array.set_attrib(prog->a_vert);

    glDrawArrays(GL_TRIANGLES, 0, m_array.size());

    glDisableVertexAttribArray(prog->a_vert);
    glUseProgram(0);

    sg_opengl_checkerror("Scaler::end");
}

}
