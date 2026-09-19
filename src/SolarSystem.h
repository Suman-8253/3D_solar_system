#ifndef SOLARSYSTEM_H
#define SOLARSYSTEM_H

/*  ═══════════════════════════════════════════════════════════════
    SolarSystem  –  Manager for all celestial bodies
    ═══════════════════════════════════════════════════════════════ */

#include "Planet.h"
#include "AsteroidBelt.h"
#include <vector>
#include <string>

class SolarSystem {
public:
    std::vector<Planet*> planets;
    AsteroidBelt* asteroidBelt;
    
    int selectedPlanetIndex;

    SolarSystem();
    ~SolarSystem();

    void update(float dt, float speedMult);
    void draw(bool showOrbits, bool useTextures, bool wireframe);
    
    Planet* getSelectedPlanet() const;
    void selectPlanet(int index);
    
    bool loadData(const std::string& path);

private:
    void initPlanets();
    void loadTextures();
};

#endif /* SOLARSYSTEM_H */
