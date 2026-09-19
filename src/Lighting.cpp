/*  ═══════════════════════════════════════════════════════════════
    Lighting.cpp  –  OpenGL fixed-function lighting setup

    CG Concepts:
      • Point light source (GL_LIGHT0) at the Sun (origin)
      • Ambient fill light (GL_LIGHT1) to prevent pitch-black dark sides
      • Attenuation (constant / linear / quadratic falloff)
      • Specular highlights
      • Global ambient light model
    ═══════════════════════════════════════════════════════════════ */

#include "Lighting.h"

void Lighting::setup(bool enabled) {
    if (enabled) {
        /* ── Primary light: Sun at origin ── */
        GLfloat sun_pos[]  = { 0.0f, 0.0f, 0.0f, 1.0f };  /* point light */
        GLfloat sun_amb[]  = { 0.03f, 0.03f, 0.02f, 1.0f };
        GLfloat sun_diff[] = { 1.00f, 0.95f, 0.85f, 1.0f }; /* warm sunlight */
        GLfloat sun_spec[] = { 1.00f, 0.95f, 0.80f, 1.0f };

        glEnable(GL_LIGHTING);
        glEnable(GL_LIGHT0);
        glLightfv(GL_LIGHT0, GL_POSITION, sun_pos);
        glLightfv(GL_LIGHT0, GL_AMBIENT,  sun_amb);
        glLightfv(GL_LIGHT0, GL_DIFFUSE,  sun_diff);
        glLightfv(GL_LIGHT0, GL_SPECULAR, sun_spec);

        /* Attenuation: reduces intensity with distance (physically based) */
        glLightf(GL_LIGHT0, GL_CONSTANT_ATTENUATION,  1.0f);
        glLightf(GL_LIGHT0, GL_LINEAR_ATTENUATION,    0.001f);
        glLightf(GL_LIGHT0, GL_QUADRATIC_ATTENUATION, 0.00005f);

        /* ── Fill light: very dim blue ambient from opposite direction ──
           Prevents the dark side of planets from being absolute black,
           simulating starlight and interplanetary glow.              */
        GLfloat fill_pos[]  = { 0.0f,  200.0f, 200.0f, 1.0f }; /* far away */
        GLfloat fill_amb[]  = { 0.0f,  0.0f,   0.0f,   1.0f };
        GLfloat fill_diff[] = { 0.04f, 0.04f,  0.07f,  1.0f }; /* very dim blue */
        GLfloat fill_spec[] = { 0.0f,  0.0f,   0.0f,   1.0f }; /* no specular */

        glEnable(GL_LIGHT1);
        glLightfv(GL_LIGHT1, GL_POSITION, fill_pos);
        glLightfv(GL_LIGHT1, GL_AMBIENT,  fill_amb);
        glLightfv(GL_LIGHT1, GL_DIFFUSE,  fill_diff);
        glLightfv(GL_LIGHT1, GL_SPECULAR, fill_spec);

        /* No attenuation on fill light — it's meant to be uniform */
        glLightf(GL_LIGHT1, GL_CONSTANT_ATTENUATION,  1.0f);
        glLightf(GL_LIGHT1, GL_LINEAR_ATTENUATION,    0.0f);
        glLightf(GL_LIGHT1, GL_QUADRATIC_ATTENUATION, 0.0f);

        /* ── Global scene ambient ── */
        GLfloat global_amb[] = { 0.02f, 0.02f, 0.03f, 1.0f };
        glLightModelfv(GL_LIGHT_MODEL_AMBIENT, global_amb);
        glLightModeli(GL_LIGHT_MODEL_LOCAL_VIEWER, GL_TRUE);

        glColorMaterial(GL_FRONT, GL_AMBIENT_AND_DIFFUSE);

    } else {
        glDisable(GL_LIGHT0);
        glDisable(GL_LIGHT1);
        glDisable(GL_LIGHTING);
    }
}
