#include "Camera.h"
#include <stdio.h>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

#define LERP_SPEED 6.0f   /* Smoothing factor for camera interpolation */

/* ---------- helpers ---------- */

static float lerp(float a, float b, float t) {
    return a + (b - a) * t;
}

static float lerpAngle(float a, float b, float t) {
    /* Lerp directly – angles are kept unrestricted so no wrapping needed */
    return a + (b - a) * t;
}

/* ---------- init ---------- */

void Camera_init(Camera* cam) {
    cam->isDragging  = 0;
    cam->lastMouseX  = 0;
    cam->lastMouseY  = 0;
    cam->mode        = CAM_FREE;
    Camera_reset(cam);
}

void Camera_reset(Camera* cam) {
    cam->angleY   = 0.0f;
    cam->angleX   = 30.0f;   /* Slight top-down view */
    cam->distance = 65.0f;

    cam->focusX = 0.0f;
    cam->focusY = 0.0f;
    cam->focusZ = 0.0f;

    /* Targets match current so there's no initial lerp drift */
    cam->targetAngleX  = cam->angleX;
    cam->targetAngleY  = cam->angleY;
    cam->targetDistance = cam->distance;
    cam->targetFocusX  = cam->focusX;
    cam->targetFocusY  = cam->focusY;
    cam->targetFocusZ  = cam->focusZ;

    cam->mode = CAM_FREE;

    Camera_updateCameraVectors(cam);
}

/* ---------- core update ---------- */

void Camera_updateCameraVectors(Camera* cam) {
    float radY = cam->angleY * (float)M_PI / 180.0f;
    float radX = cam->angleX * (float)M_PI / 180.0f;

    /* Spherical → Cartesian, offset by focus point */
    cam->posX = cam->focusX + cam->distance * sinf(radY) * cosf(radX);
    cam->posY = cam->focusY + cam->distance * sinf(radX);
    cam->posZ = cam->focusZ + cam->distance * cosf(radY) * cosf(radX);

    /* Always look at the focus point */
    cam->lookX = cam->focusX;
    cam->lookY = cam->focusY;
    cam->lookZ = cam->focusZ;

    /* Flip up vector when pitched past vertical */
    cam->upX = 0.0f;
    cam->upY = cosf(radX) >= 0.0f ? 1.0f : -1.0f;
    cam->upZ = 0.0f;
}

void Camera_updateView(Camera* cam) {
    gluLookAt(cam->posX, cam->posY, cam->posZ,
              cam->lookX, cam->lookY, cam->lookZ,
              cam->upX,  cam->upY,  cam->upZ);
}

/* Smooth interpolation – call once per frame with delta time */
void Camera_smoothUpdate(Camera* cam, float dt) {
    float t = 1.0f - expf(-LERP_SPEED * dt);   /* Exponential ease */
    if (t > 1.0f) t = 1.0f;

    cam->angleX   = lerpAngle(cam->angleX,   cam->targetAngleX,   t);
    cam->angleY   = lerpAngle(cam->angleY,   cam->targetAngleY,   t);
    cam->distance = lerp(cam->distance, cam->targetDistance, t);
    cam->focusX   = lerp(cam->focusX,   cam->targetFocusX,   t);
    cam->focusY   = lerp(cam->focusY,   cam->targetFocusY,   t);
    cam->focusZ   = lerp(cam->focusZ,   cam->targetFocusZ,   t);

    Camera_updateCameraVectors(cam);
}

/* ---------- focus helpers ---------- */

void Camera_focusPlanet(Camera* cam, float px, float py, float pz, float radius) {
    float viewDist = radius * 8.0f;
    if (viewDist < 4.0f)  viewDist = 4.0f;
    if (viewDist > 40.0f) viewDist = 40.0f;

    cam->targetFocusX  = px;
    cam->targetFocusY  = py;
    cam->targetFocusZ  = pz;
    cam->targetDistance = viewDist;
    cam->targetAngleX  = 20.0f;
    cam->mode = CAM_PLANET_FOCUS;
}

