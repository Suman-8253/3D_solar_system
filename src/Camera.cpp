#include "Camera.h"
#include <cmath>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

#define LERP_SPEED 6.0f

static float lerp(float a, float b, float t) { return a + (b - a) * t; }
static float lerpAngle(float a, float b, float t) { return a + (b - a) * t; }

Camera::Camera() {
    isDragging = 0;
    isRightDragging = 0;
    lastMouseX = 0;
    lastMouseY = 0;
    mode = CAM_FREE;
    currentView = VIEW_OVERVIEW;
    reset();
}

void Camera::reset() {
    angleY = 0.0f;
    angleX = 30.0f;
    distance = 65.0f;
    focusX = focusY = focusZ = 0.0f;

    targetAngleX  = angleX;
    targetAngleY  = angleY;
    targetDistance = distance;
    targetFocusX  = focusX;
    targetFocusY  = focusY;
    targetFocusZ  = focusZ;

    mode = CAM_FREE;
    currentView = VIEW_OVERVIEW;
    updateVectors();
}

void Camera::updateVectors() {
    float radY = angleY * (float)M_PI / 180.0f;
    float radX = angleX * (float)M_PI / 180.0f;

    posX = focusX + distance * sinf(radY) * cosf(radX);
    posY = focusY + distance * sinf(radX);
    posZ = focusZ + distance * cosf(radY) * cosf(radX);

    lookX = focusX;
    lookY = focusY;
    lookZ = focusZ;

    upX = 0.0f;
    upY = cosf(radX) >= 0.0f ? 1.0f : -1.0f;
    upZ = 0.0f;
}

void Camera::applyView() {
    gluLookAt(posX, posY, posZ, lookX, lookY, lookZ, upX, upY, upZ);
}

void Camera::smoothUpdate(float dt) {
    float t = 1.0f - expf(-LERP_SPEED * dt);
    if (t > 1.0f) t = 1.0f;

    angleX   = lerpAngle(angleX,   targetAngleX,   t);
    angleY   = lerpAngle(angleY,   targetAngleY,   t);
    distance = lerp(distance, targetDistance, t);
    focusX   = lerp(focusX,   targetFocusX,   t);
    focusY   = lerp(focusY,   targetFocusY,   t);
    focusZ   = lerp(focusZ,   targetFocusZ,   t);

    updateVectors();
}

void Camera::focusPlanet(float px, float py, float pz, float radius) {
    float viewDist = radius * 8.0f;
    if (viewDist < 4.0f)  viewDist = 4.0f;
    if (viewDist > 40.0f) viewDist = 40.0f;

    targetFocusX = px;
    targetFocusY = py;
    targetFocusZ = pz;
    targetDistance = viewDist;
    targetAngleX = 20.0f;
    mode = CAM_PLANET_FOCUS;
    currentView = VIEW_PLANET;
}

void Camera::focusSolarSystem() {
    targetFocusX = targetFocusY = targetFocusZ = 0.0f;
    targetDistance = 65.0f;
    targetAngleX = 30.0f;
    mode = CAM_FREE;
    currentView = VIEW_OVERVIEW;
}

void Camera::setViewPreset(ViewPreset vp) {
    currentView = vp;
    switch (vp) {
        case VIEW_OVERVIEW:
            focusSolarSystem();
            break;
        case VIEW_TOP:
            targetFocusX = targetFocusY = targetFocusZ = 0.0f;
            targetDistance = 80.0f;
            targetAngleX = 89.0f;
            targetAngleY = 0.0f;
            mode = CAM_FREE;
            break;
        case VIEW_SIDE:
            targetFocusX = targetFocusY = targetFocusZ = 0.0f;
            targetDistance = 75.0f;
            targetAngleX = 0.0f;
            targetAngleY = 90.0f;
            mode = CAM_FREE;
            break;
        case VIEW_FRONT:
            targetFocusX = targetFocusY = targetFocusZ = 0.0f;
            targetDistance = 75.0f;
            targetAngleX = 0.0f;
            targetAngleY = 0.0f;
            mode = CAM_FREE;
            break;
        case VIEW_FREE:
            mode = CAM_FREE;
            break;
        case VIEW_PLANET:
            break;
        case VIEW_REFERENCE:
            targetFocusX = 15.0f; // Shift focus right so sun is on left edge
            targetFocusY = targetFocusZ = 0.0f;
            targetDistance = 65.0f;
            targetAngleX = 20.0f;
            targetAngleY = -5.0f;
            mode = CAM_FREE;
            break;
        case VIEW_COUNT:
            break;
    }
}

