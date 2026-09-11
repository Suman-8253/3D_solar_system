#include "SolarSystem.h"
#include <stdlib.h>
#include <string.h>

/* ============================================================
   Lifecycle
   ============================================================ */

void SolarSystem_init(SolarSystem* sys) {
    sys->numPlanets          = 0;
    sys->selectedPlanetIndex = 3;   /* Default: Earth (index 3) */
    SolarSystem_initializePlanets(sys);
}

void SolarSystem_destroy(SolarSystem* sys) {
    int i;
    for (i = 0; i < sys->numPlanets; i++) {
        Planet_destroy(sys->planets[i]);
        free(sys->planets[i]);
    }
    sys->numPlanets = 0;
}

/* ============================================================
   Planet creation  –  name, radius, orbitRadius, orbitSpeed,
                        rotationSpeed,  r, g, b
   ============================================================ */

void SolarSystem_initializePlanets(SolarSystem* sys) {
    Planet* sun;
    Planet* mercury;
    Planet* venus;
    Planet* earth;
    Planet* moon;
    Planet* mars;
    Planet* jupiter;
    Planet* saturn;
    Planet* uranus;
    Planet* neptune;

    /* ── Sun ─────────────────────────────────────────────── */
    sun = (Planet*)malloc(sizeof(Planet));
    Planet_init(sun, "Sun", 2.5f, 0.0f, 0.0f, 1.0f,
                1.0f, 0.92f, 0.2f);
    sys->planets[sys->numPlanets++] = sun;

    /* ── Mercury ─────────────────────────────────────────── */
    mercury = (Planet*)malloc(sizeof(Planet));
    Planet_init(mercury, "Mercury", 0.30f, 5.5f, 58.0f, 0.7f,
                0.72f, 0.65f, 0.58f);
    Planet_setAxialTilt(mercury, 0.03f);
    Planet_setOrbitTilt(mercury, 7.0f);
    sys->planets[sys->numPlanets++] = mercury;

    /* ── Venus ───────────────────────────────────────────── */
    venus = (Planet*)malloc(sizeof(Planet));
    Planet_init(venus, "Venus", 0.52f, 7.8f, 42.0f, -0.4f,
                0.95f, 0.85f, 0.35f);
    Planet_setAxialTilt(venus, 177.4f);   /* Almost upside-down */
    Planet_setOrbitTilt(venus, 3.4f);
    Planet_setAtmosphere(venus, true, 1.18f,
                         0.95f, 0.82f, 0.40f, 0.30f);
    sys->planets[sys->numPlanets++] = venus;

    /* ── Earth ───────────────────────────────────────────── */
    earth = (Planet*)malloc(sizeof(Planet));
    Planet_init(earth, "Earth", 0.55f, 10.5f, 30.0f, 15.0f,
                0.20f, 0.45f, 0.90f);
    Planet_setAxialTilt(earth, 23.5f);
    Planet_setOrbitTilt(earth, 0.0f);
    Planet_setAtmosphere(earth, true, 1.14f,
                         0.45f, 0.70f, 1.00f, 0.22f);
    sys->planets[sys->numPlanets++] = earth;

    /* ── Moon ────────────────────────────────────────────── */
    moon = (Planet*)malloc(sizeof(Planet));
    Planet_init(moon, "Moon", 0.15f, 1.4f, 180.0f, 10.0f,
                0.75f, 0.75f, 0.73f);
    Planet_setAxialTilt(moon, 6.7f);
    Planet_addMoon(earth, moon);

    /* ── Mars ────────────────────────────────────────────── */
    mars = (Planet*)malloc(sizeof(Planet));
    Planet_init(mars, "Mars", 0.40f, 14.0f, 24.0f, 14.6f,
                0.82f, 0.35f, 0.15f);
    Planet_setAxialTilt(mars, 25.2f);
    Planet_setOrbitTilt(mars, 1.85f);
    sys->planets[sys->numPlanets++] = mars;

    /* ── Jupiter ─────────────────────────────────────────── */
    jupiter = (Planet*)malloc(sizeof(Planet));
    Planet_init(jupiter, "Jupiter", 1.30f, 19.0f, 13.0f, 36.0f,
                0.82f, 0.65f, 0.45f);
    Planet_setAxialTilt(jupiter, 3.1f);
    Planet_setOrbitTilt(jupiter, 1.3f);
    sys->planets[sys->numPlanets++] = jupiter;

    /* ── Saturn ──────────────────────────────────────────── */
    saturn = (Planet*)malloc(sizeof(Planet));
    Planet_init(saturn, "Saturn", 1.10f, 24.5f, 9.0f, 33.0f,
                0.92f, 0.82f, 0.60f);
    Planet_setAxialTilt(saturn, 26.7f);
    Planet_setOrbitTilt(saturn, 2.5f);
    /* Beautiful golden rings */
    Planet_setHasRings(saturn, true,
                       1.45f, 2.60f,           /* inner/outer radius */
                       0.88f, 0.80f, 0.55f, 0.60f);
    sys->planets[sys->numPlanets++] = saturn;

    /* ── Uranus ───────────────────────────────────────────── */
    uranus = (Planet*)malloc(sizeof(Planet));
    Planet_init(uranus, "Uranus", 0.80f, 30.0f, 6.0f, -21.0f,
                0.50f, 0.83f, 0.93f);
    Planet_setAxialTilt(uranus, 97.8f);   /* Almost on its side */
    Planet_setOrbitTilt(uranus, 0.8f);
    /* Subtle ice-blue rings */
    Planet_setHasRings(uranus, true,
                       1.25f, 1.70f,
                       0.55f, 0.80f, 0.95f, 0.35f);
    sys->planets[sys->numPlanets++] = uranus;

    /* ── Neptune ─────────────────────────────────────────── */
    neptune = (Planet*)malloc(sizeof(Planet));
    Planet_init(neptune, "Neptune", 0.78f, 35.5f, 4.0f, 23.0f,
                0.18f, 0.28f, 0.85f);
    Planet_setAxialTilt(neptune, 28.3f);
    Planet_setOrbitTilt(neptune, 1.8f);
    Planet_setAtmosphere(neptune, true, 1.12f,
                         0.30f, 0.45f, 1.00f, 0.18f);
    sys->planets[sys->numPlanets++] = neptune;
}

