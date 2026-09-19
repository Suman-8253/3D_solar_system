#ifndef CAMERA_H
#define CAMERA_H

/*  ═══════════════════════════════════════════════════════════════
    Camera  –  spherical orbit camera with smooth interpolation
    CG Concepts: Camera transformation (gluLookAt), perspective
                 projection, spherical coordinates
    ═══════════════════════════════════════════════════════════════ */

#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

enum CameraMode { CAM_FREE = 0, CAM_PLANET_FOCUS = 1 };

enum ViewPreset {
    VIEW_OVERVIEW = 0,
    VIEW_TOP      = 1,
    VIEW_SIDE     = 2,
    VIEW_FRONT    = 3,
    VIEW_PLANET   = 4,
    VIEW_FREE     = 5,
    VIEW_REFERENCE= 6,
    VIEW_COUNT    = 7
};

class Camera {
public:
    /* Current state */
    float posX, posY, posZ;
    float lookX, lookY, lookZ;
    float upX, upY, upZ;

    float angleY;        /* Yaw   (degrees) */
    float angleX;        /* Pitch (degrees) */
    float distance;

    /* Focus point (pan target) */
    float focusX, focusY, focusZ;

    /* Smooth interpolation targets */
    float targetAngleX, targetAngleY;
    float targetDistance;
    float targetFocusX, targetFocusY, targetFocusZ;

    /* Mouse drag state */
    int  isDragging;
    int  isRightDragging;    /* Right-click pan */
    int  lastMouseX, lastMouseY;

    CameraMode  mode;
    ViewPreset  currentView;

    Camera();
    void reset();
    void updateVectors();
    void applyView();           /* calls gluLookAt */
    void smoothUpdate(float dt);

    /* Focus helpers */
    void focusPlanet(float px, float py, float pz, float radius);
    void focusSolarSystem();
    void setViewPreset(ViewPreset vp);

    /* Input handlers */
    void processKeyboard(unsigned char key);
    void processSpecialKeys(int key);
    void processMouse(int button, int state, int x, int y);
    void processMouseMotion(int x, int y);

    void zoomIn();
    void zoomOut();
};

#endif /* CAMERA_H */
