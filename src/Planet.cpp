/*  ═══════════════════════════════════════════════════════════════
    Planet.cpp  –  Celestial body rendering and animation

    CG Concepts demonstrated:
      • Hierarchical transformation (glPushMatrix / glPopMatrix)
      • Translation: orbit (glTranslatef)
      • Rotation: orbit revolution, axial tilt, self-rotation (glRotatef)
      • Scaling: planet sizes (glScalef via gluSphere radius)
      • Texture mapping (gluQuadricTexture → gluSphere)
      • Material properties (GL_AMBIENT, GL_DIFFUSE, GL_SPECULAR, GL_EMISSION)
      • Blending (atmosphere, rings, glow: GL_SRC_ALPHA, GL_ONE)
      • Depth masking (glDepthMask) for transparent effects
      • Back-face culling (glCullFace) for atmosphere glow
      • Ring geometry (GL_TRIANGLE_STRIP annulus)
      • Frame-rate independent animation (delta time)
    ═══════════════════════════════════════════════════════════════ */

#include "Planet.h"
#include <cmath>
#include <cstdio>
#include <cstring>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

/* ════════════════════════════════════════════════════════════════
   Construction / Destruction
   ════════════════════════════════════════════════════════════════ */

Planet::Planet(const std::string& n, float r, float orb,
               float orbSpd, float rotSpd,
               float cr, float cg, float cb)
    : name(n), radius(r), orbitRadius(orb),
      orbitSpeed(orbSpd), rotationSpeed(rotSpd),
      axialTilt(0.0f), orbitTilt(0.0f),
      hasAtmosphere(false), atmosphereScale(1.15f),
      hasRings(false), ringInnerRadius(0), ringOuterRadius(0),
      currentOrbitAngle(0), currentRotationAngle(0),
      posX(0), posY(0), posZ(0),
      textureID(0), cloudTextureID(0), ringTextureID(0)
{
    color[0] = cr; color[1] = cg; color[2] = cb;
    atmosphereColor[0] = 0.4f; atmosphereColor[1] = 0.7f;
    atmosphereColor[2] = 1.0f; atmosphereColor[3] = 0.25f;
    ringColor[0] = 0.8f; ringColor[1] = 0.7f;
    ringColor[2] = 0.5f; ringColor[3] = 0.55f;

    quadric_ = gluNewQuadric();
    gluQuadricTexture(quadric_, GL_TRUE);
    gluQuadricNormals(quadric_, GLU_SMOOTH);
}

Planet::~Planet() {
    for (auto* m : moons) delete m;
    moons.clear();
    if (quadric_) { gluDeleteQuadric(quadric_); quadric_ = nullptr; }
}

/* ════════════════════════════════════════════════════════════════
   Setup helpers
   ════════════════════════════════════════════════════════════════ */

void Planet::addMoon(Planet* moon) { moons.push_back(moon); }

void Planet::setTextures(GLuint t, GLuint ct, GLuint rt) {
    textureID = t; cloudTextureID = ct; ringTextureID = rt;
}

void Planet::setAtmosphere(bool has, float sc,
                           float r, float g, float b, float a) {
    hasAtmosphere = has; atmosphereScale = sc;
    atmosphereColor[0]=r; atmosphereColor[1]=g;
    atmosphereColor[2]=b; atmosphereColor[3]=a;
}

void Planet::setRings(bool has, float inner, float outer,
                      float r, float g, float b, float a) {
    hasRings = has; ringInnerRadius = inner; ringOuterRadius = outer;
    ringColor[0]=r; ringColor[1]=g; ringColor[2]=b; ringColor[3]=a;
}

void Planet::setInfo(const std::string& tp, const std::string& dia,
                     const std::string& dist, const std::string& day,
                     const std::string& year, const std::string& temp,
                     const std::string& grav, const std::string& mc,
                     const std::string& atm, const std::string& desc,
                     const std::string& fact) {
    type=tp; diameter=dia; distFromSun=dist;
    dayLength=day; yearLength=year; avgTemp=temp;
    gravityStr=grav; numMoonsStr=mc; atmosphereStr=atm;
    description=desc; funFact=fact;
}

/* ════════════════════════════════════════════════════════════════
   Per-frame update
   CG Concept: Frame-rate independent animation via delta time
   ════════════════════════════════════════════════════════════════ */

void Planet::update(float dt, float speedMult) {
    currentOrbitAngle    += orbitSpeed    * dt * speedMult;
    currentRotationAngle += rotationSpeed * dt * speedMult;

    /* Wrap angles to [-360, 360] */
    if (currentOrbitAngle    >  360.0f) currentOrbitAngle    -= 360.0f;
    if (currentOrbitAngle    < -360.0f) currentOrbitAngle    += 360.0f;
    if (currentRotationAngle >  360.0f) currentRotationAngle -= 360.0f;
    if (currentRotationAngle < -360.0f) currentRotationAngle += 360.0f;

    for (auto* m : moons) m->update(dt, speedMult);
}

