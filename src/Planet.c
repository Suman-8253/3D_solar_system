#include "Planet.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

/* ============================================================
   Initialisation helpers
   ============================================================ */

void Planet_init(Planet* p, const char* name,
                 float radius, float orbitRadius,
                 float orbitSpeed, float rotationSpeed,
                 float r, float g, float b)
{
    strncpy(p->name, name, sizeof(p->name) - 1);
    p->name[sizeof(p->name) - 1] = '\0';

    p->radius        = radius;
    p->orbitRadius   = orbitRadius;
    p->orbitSpeed    = orbitSpeed;
    p->rotationSpeed = rotationSpeed;
    p->axialTilt     = 0.0f;
    p->orbitTilt     = 0.0f;

    p->color[0] = r;
    p->color[1] = g;
    p->color[2] = b;

    p->hasAtmosphere      = false;
    p->atmosphereColor[0] = 0.4f;
    p->atmosphereColor[1] = 0.7f;
    p->atmosphereColor[2] = 1.0f;
    p->atmosphereColor[3] = 0.25f;
    p->atmosphereScale    = 1.15f;

    p->ringInnerRadius = 0.0f;
    p->ringOuterRadius = 0.0f;
    p->ringColor[0] = 0.8f;
    p->ringColor[1] = 0.7f;
    p->ringColor[2] = 0.5f;
    p->ringColor[3] = 0.55f;
    p->hasRings = false;

    p->currentOrbitAngle    = 0.0f;
    p->currentRotationAngle = 0.0f;

    p->posX = 0.0f;
    p->posY = 0.0f;
    p->posZ = 0.0f;

    p->numMoons = 0;
}

void Planet_setHasRings(Planet* p, bool hasRings,
                        float innerR, float outerR,
                        float cr, float cg, float cb, float ca)
{
    p->hasRings        = hasRings;
    p->ringInnerRadius = innerR;
    p->ringOuterRadius = outerR;
    p->ringColor[0] = cr;
    p->ringColor[1] = cg;
    p->ringColor[2] = cb;
    p->ringColor[3] = ca;
}

void Planet_setAtmosphere(Planet* p, bool hasAtm,
                          float scale,
                          float ar, float ag, float ab, float aa)
{
    p->hasAtmosphere      = hasAtm;
    p->atmosphereScale    = scale;
    p->atmosphereColor[0] = ar;
    p->atmosphereColor[1] = ag;
    p->atmosphereColor[2] = ab;
    p->atmosphereColor[3] = aa;
}

void Planet_setAxialTilt(Planet* p, float tilt) { p->axialTilt  = tilt; }
void Planet_setOrbitTilt(Planet* p, float tilt) { p->orbitTilt  = tilt; }

void Planet_addMoon(Planet* p, Planet* moon) {
    if (p->numMoons < MAX_MOONS)
        p->moons[p->numMoons++] = moon;
}

void Planet_destroy(Planet* p) {
    int i;
    for (i = 0; i < p->numMoons; i++) {
        Planet_destroy(p->moons[i]);
        free(p->moons[i]);
    }
    p->numMoons = 0;
}

/* ============================================================
   Per-frame update
   ============================================================ */

void Planet_update(Planet* p, float timeStep, float speedMultiplier) {
    int i;
    p->currentOrbitAngle    += p->orbitSpeed    * timeStep * speedMultiplier;
    p->currentRotationAngle += p->rotationSpeed * timeStep * speedMultiplier;
    if (p->currentOrbitAngle    >  360.0f) p->currentOrbitAngle    -= 360.0f;
    if (p->currentOrbitAngle    < -360.0f) p->currentOrbitAngle    += 360.0f;
    if (p->currentRotationAngle >  360.0f) p->currentRotationAngle -= 360.0f;
    if (p->currentRotationAngle < -360.0f) p->currentRotationAngle += 360.0f;

    for (i = 0; i < p->numMoons; i++)
        Planet_update(p->moons[i], timeStep, speedMultiplier);
}

/* ============================================================
   Ring rendering – proper disc annulus (GL_TRIANGLE_STRIP)
   ============================================================ */

