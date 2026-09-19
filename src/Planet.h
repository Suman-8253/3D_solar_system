#ifndef PLANET_H
#define PLANET_H

/*  ═══════════════════════════════════════════════════════════════
    Planet  –  base class for all celestial bodies
    CG Concepts: Hierarchical transforms, materials, texture mapping,
                 translation, rotation, scaling
    ═══════════════════════════════════════════════════════════════ */

#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

#include <string>
#include <vector>

class Planet {
public:
    /* ── Identification ── */
    std::string name;

    /* ── Geometry ── */
    float radius;
    float orbitRadius;
    float orbitSpeed;       /* degrees / second */
    float rotationSpeed;    /* degrees / second */
    float axialTilt;        /* degrees */
    float orbitTilt;        /* degrees */
    float color[3];

    /* ── Atmosphere ── */
    bool  hasAtmosphere;
    float atmosphereColor[4];
    float atmosphereScale;

    /* ── Rings ── */
    bool  hasRings;
    float ringInnerRadius, ringOuterRadius;
    float ringColor[4];

    /* ── Animation state ── */
    float currentOrbitAngle;
    float currentRotationAngle;

    /* ── World position (set during draw) ── */
    float posX, posY, posZ;

    /* ── Textures ── */
    GLuint textureID;
    GLuint cloudTextureID;
    GLuint ringTextureID;

    /* ── Moons ── */
    std::vector<Planet*> moons;

    /* ── Planet information (for HUD display) ── */
    std::string type;
    std::string diameter;
    std::string distFromSun;
    std::string dayLength;
    std::string yearLength;
    std::string avgTemp;
    std::string gravityStr;
    std::string numMoonsStr;
    std::string atmosphereStr;
    std::string description;
    std::string funFact;

    /* ── Constructor / Destructor ── */
    Planet(const std::string& name, float radius, float orbitRadius,
           float orbitSpeed, float rotationSpeed,
           float r, float g, float b);
    virtual ~Planet();

    /* ── Per-frame ── */
    virtual void update(float dt, float speedMult);
    virtual void draw(bool showOrbit, bool useTextures,
                      bool isSelected, bool wireframe);
    void drawOrbit();

    /* ── Setup helpers ── */
    void addMoon(Planet* moon);
    void setTextures(GLuint tex, GLuint cloudTex, GLuint ringTex);
    void setAtmosphere(bool has, float scale,
                       float r, float g, float b, float a);
    void setRings(bool has, float inner, float outer,
                  float r, float g, float b, float a);
    void setInfo(const std::string& type,
                 const std::string& diameter,
                 const std::string& dist,
                 const std::string& day,
                 const std::string& year,
                 const std::string& temp,
                 const std::string& grav,
                 const std::string& moonCount,
                 const std::string& atm,
                 const std::string& desc,
                 const std::string& fact);

protected:
    GLUquadric* quadric_;

    void drawBody(bool useTextures, bool wireframe);
    void drawRings();
    void drawAtmosphere();
    void drawSunGlow();
};

#endif /* PLANET_H */
