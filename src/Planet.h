#ifndef PLANET_H
#define PLANET_H

#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif
#include <stdbool.h>

#define MAX_MOONS 10

typedef struct Planet {
    char  name[32];

    /* Size & orbit */
    float radius;
    float orbitRadius;
    float orbitSpeed;
    float rotationSpeed;

    /* Tilt */
    float axialTilt;    /* Degrees – planet pole tilt from vertical  */
    float orbitTilt;    /* Degrees – orbit plane inclination          */

    /* Base color */
    float color[3];

    /* Atmosphere / glow */
    bool  hasAtmosphere;
    float atmosphereColor[4];   /* RGBA */
    float atmosphereScale;      /* Multiplier over planet radius        */

    /* Ring geometry */
    float ringInnerRadius;      /* 0 = no rings */
    float ringOuterRadius;
    float ringColor[4];         /* RGBA */

    /* Animation state */
    float currentOrbitAngle;
    float currentRotationAngle;

    /* Current world position (set during Planet_draw) */
    float posX, posY, posZ;

    /* Legacy flag kept for compatibility */
    bool hasRings;

    /* Moons (hierarchical) */
    struct Planet* moons[MAX_MOONS];
    int            numMoons;
} Planet;

/* Core lifecycle */
void Planet_init(Planet* p, const char* name,
                 float radius, float orbitRadius,
                 float orbitSpeed, float rotationSpeed,
                 float r, float g, float b);
void Planet_destroy(Planet* p);

/* Per-frame */
void Planet_update(Planet* p, float timeStep, float speedMultiplier);
void Planet_draw(Planet* p, bool showOrbit, bool useTextures, bool isSelected);
void Planet_drawOrbit(Planet* p);

/* Helpers */
void Planet_addMoon(Planet* p, Planet* moon);
void Planet_setHasRings(Planet* p, bool hasRings,
                        float innerR, float outerR,
                        float cr, float cg, float cb, float ca);
void Planet_setAtmosphere(Planet* p, bool hasAtm,
                          float scale,
                          float ar, float ag, float ab, float aa);
void Planet_setAxialTilt(Planet* p, float tilt);
void Planet_setOrbitTilt(Planet* p, float tilt);

/* Draw sub-steps (called from Planet_draw, exposed for Renderer sun-glow) */
void Planet_drawRings(Planet* p);
void Planet_drawAtmosphere(Planet* p);

#endif /* PLANET_H */