void Planet_drawRings(Planet* p) {
    int   i;
    int   segments = 80;
    float inner    = p->ringInnerRadius;
    float outer    = p->ringOuterRadius;
    float tiltDeg  = 20.0f;   /* Ring tilt relative to planet equator */

    if (inner <= 0.0f || outer <= 0.0f) return;

    glPushMatrix();
    glRotatef(tiltDeg, 1.0f, 0.0f, 0.2f);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glDisable(GL_LIGHTING);
    glDisable(GL_CULL_FACE);

    /* Draw disc as triangle strip (two concentric circles) */
    glBegin(GL_TRIANGLE_STRIP);
    for (i = 0; i <= segments; i++) {
        float theta = 2.0f * (float)M_PI * (float)i / (float)segments;
        float cosT  = cosf(theta);
        float sinT  = sinf(theta);

        /* Vary brightness around ring for visual interest */
        float bright = 0.75f + 0.25f * cosf(theta * 3.0f);
        glColor4f(p->ringColor[0] * bright,
                  p->ringColor[1] * bright,
                  p->ringColor[2] * bright,
                  p->ringColor[3]);

        glVertex3f(outer * cosT, 0.0f, outer * sinT);
        glVertex3f(inner * cosT, 0.0f, inner * sinT);
    }
    glEnd();

    /* Second, slightly transparent pass for inner/outer edge glow */
    glBegin(GL_LINE_LOOP);
    glColor4f(1.0f, 0.95f, 0.7f, 0.3f);
    for (i = 0; i < segments; i++) {
        float theta = 2.0f * (float)M_PI * (float)i / (float)segments;
        glVertex3f(outer * cosf(theta), 0.0f, outer * sinf(theta));
    }
    glEnd();

    glEnable(GL_CULL_FACE);
    glEnable(GL_LIGHTING);
    glDisable(GL_BLEND);

    glPopMatrix();
}

/* ============================================================
   Atmosphere glow rendering
   ============================================================ */

void Planet_drawAtmosphere(Planet* p) {
    int   pass;
    float scales[] = { p->atmosphereScale, p->atmosphereScale * 1.06f };
    float alphas[] = { p->atmosphereColor[3], p->atmosphereColor[3] * 0.35f };

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glDisable(GL_LIGHTING);
    glCullFace(GL_FRONT);   /* Draw back-facing hemisphere for glow rim */
    glEnable(GL_CULL_FACE);

    for (pass = 0; pass < 2; pass++) {
        glColor4f(p->atmosphereColor[0],
                  p->atmosphereColor[1],
                  p->atmosphereColor[2],
                  alphas[pass]);
        glutSolidSphere(p->radius * scales[pass], 28, 28);
    }

    glCullFace(GL_BACK);
    glEnable(GL_LIGHTING);
    glDisable(GL_BLEND);
}

/* ============================================================
   Sun multi-pass glow
   ============================================================ */

static void drawSunGlowPasses(Planet* p) {
    /* Multiple transparent spheres expanding outward to simulate corona */
    float scales[] = { 1.15f, 1.35f, 1.65f, 2.10f, 2.80f };
    float alphas[] = { 0.20f, 0.12f, 0.07f, 0.04f, 0.02f };
    int   nPasses  = 5;
    int   i;

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE);   /* Additive blending for glow */
    glDisable(GL_LIGHTING);
    glDisable(GL_CULL_FACE);

    for (i = 0; i < nPasses; i++) {
        float s = scales[i];
        float a = alphas[i];
        /* Warm orange-yellow glow fading outward */
        glColor4f(1.0f, 0.6f - i * 0.05f, 0.0f + i * 0.01f, a);
        glutSolidSphere(p->radius * s, 24, 24);
    }

    glEnable(GL_CULL_FACE);
    glEnable(GL_LIGHTING);
    glDisable(GL_BLEND);
}

/* ============================================================
   Orbit path drawing
   ============================================================ */

void Planet_drawOrbit(Planet* p) {
    int   i;
    int   segs = 120;
    float cosT, sinZ;

    if (p->orbitRadius <= 0.0f) return;

    glPushMatrix();
    if (p->orbitTilt != 0.0f)
        glRotatef(p->orbitTilt, 0.0f, 0.0f, 1.0f);

    glDisable(GL_LIGHTING);

    /* Outer faint glow line */
    glLineWidth(2.0f);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glColor4f(0.3f, 0.4f, 0.8f, 0.12f);
    glBegin(GL_LINE_LOOP);
    for (i = 0; i < segs; i++) {
        float theta = 2.0f * (float)M_PI * (float)i / (float)segs;
        glVertex3f(p->orbitRadius * cosf(theta), 0.0f, p->orbitRadius * sinf(theta));
    }
    glEnd();

    /* Inner crisp orbit line */
    glLineWidth(1.0f);
    glColor4f(0.35f, 0.45f, 0.75f, 0.45f);
    glBegin(GL_LINE_LOOP);
    for (i = 0; i < segs; i++) {
        float theta = 2.0f * (float)M_PI * (float)i / (float)segs;
        glVertex3f(p->orbitRadius * cosf(theta), 0.0f, p->orbitRadius * sinf(theta));
    }
    glEnd();

    glDisable(GL_BLEND);
    glLineWidth(1.0f);
    glEnable(GL_LIGHTING);

    glPopMatrix();
    (void)cosT; (void)sinZ;
}

