#ifndef LIGHTING_H
#define LIGHTING_H

/*  ═══════════════════════════════════════════════════════════════
    Lighting  –  OpenGL lighting setup
    CG Concepts: Ambient/Diffuse/Specular lighting, attenuation
    ═══════════════════════════════════════════════════════════════ */

#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

class Lighting {
public:
    static void setup(bool enabled);
};

#endif /* LIGHTING_H */
