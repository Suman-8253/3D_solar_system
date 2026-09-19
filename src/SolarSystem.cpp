#include "SolarSystem.h"
#include "TextureManager.h"
#include <fstream>
#include <iostream>
#include <sstream>

/* Trim string */
static void trim(std::string& s) {
    s.erase(0, s.find_first_not_of(" \t\r\n"));
    s.erase(s.find_last_not_of(" \t\r\n") + 1);
}

SolarSystem::SolarSystem() : asteroidBelt(nullptr), selectedPlanetIndex(3) { // Default Earth
    initPlanets();
    loadTextures();
    loadData("data/planets.dat");
}

SolarSystem::~SolarSystem() {
    for (auto* p : planets) delete p;
    planets.clear();
    delete asteroidBelt;
}

void SolarSystem::initPlanets() {
    Planet* sun = new Planet("Sun", 2.5f, 0.0f, 0.0f, 1.0f, 1.0f, 0.92f, 0.2f);
    planets.push_back(sun);

    Planet* mercury = new Planet("Mercury", 0.30f, 5.5f, 58.0f, 0.7f, 0.72f, 0.65f, 0.58f);
    mercury->axialTilt = 0.03f; mercury->orbitTilt = 7.0f;
    planets.push_back(mercury);

    Planet* venus = new Planet("Venus", 0.52f, 7.8f, 42.0f, -0.4f, 0.95f, 0.85f, 0.35f);
    venus->axialTilt = 177.4f; venus->orbitTilt = 3.4f;
    venus->setAtmosphere(true, 1.18f, 0.95f, 0.82f, 0.40f, 0.30f);
    planets.push_back(venus);

    Planet* earth = new Planet("Earth", 0.55f, 10.5f, 30.0f, 15.0f, 0.20f, 0.45f, 0.90f);
    earth->axialTilt = 23.5f; earth->orbitTilt = 0.0f;
    earth->setAtmosphere(true, 1.14f, 0.45f, 0.70f, 1.00f, 0.22f);
    planets.push_back(earth);

    Planet* moon = new Planet("Moon", 0.15f, 1.4f, 180.0f, 10.0f, 0.75f, 0.75f, 0.73f);
    moon->axialTilt = 6.7f;
    earth->addMoon(moon);

    Planet* mars = new Planet("Mars", 0.40f, 14.0f, 24.0f, 14.6f, 0.82f, 0.35f, 0.15f);
    mars->axialTilt = 25.2f; mars->orbitTilt = 1.85f;
    planets.push_back(mars);

    Planet* jupiter = new Planet("Jupiter", 1.30f, 19.0f, 13.0f, 36.0f, 0.82f, 0.65f, 0.45f);
    jupiter->axialTilt = 3.1f; jupiter->orbitTilt = 1.3f;
    planets.push_back(jupiter);

    Planet* saturn = new Planet("Saturn", 1.10f, 24.5f, 9.0f, 33.0f, 0.92f, 0.82f, 0.60f);
    saturn->axialTilt = 26.7f; saturn->orbitTilt = 2.5f;
    saturn->setRings(true, 1.45f, 2.60f, 0.88f, 0.80f, 0.55f, 0.60f);
    planets.push_back(saturn);

    Planet* uranus = new Planet("Uranus", 0.80f, 30.0f, 6.0f, -21.0f, 0.50f, 0.83f, 0.93f);
    uranus->axialTilt = 97.8f; uranus->orbitTilt = 0.8f;
    uranus->setRings(true, 1.25f, 1.70f, 0.55f, 0.80f, 0.95f, 0.35f);
    planets.push_back(uranus);

    Planet* neptune = new Planet("Neptune", 0.78f, 35.5f, 4.0f, 23.0f, 0.18f, 0.28f, 0.85f);
    neptune->axialTilt = 28.3f; neptune->orbitTilt = 1.8f;
    neptune->setAtmosphere(true, 1.12f, 0.30f, 0.45f, 1.00f, 0.18f);
    planets.push_back(neptune);

    Planet* pluto = new Planet("Pluto", 0.18f, 40.0f, 3.0f, -6.0f, 0.85f, 0.80f, 0.75f); // Greyish ice
    pluto->axialTilt = 122.5f; pluto->orbitTilt = 17.1f;
    planets.push_back(pluto);

    asteroidBelt = new AsteroidBelt(300, 15.0f, 18.0f); // Between Mars (14) and Jupiter (19)
}