/* ============================================================
   Main draw function
   ============================================================ */

void Planet_draw(Planet* p, bool showOrbit, bool useTextures, bool isSelected) {
    int     i;
    GLfloat mat_ambient[4], mat_diffuse[4], mat_specular[4], mat_emission[4], mat_shininess;
    int     isSun = (strcmp(p->name, "Sun") == 0);

    /* -- Begin hierarchical transformation -- */
    glPushMatrix();

    /* 1. Orbit inclination */
    if (p->orbitTilt != 0.0f)
        glRotatef(p->orbitTilt, 0.0f, 0.0f, 1.0f);

    /* 2. Revolution around parent (orbit) */
    if (p->orbitRadius > 0.0f) {
        glRotatef(p->currentOrbitAngle, 0.0f, 1.0f, 0.0f);
        glTranslatef(p->orbitRadius, 0.0f, 0.0f);
    }

    /* Store world position by reading the modelview matrix */
    {
        GLfloat mv[16];
        glGetFloatv(GL_MODELVIEW_MATRIX, mv);
        /* World position is the translation part of the inverse of the view matrix.
           For simple extraction from accumulated model matrix: */
        p->posX = mv[12];
        p->posY = mv[13];
        p->posZ = mv[14];
    }

    /* 3. Set material properties */
    mat_ambient[0]  = p->color[0] * 0.15f; mat_ambient[1]  = p->color[1] * 0.15f;
    mat_ambient[2]  = p->color[2] * 0.15f; mat_ambient[3]  = 1.0f;

    mat_diffuse[0]  = p->color[0]; mat_diffuse[1]  = p->color[1];
    mat_diffuse[2]  = p->color[2]; mat_diffuse[3]  = 1.0f;

    mat_specular[0] = 0.3f;  mat_specular[1] = 0.3f;
    mat_specular[2] = 0.3f;  mat_specular[3] = 1.0f;

    mat_emission[0] = 0.0f;  mat_emission[1] = 0.0f;
    mat_emission[2] = 0.0f;  mat_emission[3] = 1.0f;
    mat_shininess   = 15.0f;

    if (isSun) {
        /* Sun is self-luminous */
        mat_emission[0] = p->color[0] * 0.9f;
        mat_emission[1] = p->color[1] * 0.7f;
        mat_emission[2] = p->color[2] * 0.0f;
        mat_ambient[0]  = p->color[0];
        mat_ambient[1]  = p->color[1];
        mat_ambient[2]  = p->color[2];
        mat_shininess   = 0.0f;
    } else if (isSelected) {
        mat_emission[0] = 0.25f;
        mat_emission[1] = 0.25f;
        mat_emission[2] = 0.35f;
        mat_shininess   = 40.0f;
    }

    glMaterialfv(GL_FRONT, GL_AMBIENT,  mat_ambient);
    glMaterialfv(GL_FRONT, GL_DIFFUSE,  mat_diffuse);
    glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
    glMaterialfv(GL_FRONT, GL_EMISSION, mat_emission);
    glMaterialf (GL_FRONT, GL_SHININESS, mat_shininess);
    glColor3f(p->color[0], p->color[1], p->color[2]);

    /* 4. Save matrix for moons (before axial tilt and self-rotation) */
    glPushMatrix();

    /* 5. Axial tilt */
    if (p->axialTilt != 0.0f)
        glRotatef(p->axialTilt, 0.0f, 0.0f, 1.0f);

    /* 6. Self-rotation */
    glRotatef(p->currentRotationAngle, 0.0f, 1.0f, 0.0f);

    /* 7. Draw the sphere */
    glutSolidSphere(p->radius, 40, 40);

    /* 8. Sun glow passes */
    if (isSun) {
        drawSunGlowPasses(p);
    }

    glPopMatrix();  /* Back to orbit position (before tilt/rotation) */

    /* 9. Rings (drawn in orbit space, tilted by ringColor settings) */
    if (p->hasRings && p->ringInnerRadius > 0.0f) {
        Planet_drawRings(p);
    }

    /* 10. Atmosphere (drawn in orbit space, slightly larger) */
    if (p->hasAtmosphere) {
        Planet_drawAtmosphere(p);
    }

    /* 11. Moons (hierarchical – still inside orbit rotation) */
    for (i = 0; i < p->numMoons; i++) {
        if (showOrbit)
            Planet_drawOrbit(p->moons[i]);
        Planet_draw(p->moons[i], showOrbit, useTextures, false);
    }

    glPopMatrix();   /* Restore parent space */
}
