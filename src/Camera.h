#ifndef CAMERA_H
#define CAMERA_H

#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif
#include <math.h>

typedef enum {
    CAM_FREE = 0,
    CAM_PLANET_FOCUS = 1
} CameraMode;

typedef struct Camera {
    /* Current state */
    float posX, posY, posZ;
    float lookX, lookY, lookZ;
    float upX, upY, upZ;

    float angleY;     /* Yaw   (degrees, around Y axis) */
    float angleX;     /* Pitch (degrees, around X axis) */
    float distance;   /* Distance from focus point      */

    /* Focus/pan offset (for planet focus mode) */
    float focusX, focusY, focusZ;

    /* Smooth interpolation targets */
    float targetAngleX, targetAngleY;
    float targetDistance;
    float targetFocusX, targetFocusY, targetFocusZ;

    /* Mouse drag state */
    int   isDragging;
    int   lastMouseX, lastMouseY;

    /* Camera mode */
    CameraMode mode;
} Camera;

void Camera_init(Camera* cam);
void Camera_reset(Camera* cam);
void Camera_updateCameraVectors(Camera* cam);
void Camera_updateView(Camera* cam);
void Camera_smoothUpdate(Camera* cam, float dt);

void Camera_focusPlanet(Camera* cam, float px, float py, float pz, float radius);
void Camera_focusSolarSystem(Camera* cam);

void Camera_processKeyboard(Camera* cam, unsigned char key, int x, int y);
void Camera_processSpecialKeys(Camera* cam, int key, int x, int y);
void Camera_processMouse(Camera* cam, int button, int state, int x, int y);
void Camera_processMouseMotion(Camera* cam, int x, int y);

void Camera_zoomIn(Camera* cam);
void Camera_zoomOut(Camera* cam);

#endif /* CAMERA_H */
