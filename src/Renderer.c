#include "Renderer.h"
#include "Planet.h"
#include "Camera.h"
#include <stdlib.h>
#include <time.h>
#include <stdio.h>
#include <string.h>
#include <math.h>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

/* ============================================================
   Lifecycle
   ============================================================ */

void Renderer_init(Renderer* r) {
    r->numStars = 0;
    srand((unsigned int)time(NULL));
    Renderer_generateStars(r, MAX_STARS);
}

void Renderer_destroy(Renderer* r) {
    (void)r;
}

/* ============================================================
   OpenGL state initialisation
   ============================================================ */

void Renderer_initOpenGL(Renderer* r) {
    (void)r;

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_NORMALIZE);
    glEnable(GL_COLOR_MATERIAL);
    glShadeModel(GL_SMOOTH);

    /* Blending & anti-aliasing */
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_LINE_SMOOTH);
    glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
    glEnable(GL_POINT_SMOOTH);
    glHint(GL_POINT_SMOOTH_HINT, GL_NICEST);

    /* Deep space background – nearly pure black */
    glClearColor(0.0f, 0.0f, 0.02f, 1.0f);
}

/* ============================================================
   Lighting
   ============================================================ */

void Renderer_setupLighting(Renderer* r, bool lightingEnabled) {
    (void)r;
    if (lightingEnabled) {
        GLfloat light_pos[]    = { 0.0f, 0.0f, 0.0f, 1.0f };
        GLfloat light_amb[]    = { 0.05f, 0.04f, 0.03f, 1.0f };
        GLfloat light_diff[]   = { 1.0f,  0.98f, 0.90f, 1.0f };  /* Warm sunlight */
        GLfloat light_spec[]   = { 1.0f,  1.0f,  1.0f,  1.0f };
        GLfloat global_amb[]   = { 0.02f, 0.02f, 0.03f, 1.0f };  /* Near-zero space ambient */

        glEnable(GL_LIGHTING);
        glEnable(GL_LIGHT0);

        glLightfv(GL_LIGHT0, GL_POSITION, light_pos);
        glLightfv(GL_LIGHT0, GL_AMBIENT,  light_amb);
        glLightfv(GL_LIGHT0, GL_DIFFUSE,  light_diff);
        glLightfv(GL_LIGHT0, GL_SPECULAR, light_spec);

        /* Very slight distance attenuation for realism */
        glLightf(GL_LIGHT0, GL_CONSTANT_ATTENUATION,  1.0f);
        glLightf(GL_LIGHT0, GL_LINEAR_ATTENUATION,    0.002f);
        glLightf(GL_LIGHT0, GL_QUADRATIC_ATTENUATION, 0.0001f);

        glLightModelfv(GL_LIGHT_MODEL_AMBIENT, global_amb);
        glLightModeli(GL_LIGHT_MODEL_LOCAL_VIEWER, GL_TRUE);  /* Better specular */

        glColorMaterial(GL_FRONT, GL_AMBIENT_AND_DIFFUSE);
    } else {
        glDisable(GL_LIGHTING);
    }
}

/* ============================================================
   Projection matrices
   ============================================================ */

void Renderer_setPerspective(Renderer* r, int width, int height) {
    (void)r;
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(50.0, (double)width / (double)height, 0.5, 600.0);
    glMatrixMode(GL_MODELVIEW);
}

void Renderer_setOrthographic(Renderer* r, int width, int height) {
    (void)r;
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(0, width, 0, height);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}

/* ============================================================
   Star generation – 3 visual classes
   ============================================================ */

void Renderer_generateStars(Renderer* r, int count) {
    int   i;
    float theta, phi, radius;
    int   cls;

    if (count > MAX_STARS) count = MAX_STARS;
    r->numStars = 0;

    for (i = 0; i < count; i++) {
        Star* s = &r->stars[r->numStars];

        theta  = (float)rand() / RAND_MAX * 2.0f * (float)M_PI;
        phi    = (float)rand() / RAND_MAX * (float)M_PI;
        radius = 290.0f + (float)rand() / RAND_MAX * 20.0f;  /* Slight depth variation */

        s->x = radius * sinf(phi) * cosf(theta);
        s->y = radius * sinf(phi) * sinf(theta);
        s->z = radius * cosf(phi);

        /* Classify star */
        int roll = rand() % 100;
        if      (roll < 60) cls = STAR_SMALL;
        else if (roll < 88) cls = STAR_MEDIUM;
        else                cls = STAR_LARGE;

        s->cls        = (StarClass)cls;
        s->brightness = 0.5f + (float)rand() / RAND_MAX * 0.5f;

        switch (cls) {
            case STAR_SMALL:
                /* White / cool white */
                s->size = 1.0f;
                s->r = s->brightness;
                s->g = s->brightness;
                s->b = s->brightness;
                break;
            case STAR_MEDIUM:
                /* Blue-white (hot stars) */
                s->size = 1.6f;
                s->r = s->brightness * 0.80f;
                s->g = s->brightness * 0.88f;
                s->b = s->brightness * 1.00f;
                break;
            case STAR_LARGE:
                /* Warm orange-yellow (giant stars) */
                s->size = 2.4f;
                s->r = s->brightness * 1.00f;
                s->g = s->brightness * 0.80f;
                s->b = s->brightness * 0.40f;
                break;
        }
        r->numStars++;
    }
}

