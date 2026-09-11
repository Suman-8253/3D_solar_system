#ifndef SOLARSYSTEM_H
#define SOLARSYSTEM_H

#include "Planet.h"
#include <stdbool.h>

#define MAX_PLANETS 20

typedef struct {
    Planet* planets[MAX_PLANETS];
    int     numPlanets;
    int     selectedPlanetIndex;
} SolarSystem;

void    SolarSystem_init(SolarSystem* sys);
void    SolarSystem_destroy(SolarSystem* sys);
void    SolarSystem_initializePlanets(SolarSystem* sys);
void    SolarSystem_update(SolarSystem* sys, float timeStep, float speedMultiplier);
void    SolarSystem_draw(SolarSystem* sys, bool showOrbits, bool useTextures);
void    SolarSystem_drawOrbits(SolarSystem* sys);

Planet* SolarSystem_getPlanet(SolarSystem* sys, int index);
int     SolarSystem_getNumPlanets(SolarSystem* sys);

void    SolarSystem_setSelectedPlanetIndex(SolarSystem* sys, int index);
int     SolarSystem_getSelectedPlanetIndex(SolarSystem* sys);
Planet* SolarSystem_getSelectedPlanet(SolarSystem* sys);

/* Returns the current world-space position of planet [index] */
void    SolarSystem_getPlanetWorldPos(SolarSystem* sys, int index,
                                      float* x, float* y, float* z);

#endif /* SOLARSYSTEM_H */