void SolarSystem::loadTextures() {
    TextureManager& tm = TextureManager::instance();
    GLuint sunTex = tm.load("textures/sun.jpg");
    GLuint mercuryTex = tm.load("textures/mercury.jpg");
    GLuint venusTex = tm.load("textures/venus.jpg");
    GLuint earthTex = tm.load("textures/earth.jpg");
    GLuint earthCloudTex = tm.load("textures/earth_clouds.png");
    GLuint moonTex = tm.load("textures/moon.jpg");
    GLuint marsTex = tm.load("textures/mars.jpg");
    GLuint jupiterTex = tm.load("textures/jupiter.jpg");
    GLuint saturnTex = tm.load("textures/saturn.jpg");
    GLuint saturnRingTex = tm.load("textures/saturn_ring.png");
    GLuint uranusTex = tm.load("textures/uranus.jpg");
    GLuint uranusRingTex = tm.load("textures/uranus_ring.png");
    GLuint neptuneTex = tm.load("textures/neptune.jpg");
    GLuint plutoTex = tm.load("textures/pluto.jpg"); // Might fail, will fallback

    for (auto* p : planets) {
        if (p->name == "Sun") p->setTextures(sunTex, 0, 0);
        else if (p->name == "Mercury") p->setTextures(mercuryTex, 0, 0);
        else if (p->name == "Venus") p->setTextures(venusTex, 0, 0);
        else if (p->name == "Earth") {
            p->setTextures(earthTex, earthCloudTex, 0);
            if (!p->moons.empty()) p->moons[0]->setTextures(moonTex, 0, 0);
        }
        else if (p->name == "Mars") p->setTextures(marsTex, 0, 0);
        else if (p->name == "Jupiter") p->setTextures(jupiterTex, 0, 0);
        else if (p->name == "Saturn") p->setTextures(saturnTex, 0, saturnRingTex);
        else if (p->name == "Uranus") p->setTextures(uranusTex, 0, uranusRingTex);
        else if (p->name == "Neptune") p->setTextures(neptuneTex, 0, 0);
        else if (p->name == "Pluto") p->setTextures(plutoTex, 0, 0);
    }
}

bool SolarSystem::loadData(const std::string& path) {
    std::ifstream file(path);
    if (!file.is_open()) return false;

    std::string line;
    Planet* current = nullptr;

    while (std::getline(file, line)) {
        trim(line);
        if (line.empty() || line[0] == '#') continue;

        if (line[0] == '[' && line.back() == ']') {
            std::string name = line.substr(1, line.length() - 2);
            current = nullptr;
            for (auto* p : planets) {
                if (p->name == name) {
                    current = p;
                    break;
                }
            }
            continue;
        }

        if (current) {
            size_t eq = line.find('=');
            if (eq != std::string::npos) {
                std::string key = line.substr(0, eq);
                std::string val = line.substr(eq + 1);
                trim(key); trim(val);

                if (key == "Type") current->type = val;
                else if (key == "Diameter") current->diameter = val;
                else if (key == "DistanceSun") current->distFromSun = val;
                else if (key == "DayLength") current->dayLength = val;
                else if (key == "YearLength") current->yearLength = val;
                else if (key == "AvgTemp") current->avgTemp = val;
                else if (key == "Gravity") current->gravityStr = val;
                else if (key == "Moons") current->numMoonsStr = val;
                else if (key == "Atmosphere") current->atmosphereStr = val;
                else if (key == "Fact") current->funFact = val;
                else if (key == "Description") current->description = val;
            }
        }
    }
    return true;
}

void SolarSystem::update(float dt, float speedMult) {
    for (auto* p : planets) p->update(dt, speedMult);
    if (asteroidBelt) asteroidBelt->update(dt, speedMult);
}

void SolarSystem::draw(bool showOrbits, bool useTextures, bool wireframe) {
    for (size_t i = 0; i < planets.size(); i++) {
        bool isSelected = (int)i == selectedPlanetIndex;
        if (showOrbits && i > 0) planets[i]->drawOrbit();
        planets[i]->draw(showOrbits, useTextures, isSelected, wireframe);
    }
    if (asteroidBelt) asteroidBelt->draw(wireframe);
}

Planet* SolarSystem::getSelectedPlanet() const {
    if (selectedPlanetIndex >= 0 && selectedPlanetIndex < (int)planets.size())
        return planets[selectedPlanetIndex];
    return nullptr;
}

void SolarSystem::selectPlanet(int index) {
    if (index >= 0 && index < (int)planets.size())
        selectedPlanetIndex = index;
}
