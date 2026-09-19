/*  ═══════════════════════════════════════════════════════════════
    Renderer.cpp  –  OpenGL initialisation and scene-wide rendering

    CG Concepts:
      • Depth buffering (GL_DEPTH_TEST)
      • Blending (GL_BLEND, additive and alpha modes)
      • Perspective projection (gluPerspective)
      • Orthographic projection (gluOrtho2D)
      • Point rendering (GL_POINTS, glPointSize)
      • Star field procedural generation (random spherical coords)
      • Nebula background quad
    ═══════════════════════════════════════════════════════════════ */

#include "Renderer.h"
#include "TextureManager.h"
#include <cstdlib>
#include <ctime>
#include <cmath>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

#define MAX_STARS 5000

Renderer::Renderer() : bgTextureID_(0), time_(0.0f) {
    std::srand((unsigned int)std::time(nullptr));
    generateStars(MAX_STARS);
}

Renderer::~Renderer() {}

void Renderer::initOpenGL() {
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);
    glEnable(GL_NORMALIZE);
    glEnable(GL_COLOR_MATERIAL);
    glShadeModel(GL_SMOOTH);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glEnable(GL_LINE_SMOOTH);
    glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
    glEnable(GL_POINT_SMOOTH);
    glHint(GL_POINT_SMOOTH_HINT, GL_NICEST);

    /* Perspective correction for texture mapping */
    glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);

    glClearColor(0.0f, 0.0f, 0.01f, 1.0f); /* near-perfect black deep space */

    bgTextureID_ = TextureManager::instance().load("textures/bg_nebula.png");
}

void Renderer::setPerspective(int width, int height) {
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(50.0,
                   (double)width / (double)height,
                   0.5,   /* near plane */
                   600.0  /* far plane  */);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}

void Renderer::setOrthographic(int width, int height) {
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(0, width, 0, height);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}

/* ── Star generation ──────────────────────────────────────────── */

void Renderer::generateStars(int count) {
    stars_.clear();
    stars_.reserve(count);

    for (int i = 0; i < count; i++) {
        Star s;

        /* Random point on sphere surface (radius 280–300 units) */
        float theta  = (float)rand() / RAND_MAX * 2.0f * (float)M_PI;
        float phi    = acosf(1.0f - 2.0f * (float)rand() / RAND_MAX); /* uniform */
        float radius = 280.0f + (float)rand() / RAND_MAX * 20.0f;

        s.x = radius * sinf(phi) * cosf(theta);
        s.y = radius * sinf(phi) * sinf(theta);
        s.z = radius * cosf(phi);

        /* Size / class distribution */
        int roll = rand() % 100;
        if      (roll < 60) s.cls = STAR_SMALL;
        else if (roll < 88) s.cls = STAR_MEDIUM;
        else                s.cls = STAR_LARGE;

        s.brightness  = 0.45f + (float)rand() / RAND_MAX * 0.55f;
        s.twinkleFreq = 0.5f  + (float)rand() / RAND_MAX * 3.0f;
        s.twinklePhase= (float)rand() / RAND_MAX * 2.0f * (float)M_PI;

        /* Star colour by spectral class */
        switch (s.cls) {
            case STAR_SMALL:  /* white / yellow-white */
                s.size = 1.0f;
                s.r = s.brightness;
                s.g = s.brightness;
                s.b = s.brightness * 0.9f;
                break;
            case STAR_MEDIUM: /* blue-white */
                s.size = 1.8f;
                s.r = s.brightness * 0.80f;
                s.g = s.brightness * 0.88f;
                s.b = s.brightness * 1.00f;
                break;
            case STAR_LARGE:  /* orange / red giant */
                s.size = 2.8f;
                s.r = s.brightness * 1.00f;
                s.g = s.brightness * 0.72f;
                s.b = s.brightness * 0.35f;
                break;
        }
        stars_.push_back(s);
    }
}

/* ── Star background draw ─────────────────────────────────────── */

