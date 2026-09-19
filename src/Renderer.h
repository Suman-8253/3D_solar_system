#ifndef RENDERER_H
#define RENDERER_H

/*  ═══════════════════════════════════════════════════════════════
    Renderer  –  OpenGL initialisation and scene-wide rendering
    CG Concepts: Depth testing, double buffering, blending,
                 projection matrices, star field generation
    ═══════════════════════════════════════════════════════════════ */

#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif
#include <vector>

enum StarClass {
    STAR_SMALL  = 0,
    STAR_MEDIUM = 1,
    STAR_LARGE  = 2
};

struct Star {
    float x, y, z;          /* Position on sphere surface */
    float r, g, b;          /* Base colour */
    float brightness;        /* Base brightness [0..1] */
    float size;              /* Point size in pixels */
    float twinkleFreq;       /* Oscillation frequency (rad/s) */
    float twinklePhase;      /* Phase offset for variety */
    StarClass cls;
};

class Renderer {
public:
    Renderer();
    ~Renderer();

    void initOpenGL();
    void setPerspective(int width, int height);
    void setOrthographic(int width, int height);

    void drawStarBackground();

private:
    std::vector<Star> stars_;
    GLuint bgTextureID_;
    float  time_;             /* Accumulated time for twinkling */

    void generateStars(int count);
};

#endif /* RENDERER_H */