void Camera::processKeyboard(unsigned char key) {
    switch (key) {
        case 'w': case 'W': zoomIn(); break;
        case 's': case 'S': zoomOut(); break;
        case 'a': case 'A': targetAngleY -= 5.0f; mode = CAM_FREE; currentView = VIEW_FREE; break;
        case 'd': case 'D': targetAngleY += 5.0f; mode = CAM_FREE; currentView = VIEW_FREE; break;
        case 'q': case 'Q': targetAngleX += 5.0f; if (targetAngleX > 89.0f) targetAngleX = 89.0f; mode = CAM_FREE; currentView = VIEW_FREE; break;
        case 'e': case 'E': targetAngleX -= 5.0f; if (targetAngleX < -89.0f) targetAngleX = -89.0f; mode = CAM_FREE; currentView = VIEW_FREE; break;
        case 'r': case 'R': reset(); break;
    }
}

void Camera::processSpecialKeys(int key) {
    switch (key) {
        case GLUT_KEY_UP:    targetAngleX += 3.0f; if (targetAngleX > 89.0f) targetAngleX = 89.0f; mode = CAM_FREE; currentView = VIEW_FREE; break;
        case GLUT_KEY_DOWN:  targetAngleX -= 3.0f; if (targetAngleX < -89.0f) targetAngleX = -89.0f; mode = CAM_FREE; currentView = VIEW_FREE; break;
        case GLUT_KEY_LEFT:  targetAngleY -= 3.0f; mode = CAM_FREE; currentView = VIEW_FREE; break;
        case GLUT_KEY_RIGHT: targetAngleY += 3.0f; mode = CAM_FREE; currentView = VIEW_FREE; break;
    }
}

void Camera::processMouse(int button, int state, int x, int y) {
    if (button == 3) { zoomIn(); return; }
    if (button == 4) { zoomOut(); return; }

    if (button == GLUT_LEFT_BUTTON) {
        if (state == GLUT_DOWN) {
            isDragging = 1;
            lastMouseX = x;
            lastMouseY = y;
        } else {
            isDragging = 0;
        }
    } else if (button == GLUT_RIGHT_BUTTON) {
        if (state == GLUT_DOWN) {
            isRightDragging = 1;
            lastMouseX = x;
            lastMouseY = y;
            mode = CAM_FREE;
            currentView = VIEW_FREE;
        } else {
            isRightDragging = 0;
        }
    }
}

void Camera::processMouseMotion(int x, int y) {
    float dx = (float)(x - lastMouseX);
    float dy = (float)(y - lastMouseY);
    lastMouseX = x;
    lastMouseY = y;

    if (isDragging) {
        targetAngleY += dx * 0.4f;
        targetAngleX += dy * 0.3f;
        if (targetAngleX >  89.0f) targetAngleX =  89.0f;
        if (targetAngleX < -89.0f) targetAngleX = -89.0f;
        mode = CAM_FREE;
        currentView = VIEW_FREE;
    } else if (isRightDragging) {
        /* Pan the focus point */
        float radY = angleY * (float)M_PI / 180.0f;
        float rightX = cosf(radY);
        float rightZ = -sinf(radY);
        
        targetFocusX -= rightX * dx * 0.05f;
        targetFocusZ -= rightZ * dx * 0.05f;
        targetFocusY += dy * 0.05f;
    }
}

void Camera::zoomIn() {
    targetDistance -= 3.0f;
    if (targetDistance < 3.0f) targetDistance = 3.0f;
}

void Camera::zoomOut() {
    targetDistance += 3.0f;
    if (targetDistance > 250.0f) targetDistance = 250.0f;
}