void Renderer::drawStarBackground() {
    glPushMatrix();
    glDisable(GL_LIGHTING);
    glDisable(GL_DEPTH_TEST);
    glDepthMask(GL_FALSE);

    /* ── Nebula background ── */
    if (bgTextureID_) {
        glEnable(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, bgTextureID_);
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glColor4f(1.0f, 1.0f, 1.0f, 0.35f); /* dim so stars show through */

        /* Draw on all 6 faces of a large cube to cover all directions */
        float d = 399.0f;

        /* Back face */
        glBegin(GL_QUADS);
        glTexCoord2f(0,0); glVertex3f(-d,-d,-d);
        glTexCoord2f(1,0); glVertex3f( d,-d,-d);
        glTexCoord2f(1,1); glVertex3f( d, d,-d);
        glTexCoord2f(0,1); glVertex3f(-d, d,-d);
        glEnd();
        /* Front face */
        glBegin(GL_QUADS);
        glTexCoord2f(0,0); glVertex3f(-d,-d, d);
        glTexCoord2f(1,0); glVertex3f( d,-d, d);
        glTexCoord2f(1,1); glVertex3f( d, d, d);
        glTexCoord2f(0,1); glVertex3f(-d, d, d);
        glEnd();
        /* Left face */
        glBegin(GL_QUADS);
        glTexCoord2f(0,0); glVertex3f(-d,-d,-d);
        glTexCoord2f(1,0); glVertex3f(-d,-d, d);
        glTexCoord2f(1,1); glVertex3f(-d, d, d);
        glTexCoord2f(0,1); glVertex3f(-d, d,-d);
        glEnd();
        /* Right face */
        glBegin(GL_QUADS);
        glTexCoord2f(0,0); glVertex3f( d,-d,-d);
        glTexCoord2f(1,0); glVertex3f( d,-d, d);
        glTexCoord2f(1,1); glVertex3f( d, d, d);
        glTexCoord2f(0,1); glVertex3f( d, d,-d);
        glEnd();
        /* Top face */
        glBegin(GL_QUADS);
        glTexCoord2f(0,0); glVertex3f(-d, d,-d);
        glTexCoord2f(1,0); glVertex3f( d, d,-d);
        glTexCoord2f(1,1); glVertex3f( d, d, d);
        glTexCoord2f(0,1); glVertex3f(-d, d, d);
        glEnd();
        /* Bottom face */
        glBegin(GL_QUADS);
        glTexCoord2f(0,0); glVertex3f(-d,-d,-d);
        glTexCoord2f(1,0); glVertex3f( d,-d,-d);
        glTexCoord2f(1,1); glVertex3f( d,-d, d);
        glTexCoord2f(0,1); glVertex3f(-d,-d, d);
        glEnd();

        glDisable(GL_TEXTURE_2D);
        glDisable(GL_BLEND);
    }

    /* ── Stars ── */
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    /* Time-based twinkling */
    time_ += 0.016f; /* ~60 fps tick, good enough for twinkle */

    float sizes[]   = { 1.0f, 2.0f, 3.2f };
    StarClass cls[] = { STAR_SMALL, STAR_MEDIUM, STAR_LARGE };

    for (int pass = 0; pass < 3; pass++) {
        glPointSize(sizes[pass]);
        glBegin(GL_POINTS);
        for (const auto& s : stars_) {
            if (s.cls != cls[pass]) continue;

            /* Sine twinkle: modulate brightness ±15% */
            float twinkle = 1.0f + 0.15f * sinf(time_ * s.twinkleFreq + s.twinklePhase);
            float b = s.brightness * twinkle;
            if (b > 1.0f) b = 1.0f;

            glColor4f(s.r * twinkle, s.g * twinkle, s.b * twinkle, b);
            glVertex3f(s.x, s.y, s.z);
        }
        glEnd();
    }

    glPointSize(1.0f);
    glDisable(GL_BLEND);
    glDepthMask(GL_TRUE);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_LIGHTING);
    glPopMatrix();
}
