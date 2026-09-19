#include "AsteroidBelt.h"
#include <cstdlib>
#include <cmath>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

AsteroidBelt::AsteroidBelt(int count, float minRadius, float maxRadius) {
    baseColor_[0] = 0.45f;
    baseColor_[1] = 0.42f;
    baseColor_[2] = 0.40f;

    for (int i = 0; i < count; i++) {
        Asteroid a;
        a.distance = minRadius + (float)rand() / RAND_MAX * (maxRadius - minRadius);
        a.angle    = (float)rand() / RAND_MAX * 360.0f;
        a.speed    = 15.0f + (float)rand() / RAND_MAX * 10.0f; // degrees/sec
        a.size     = 0.02f + (float)rand() / RAND_MAX * 0.06f;
        a.yOffset  = -0.3f + (float)rand() / RAND_MAX * 0.6f;
        
        a.rotX = (float)rand() / RAND_MAX * 360.0f;
        a.rotY = (float)rand() / RAND_MAX * 360.0f;
        a.rotZ = (float)rand() / RAND_MAX * 360.0f;

        asteroids_.push_back(a);
    }
}

AsteroidBelt::~AsteroidBelt() {}

void AsteroidBelt::update(float dt, float speedMult) {
    for (auto& a : asteroids_) {
        a.angle += a.speed * dt * speedMult;
        if (a.angle > 360.0f) a.angle -= 360.0f;
        
        a.rotX += 20.0f * dt * speedMult;
        a.rotY += 15.0f * dt * speedMult;
    }
}

void AsteroidBelt::draw(bool wireframe) {
    GLfloat mat_amb[]  = { baseColor_[0]*0.2f, baseColor_[1]*0.2f, baseColor_[2]*0.2f, 1.0f };
    GLfloat mat_diff[] = { baseColor_[0], baseColor_[1], baseColor_[2], 1.0f };
    GLfloat mat_spec[] = { 0.0f, 0.0f, 0.0f, 1.0f };
    
    glMaterialfv(GL_FRONT, GL_AMBIENT,  mat_amb);
    glMaterialfv(GL_FRONT, GL_DIFFUSE,  mat_diff);
    glMaterialfv(GL_FRONT, GL_SPECULAR, mat_spec);
    glColor3fv(baseColor_);

    if (wireframe) glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

    for (const auto& a : asteroids_) {
        glPushMatrix();
        
        glRotatef(a.angle, 0, 1, 0);
        glTranslatef(a.distance, a.yOffset, 0);
        
        glRotatef(a.rotX, 1, 0, 0);
        glRotatef(a.rotY, 0, 1, 0);
        glRotatef(a.rotZ, 0, 0, 1);
        
        glutSolidSphere(a.size, 6, 6); // Low poly for asteroids
        
        glPopMatrix();
    }

    if (wireframe) glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
}