/* ════════════════════════════════════════════════════════════════
   Ring rendering  –  disc annulus via GL_TRIANGLE_STRIP
   CG Concept: Custom geometry, texture mapping, blending
   ════════════════════════════════════════════════════════════════ */

void Planet::drawRings() {
    const int segs = 200;  /* smooth rings */
    float inner = ringInnerRadius, outer = ringOuterRadius;
    if (inner <= 0.0f || outer <= 0.0f) return;

    glPushMatrix();
    /* Saturn ring tilt to match axial tilt */
    glRotatef(axialTilt, 0.0f, 0.0f, 1.0f);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glDisable(GL_LIGHTING);
    glDisable(GL_CULL_FACE);

    if (ringTextureID) {
        glEnable(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, ringTextureID);
        glColor4f(1.0f, 1.0f, 1.0f, ringColor[3]);
    } else {
        glDisable(GL_TEXTURE_2D);
    }

    /* Main ring strip */
    glBegin(GL_TRIANGLE_STRIP);
    for (int i = 0; i <= segs; i++) {
        float theta = 2.0f * (float)M_PI * (float)i / segs;
        float ct = cosf(theta), st = sinf(theta);
        float tx = (float)i / segs;

        if (!ringTextureID) {
            /* Subtle brightness variation for untextured rings */
            float bright = 0.75f + 0.25f * cosf(theta * 4.0f);
            glColor4f(ringColor[0]*bright, ringColor[1]*bright,
                      ringColor[2]*bright, ringColor[3]);
        }

        if (ringTextureID) glTexCoord2f(tx, 1.0f);
        glVertex3f(outer * ct, 0.0f, outer * st);
        if (ringTextureID) glTexCoord2f(tx, 0.0f);
        glVertex3f(inner * ct, 0.0f, inner * st);
    }
    glEnd();

    if (ringTextureID) glDisable(GL_TEXTURE_2D);

    /* Outer edge glow */
    glBegin(GL_LINE_LOOP);
    glColor4f(1.0f, 0.95f, 0.7f, 0.25f);
    for (int i = 0; i < segs; i++) {
        float theta = 2.0f * (float)M_PI * (float)i / segs;
        glVertex3f(outer * cosf(theta), 0.0f, outer * sinf(theta));
    }
    glEnd();

    glEnable(GL_CULL_FACE);
    glEnable(GL_LIGHTING);
    glDisable(GL_BLEND);
    glPopMatrix();
}

/* ════════════════════════════════════════════════════════════════
   Atmosphere glow  –  Fresnel-style additive back-face sphere
   CG Concept: Blending (additive), depth masking, back-face culling
   ════════════════════════════════════════════════════════════════ */

void Planet::drawAtmosphere() {
    /* 3-pass additive atmosphere: inner thick + outer thin haze */
    float scales[] = { atmosphereScale,
                       atmosphereScale * 1.05f,
                       atmosphereScale * 1.12f };
    float alphas[] = { atmosphereColor[3],
                       atmosphereColor[3] * 0.50f,
                       atmosphereColor[3] * 0.20f };

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE); /* additive for glow */
    glDepthMask(GL_FALSE);
    glDisable(GL_LIGHTING);

    /* Render back-faces only so glow appears around the edge (Fresnel-like) */
    glEnable(GL_CULL_FACE);
    glCullFace(GL_FRONT);

    for (int pass = 0; pass < 3; pass++) {
        glColor4f(atmosphereColor[0], atmosphereColor[1],
                  atmosphereColor[2], alphas[pass]);
        glPushMatrix();
        /* Slowly rotate cloud/atmosphere layer */
        if (cloudTextureID && pass == 0) {
            glEnable(GL_TEXTURE_2D);
            glBindTexture(GL_TEXTURE_2D, cloudTextureID);
            glColor4f(1.0f, 1.0f, 1.0f, alphas[pass] * 1.8f);
            glRotatef(currentRotationAngle * 1.15f, 0, 1, 0);
        }
        gluSphere(quadric_, radius * scales[pass], 48, 48);
        if (cloudTextureID && pass == 0) glDisable(GL_TEXTURE_2D);
        glPopMatrix();
    }

    glCullFace(GL_BACK);
    glEnable(GL_LIGHTING);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glDisable(GL_BLEND);
    glDepthMask(GL_TRUE);
}

/* ════════════════════════════════════════════════════════════════
   Sun multi-pass corona glow
   CG Concept: Additive blending, multi-pass rendering
   ════════════════════════════════════════════════════════════════ */

