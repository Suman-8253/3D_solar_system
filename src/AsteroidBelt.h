#ifndef ASTEROIDBELT_H
#define ASTEROIDBELT_H

/*  ═══════════════════════════════════════════════════════════════
    AsteroidBelt  –  generates and renders random asteroids
    CG Concepts: Instancing, procedurally generated geometry,
                 translation, rotation
    ═══════════════════════════════════════════════════════════════ */

#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif
#include <vector>

struct Asteroid {
    float distance;
    float angle;
    float speed;
    float size;
    float rotX, rotY, rotZ;
    float yOffset;
};

class AsteroidBelt {
public:
    AsteroidBelt(int count, float minRadius, float maxRadius);
    ~AsteroidBelt();

    void update(float dt, float speedMult);
    void draw(bool wireframe);

private:
    std::vector<Asteroid> asteroids_;
    float baseColor_[3];
};

#endif /* ASTEROIDBELT_H */