/* ============================================================
   Per-frame update
   ============================================================ */

void SolarSystem_update(SolarSystem* sys, float timeStep, float speedMultiplier) {
    int i;
    for (i = 0; i < sys->numPlanets; i++)
        Planet_update(sys->planets[i], timeStep, speedMultiplier);
}

/* ============================================================
   Draw
   ============================================================ */

void SolarSystem_draw(SolarSystem* sys, bool showOrbits, bool useTextures) {
    int  i;
    bool isSelected;
    for (i = 0; i < sys->numPlanets; i++) {
        isSelected = (i == sys->selectedPlanetIndex);
        if (showOrbits && i > 0)
            Planet_drawOrbit(sys->planets[i]);
        Planet_draw(sys->planets[i], showOrbits, useTextures, isSelected);
    }
}

void SolarSystem_drawOrbits(SolarSystem* sys) {
    int i;
    for (i = 1; i < sys->numPlanets; i++)
        Planet_drawOrbit(sys->planets[i]);
}

/* ============================================================
   Accessors
   ============================================================ */

Planet* SolarSystem_getPlanet(SolarSystem* sys, int index) {
    if (index >= 0 && index < sys->numPlanets)
        return sys->planets[index];
    return NULL;
}

int SolarSystem_getNumPlanets(SolarSystem* sys) { return sys->numPlanets; }

void SolarSystem_setSelectedPlanetIndex(SolarSystem* sys, int index) {
    if (index >= 0 && index < sys->numPlanets)
        sys->selectedPlanetIndex = index;
}

int SolarSystem_getSelectedPlanetIndex(SolarSystem* sys) {
    return sys->selectedPlanetIndex;
}

Planet* SolarSystem_getSelectedPlanet(SolarSystem* sys) {
    return SolarSystem_getPlanet(sys, sys->selectedPlanetIndex);
}

void SolarSystem_getPlanetWorldPos(SolarSystem* sys, int index,
                                   float* x, float* y, float* z) {
    Planet* p = SolarSystem_getPlanet(sys, index);
    if (p) {
        *x = p->posX;
        *y = p->posY;
        *z = p->posZ;
    } else {
        *x = *y = *z = 0.0f;
    }
}