void Planet::drawSunGlow() {
    /* 7 nested spheres with additive blending – creates a warm corona */
    const float scales[] = { 1.10f, 1.25f, 1.45f, 1.75f, 2.20f, 2.80f, 3.60f };
    const float alphas[] = { 0.22f, 0.14f, 0.09f, 0.06f, 0.035f, 0.018f, 0.008f };

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE); /* additive — makes it glow */
    glDepthMask(GL_FALSE);
    glDisable(GL_LIGHTING);
    glDisable(GL_CULL_FACE);

    for (int i = 0; i < 7; i++) {
        /* Colour transitions: white core → orange → red outer corona */
        float t = (float)i / 6.0f;
        float cr = 1.0f;
        float cg = 1.0f - t * 0.50f;
        float cb = 1.0f - t * 0.95f;
        glColor4f(cr, cg, cb, alphas[i]);
        glutSolidSphere(radius * scales[i], 32, 32);
    }

    glEnable(GL_CULL_FACE);
    glEnable(GL_LIGHTING);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glDisable(GL_BLEND);
    glDepthMask(GL_TRUE);
}

/* ════════════════════════════════════════════════════════════════
   Orbit path
   CG Concept: Line rendering, blending, orbit geometry
   ════════════════════════════════════════════════════════════════ */

void Planet::drawOrbit() {
    const int segs = 180;
    if (orbitRadius <= 0.0f) return;

    glPushMatrix();
    if (orbitTilt != 0.0f) glRotatef(orbitTilt, 0, 0, 1);

    glDisable(GL_LIGHTING);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    /* Outer faint glow pass */
    glLineWidth(2.5f);
    glColor4f(color[0], color[1], color[2], 0.08f);
    glBegin(GL_LINE_LOOP);
    for (int i = 0; i < segs; i++) {
        float t = 2.0f * (float)M_PI * i / segs;
        glVertex3f(orbitRadius * cosf(t), 0, orbitRadius * sinf(t));
    }
    glEnd();

    /* Inner crisp line */
    glLineWidth(1.0f);
    glColor4f(color[0] * 0.7f + 0.15f,
              color[1] * 0.7f + 0.15f,
              color[2] * 0.7f + 0.15f, 0.38f);
    glBegin(GL_LINE_LOOP);
    for (int i = 0; i < segs; i++) {
        float t = 2.0f * (float)M_PI * i / segs;
        glVertex3f(orbitRadius * cosf(t), 0, orbitRadius * sinf(t));
    }
    glEnd();

    glDisable(GL_BLEND);
    glLineWidth(1.0f);
    glEnable(GL_LIGHTING);
    glPopMatrix();
}

/* ════════════════════════════════════════════════════════════════
   Body rendering  –  textured or coloured sphere
   CG Concept: Texture mapping, material properties, wireframe mode
   ════════════════════════════════════════════════════════════════ */

void Planet::drawBody(bool useTextures, bool wireframe) {
    bool isSun = (name == "Sun");

    /* Sphere quality: higher slices for larger / closer planets */
    int slices = wireframe ? 24 : 72;
    if (radius < 0.3f) slices = wireframe ? 16 : 48; /* smaller bodies */

    if (wireframe)
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

    if (isSun) glDisable(GL_LIGHTING);

    if (useTextures && textureID) {
        glEnable(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, textureID);
        glColor3f(1.0f, 1.0f, 1.0f);
        gluSphere(quadric_, radius, slices, slices);
        glDisable(GL_TEXTURE_2D);
    } else {
        glColor3f(color[0], color[1], color[2]);
        gluSphere(quadric_, radius, slices, slices);
    }

    if (isSun) glEnable(GL_LIGHTING);

    if (wireframe)
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
}

/* ════════════════════════════════════════════════════════════════
   Main draw  –  hierarchical transformation pipeline
   CG Concepts: Translation, Rotation, Matrix stack,
                Hierarchical transforms (parent → child for moons)
   ════════════════════════════════════════════════════════════════ */

