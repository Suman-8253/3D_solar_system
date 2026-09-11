#ifndef RENDERER_H
#define RENDERER_H

#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif
#include <stdbool.h>

#define MAX_STARS 3000

typedef struct Planet   Planet;
typedef struct Camera   Camera;

/* Star classification for visual variety */
typedef enum {
    STAR_SMALL  = 0,   /* tiny white     – majority  */
    STAR_MEDIUM = 1,   /* medium blue-white           */
    STAR_LARGE  = 2    /* large warm yellow/orange    */
} StarClass;

typedef struct {
    float     x, y, z;
    float     r, g, b;
    float     brightness;
    float     size;
    StarClass cls;
} Star;

typedef struct {
    Star stars[MAX_STARS];
    int  numStars;
} Renderer;

/* Lifecycle */
void Renderer_init(Renderer* r);
void Renderer_destroy(Renderer* r);

/* OpenGL state */
void Renderer_initOpenGL(Renderer* r);
void Renderer_setupLighting(Renderer* r, bool lightingEnabled);
void Renderer_setPerspective(Renderer* r, int width, int height);
void Renderer_setOrthographic(Renderer* r, int width, int height);

/* Stars */
void Renderer_generateStars(Renderer* r, int count);
void Renderer_drawStarBackground(Renderer* r);

/* 2D text / HUD */
void Renderer_drawText(Renderer* r, float x, float y, const char* text, void* font);
void Renderer_drawHUD(Renderer* r, int width, int height,
                      Planet* selectedPlanet,
                      float speedMult, bool isPaused,
                      bool orbitsEnabled, bool lightingEnabled, bool texturesEnabled,
                      int   cameraMode);

#endif /* RENDERER_H */