/* ============================================================
   Star rendering – 3 passes (one per class, different pointsize)
   ============================================================ */

void Renderer_drawStarBackground(Renderer* r) {
    int       pass, i;
    float     sizes[]     = { 1.0f, 1.8f, 3.0f };
    StarClass classes[]   = { STAR_SMALL, STAR_MEDIUM, STAR_LARGE };

    glPushMatrix();
    glDisable(GL_LIGHTING);
    glDisable(GL_DEPTH_TEST);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    for (pass = 0; pass < 3; pass++) {
        glPointSize(sizes[pass]);
        glBegin(GL_POINTS);
        for (i = 0; i < r->numStars; i++) {
            Star* s = &r->stars[i];
            if (s->cls != classes[pass]) continue;
            glColor4f(s->r, s->g, s->b, s->brightness);
            glVertex3f(s->x, s->y, s->z);
        }
        glEnd();
    }

    glPointSize(1.0f);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_LIGHTING);
    glPopMatrix();
}

/* ============================================================
   Text helpers
   ============================================================ */

void Renderer_drawText(Renderer* r, float x, float y, const char* text, void* font) {
    (void)r;
    if (!font) font = GLUT_BITMAP_HELVETICA_12;
    glRasterPos2f(x, y);
    while (*text) {
        glutBitmapCharacter(font, (int)(unsigned char)*text);
        text++;
    }
}

/* ============================================================
   HUD  – redesigned top-left panel
   ============================================================ */