void Planet::draw(bool showOrbit, bool useTextures,
                  bool isSelected, bool wireframe)
{
    bool isSun = (name == "Sun");

    /* ── BEGIN hierarchical transform ── */
    glPushMatrix();                         /* CG: Save parent coordinate frame */

    /* 1. Orbital plane inclination */
    if (orbitTilt != 0.0f)
        glRotatef(orbitTilt, 0, 0, 1);     /* CG: Rotation – tilt orbit plane */

    /* 2. Revolution around parent (orbit) */
    if (orbitRadius > 0.0f) {
        glRotatef(currentOrbitAngle, 0, 1, 0); /* CG: Rotation – revolution */
        glTranslatef(orbitRadius, 0, 0);        /* CG: Translation – orbit offset */
    }

    /* 3. Extract world position from current modelview (for UI label projection) */
    {
        GLfloat mv[16];
        glGetFloatv(GL_MODELVIEW_MATRIX, mv);
        posX = mv[12]; posY = mv[13]; posZ = mv[14];
    }

    /* 4. Set material properties */
    GLfloat mat_amb[4], mat_diff[4], mat_spec[4], mat_emis[4];
    float   shininess;

    mat_amb[0]  = color[0] * 0.02f; mat_amb[1]  = color[1] * 0.02f;
    mat_amb[2]  = color[2] * 0.02f; mat_amb[3]  = 1.0f;
    mat_diff[0] = color[0];         mat_diff[1] = color[1];
    mat_diff[2] = color[2];         mat_diff[3] = 1.0f;
    mat_spec[0] = 0.12f; mat_spec[1] = 0.12f;
    mat_spec[2] = 0.12f; mat_spec[3] = 1.0f;
    mat_emis[0] = mat_emis[1] = mat_emis[2] = 0.0f; mat_emis[3] = 1.0f;
    shininess = 18.0f;

    if (isSun) {
        /* Sun: self-luminous emission material */
        mat_emis[0] = color[0] * 1.0f;
        mat_emis[1] = color[1] * 0.85f;
        mat_emis[2] = 0.05f;
        mat_amb[0]  = color[0]; mat_amb[1] = color[1]; mat_amb[2] = color[2];
        shininess = 0.0f;
    } else if (isSelected) {
        /* Selected: faint blue-white emission tint */
        mat_emis[0] = 0.15f; mat_emis[1] = 0.18f; mat_emis[2] = 0.28f;
        shininess = 50.0f;
    }

    glMaterialfv(GL_FRONT, GL_AMBIENT,   mat_amb);
    glMaterialfv(GL_FRONT, GL_DIFFUSE,   mat_diff);
    glMaterialfv(GL_FRONT, GL_SPECULAR,  mat_spec);
    glMaterialfv(GL_FRONT, GL_EMISSION,  mat_emis);
    glMaterialf (GL_FRONT, GL_SHININESS, shininess);

    /* 5. Save matrix before applying axial tilt (moons inherit orbit position only) */
    glPushMatrix();

    /* 6. Axial tilt – CG: Rotation applied before self-rotation */
    if (axialTilt != 0.0f)
        glRotatef(axialTilt, 0, 0, 1);     /* CG: Rotation about Z = axis tilt */

    /* 7. Self-rotation (spin) */
    glRotatef(currentRotationAngle, 0, 1, 0); /* CG: Rotation */

    /* 8. Texture axis correction (-90° on X so poles align with Y-axis) */
    glPushMatrix();
    glRotatef(-90.0f, 1, 0, 0);

    /* 9. Draw sphere body */
    drawBody(useTextures, wireframe);

    glPopMatrix(); /* pop texture axis correction */

    /* 10. Sun corona glow */
    if (isSun) drawSunGlow();

    glPopMatrix(); /* back to orbit position (before axial tilt) */

    /* 11. Rings – drawn at orbit position, tilt via drawRings() */
    if (hasRings && ringInnerRadius > 0.0f)
        drawRings();

    /* 12. Atmosphere */
    if (hasAtmosphere)
        drawAtmosphere();

    /* 13. Selection highlight ring (animated pulsing) */
    if (isSelected && !isSun) {
        /* Use a static animation counter — small hack, good visual result */
        static float pulse = 0.0f;
        pulse += 0.04f;
        float alpha = 0.35f + 0.20f * sinf(pulse);
        float hr    = radius * 1.35f;
        int   hs    = 80;

        glDisable(GL_LIGHTING);
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        /* Outer glow ring */
        glLineWidth(3.0f);
        glColor4f(0.25f, 0.65f, 1.0f, alpha * 0.5f);
        glBegin(GL_LINE_LOOP);
        for (int i = 0; i < hs; i++) {
            float t = 2.0f * (float)M_PI * i / hs;
            glVertex3f(hr * 1.15f * cosf(t), 0, hr * 1.15f * sinf(t));
        }
        glEnd();

        /* Inner precise ring */
        glLineWidth(1.5f);
        glColor4f(0.40f, 0.80f, 1.0f, alpha);
        glBegin(GL_LINE_LOOP);
        for (int i = 0; i < hs; i++) {
            float t = 2.0f * (float)M_PI * i / hs;
            glVertex3f(hr * cosf(t), 0, hr * sinf(t));
        }
        glEnd();

        glLineWidth(1.0f);
        glDisable(GL_BLEND);
        glEnable(GL_LIGHTING);
    }

    /* 14. Draw moons (hierarchical child transforms)
       CG Concept: Child objects inherit parent's current transformation */
    for (size_t i = 0; i < moons.size(); i++) {
        if (showOrbit) moons[i]->drawOrbit();
        moons[i]->draw(showOrbit, useTextures, false, wireframe);
    }

    glPopMatrix(); /* CG: Restore parent coordinate frame */
}