void Camera_focusSolarSystem(Camera* cam) {
    cam->targetFocusX  = 0.0f;
    cam->targetFocusY  = 0.0f;
    cam->targetFocusZ  = 0.0f;
    cam->targetDistance = 65.0f;
    cam->targetAngleX  = 30.0f;
    cam->mode = CAM_FREE;
}

/* ---------- input ---------- */

void Camera_processKeyboard(Camera* cam, unsigned char key, int x, int y) {
    (void)x; (void)y;
    switch (key) {
        case 'w': case 'W':
            cam->targetDistance -= 2.0f;
            if (cam->targetDistance < 3.0f) cam->targetDistance = 3.0f;
            break;
        case 's': case 'S':
            cam->targetDistance += 2.0f;
            if (cam->targetDistance > 250.0f) cam->targetDistance = 250.0f;
            break;
        case 'a': case 'A':
            cam->targetAngleY -= 5.0f;
            cam->mode = CAM_FREE;
            break;
        case 'd': case 'D':
            cam->targetAngleY += 5.0f;
            cam->mode = CAM_FREE;
            break;
        case 'q': case 'Q':
            cam->targetAngleX += 5.0f;
            if (cam->targetAngleX > 89.0f) cam->targetAngleX = 89.0f;
            break;
        case 'e': case 'E':
            cam->targetAngleX -= 5.0f;
            if (cam->targetAngleX < -89.0f) cam->targetAngleX = -89.0f;
            break;
        case 'r': case 'R':
            Camera_focusSolarSystem(cam);
            cam->targetAngleX = 30.0f;
            cam->targetAngleY = 0.0f;
            cam->targetDistance = 65.0f;
            break;
        default:
            break;
    }
}

void Camera_processSpecialKeys(Camera* cam, int key, int x, int y) {
    (void)x; (void)y;
    switch (key) {
        case GLUT_KEY_UP:
            cam->targetAngleX += 3.0f;
            if (cam->targetAngleX > 89.0f) cam->targetAngleX = 89.0f;
            break;
        case GLUT_KEY_DOWN:
            cam->targetAngleX -= 3.0f;
            if (cam->targetAngleX < -89.0f) cam->targetAngleX = -89.0f;
            break;
        case GLUT_KEY_LEFT:
            cam->targetAngleY -= 3.0f;
            break;
        case GLUT_KEY_RIGHT:
            cam->targetAngleY += 3.0f;
            break;
    }
}

void Camera_processMouse(Camera* cam, int button, int state, int x, int y) {
    /* Mouse wheel: FreeGLUT reports scroll as button 3 (up) and 4 (down) */
    if (button == 3) {
        cam->targetDistance -= 3.0f;
        if (cam->targetDistance < 3.0f) cam->targetDistance = 3.0f;
        return;
    }
    if (button == 4) {
        cam->targetDistance += 3.0f;
        if (cam->targetDistance > 250.0f) cam->targetDistance = 250.0f;
        return;
    }

    /* Left button drag for orbit */
    if (button == GLUT_LEFT_BUTTON) {
        if (state == GLUT_DOWN) {
            cam->isDragging = 1;
            cam->lastMouseX = x;
            cam->lastMouseY = y;
        } else {
            cam->isDragging = 0;
        }
    }
}

void Camera_processMouseMotion(Camera* cam, int x, int y) {
    if (!cam->isDragging) return;

    float dx = (float)(x - cam->lastMouseX);
    float dy = (float)(y - cam->lastMouseY);

    cam->lastMouseX = x;
    cam->lastMouseY = y;

    /* Sensitivity */
    cam->targetAngleY += dx * 0.4f;
    cam->targetAngleX += dy * 0.3f;

    /* Clamp pitch */
    if (cam->targetAngleX >  89.0f) cam->targetAngleX =  89.0f;
    if (cam->targetAngleX < -89.0f) cam->targetAngleX = -89.0f;

    cam->mode = CAM_FREE;
}

void Camera_zoomIn(Camera* cam) {
    cam->targetDistance -= 3.0f;
    if (cam->targetDistance < 3.0f) cam->targetDistance = 3.0f;
}

void Camera_zoomOut(Camera* cam) {
    cam->targetDistance += 3.0f;
    if (cam->targetDistance > 250.0f) cam->targetDistance = 250.0f;
}