void Renderer_drawHUD(Renderer* r, int width, int height,
                      Planet* selectedPlanet,
                      float speedMult, bool isPaused,
                      bool orbitsEnabled, bool lightingEnabled, bool texturesEnabled,
                      int cameraMode)
{
    char   buf[256];
    float  panelW = 370.0f;
    float  panelH = 230.0f;
    float  px     = 10.0f;
    float  py     = (float)height - panelH - 10.0f;
    float  ty;

    /* Right-side planet info panel dimensions */
    float  rpW    = 230.0f;
    float  rpH    = 140.0f;
    float  rpx    = (float)width - rpW - 10.0f;
    float  rpy    = (float)height - rpH - 10.0f;

    glDisable(GL_LIGHTING);
    glDisable(GL_DEPTH_TEST);

    Renderer_setOrthographic(r, width, height);

    /* ── LEFT PANEL background ───────────────────────────── */
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glColor4f(0.0f, 0.02f, 0.08f, 0.72f);
    glBegin(GL_QUADS);
        glVertex2f(px,        py);
        glVertex2f(px+panelW, py);
        glVertex2f(px+panelW, py+panelH);
        glVertex2f(px,        py+panelH);
    glEnd();

    /* Panel border */
    glColor4f(0.3f, 0.6f, 1.0f, 0.4f);
    glLineWidth(1.0f);
    glBegin(GL_LINE_LOOP);
        glVertex2f(px,        py);
        glVertex2f(px+panelW, py);
        glVertex2f(px+panelW, py+panelH);
        glVertex2f(px,        py+panelH);
    glEnd();

    /* ── Title ───────────────────────────────────────────── */
    ty = py + panelH - 24.0f;
    glColor3f(1.0f, 0.88f, 0.2f);
    Renderer_drawText(r, px + 10.0f, ty, "SOLAR SYSTEM EXPLORER",
                      GLUT_BITMAP_HELVETICA_18);

    /* Separator */
    glColor4f(0.3f, 0.6f, 1.0f, 0.35f);
    glBegin(GL_LINES);
        glVertex2f(px + 6.0f,        ty - 6.0f);
        glVertex2f(px + panelW - 6.0f, ty - 6.0f);
    glEnd();

    /* ── Status line ─────────────────────────────────────── */
    ty -= 22.0f;
    glColor3f(0.7f, 0.9f, 1.0f);
    snprintf(buf, sizeof(buf), "Status: %s  |  Speed: %.1fx  |  Camera: %s",
             isPaused ? "PAUSED" : "RUNNING",
             speedMult,
             cameraMode == 1 ? "PLANET FOCUS" : "FREE ORBIT");
    Renderer_drawText(r, px + 10.0f, ty, buf, GLUT_BITMAP_HELVETICA_12);

    /* Toggles */
    ty -= 18.0f;
    glColor3f(0.65f, 0.85f, 0.65f);
    snprintf(buf, sizeof(buf), "Orbits: %s  |  Lighting: %s",
             orbitsEnabled  ? "ON" : "OFF",
             lightingEnabled ? "ON" : "OFF");
    Renderer_drawText(r, px + 10.0f, ty, buf, GLUT_BITMAP_HELVETICA_12);

    /* ── Controls header ─────────────────────────────────── */
    ty -= 22.0f;
    glColor3f(1.0f, 0.75f, 0.2f);
    Renderer_drawText(r, px + 10.0f, ty, "CONTROLS", GLUT_BITMAP_HELVETICA_12);

    ty -= 18.0f;
    glColor3f(0.80f, 0.80f, 0.80f);
    Renderer_drawText(r, px + 10.0f, ty,
                      "W/S: Zoom  |  A/D: Orbit  |  Q/E: Tilt  |  R: Reset",
                      GLUT_BITMAP_HELVETICA_12);

    ty -= 16.0f;
    Renderer_drawText(r, px + 10.0f, ty,
                      "Mouse Drag: Rotate  |  Scroll: Zoom",
                      GLUT_BITMAP_HELVETICA_12);

    ty -= 16.0f;
    Renderer_drawText(r, px + 10.0f, ty,
                      "1-8: Focus Planet  |  0: Solar System View",
                      GLUT_BITMAP_HELVETICA_12);

    ty -= 16.0f;
    Renderer_drawText(r, px + 10.0f, ty,
                      "SPACE/P: Pause  |  +/-: Speed  |  O: Orbits",
                      GLUT_BITMAP_HELVETICA_12);

    ty -= 16.0f;
    Renderer_drawText(r, px + 10.0f, ty,
                      "Click planet: Detail View  |  ESC: Back / Exit",
                      GLUT_BITMAP_HELVETICA_12);

    /* ── RIGHT PANEL: Selected planet info ───────────────── */
    if (selectedPlanet) {
        glColor4f(0.0f, 0.02f, 0.08f, 0.72f);
        glBegin(GL_QUADS);
            glVertex2f(rpx,       rpy);
            glVertex2f(rpx+rpW,   rpy);
            glVertex2f(rpx+rpW,   rpy+rpH);
            glVertex2f(rpx,       rpy+rpH);
        glEnd();

        glColor4f(0.3f, 0.6f, 1.0f, 0.4f);
        glBegin(GL_LINE_LOOP);
            glVertex2f(rpx,       rpy);
            glVertex2f(rpx+rpW,   rpy);
            glVertex2f(rpx+rpW,   rpy+rpH);
            glVertex2f(rpx,       rpy+rpH);
        glEnd();

        ty = rpy + rpH - 22.0f;
        glColor3f(1.0f, 0.88f, 0.2f);
        snprintf(buf, sizeof(buf), "[ %s ]", selectedPlanet->name);
        Renderer_drawText(r, rpx + 10.0f, ty, buf, GLUT_BITMAP_HELVETICA_18);

        glColor4f(0.3f, 0.6f, 1.0f, 0.35f);
        glBegin(GL_LINES);
            glVertex2f(rpx + 6.0f,     ty - 6.0f);
            glVertex2f(rpx + rpW - 6.0f, ty - 6.0f);
        glEnd();

        ty -= 20.0f;
        glColor3f(0.75f, 0.88f, 1.0f);
        snprintf(buf, sizeof(buf), "Orbit Radius : %.1f au", selectedPlanet->orbitRadius);
        Renderer_drawText(r, rpx + 10.0f, ty, buf, GLUT_BITMAP_HELVETICA_12);

        ty -= 16.0f;
        snprintf(buf, sizeof(buf), "Orbit Speed  : %.1f deg/s", selectedPlanet->orbitSpeed);
        Renderer_drawText(r, rpx + 10.0f, ty, buf, GLUT_BITMAP_HELVETICA_12);

        ty -= 16.0f;
        snprintf(buf, sizeof(buf), "Rotation     : %.1f deg/s", selectedPlanet->rotationSpeed);
        Renderer_drawText(r, rpx + 10.0f, ty, buf, GLUT_BITMAP_HELVETICA_12);

        ty -= 16.0f;
        snprintf(buf, sizeof(buf), "Axial Tilt   : %.1f deg", selectedPlanet->axialTilt);
        Renderer_drawText(r, rpx + 10.0f, ty, buf, GLUT_BITMAP_HELVETICA_12);

        ty -= 16.0f;
        snprintf(buf, sizeof(buf), "Moons        : %d", selectedPlanet->numMoons);
        Renderer_drawText(r, rpx + 10.0f, ty, buf, GLUT_BITMAP_HELVETICA_12);

        ty -= 16.0f;
        snprintf(buf, sizeof(buf), "Rings        : %s", selectedPlanet->hasRings ? "Yes" : "No");
        Renderer_drawText(r, rpx + 10.0f, ty, buf, GLUT_BITMAP_HELVETICA_12);
    }

    glDisable(GL_BLEND);
    glEnable(GL_DEPTH_TEST);
    if (lightingEnabled) glEnable(GL_LIGHTING);
}
