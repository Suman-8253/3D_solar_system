#ifndef INPUT_H
#define INPUT_H

/*  ═══════════════════════════════════════════════════════════════
    Input  –  keyboard, special keys, mouse, mouse motion handlers
    ═══════════════════════════════════════════════════════════════ */

#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

#include "Camera.h"
#include "SolarSystem.h"

class Input {
public:
    /* Keyboard: WASD/QE camera, Space pause, +/- speed, toggles */
    static void handleKeyboard(unsigned char key, int x, int y,
                               Camera& cam, SolarSystem& sys,
                               bool& isPaused, float& speedMult,
                               bool& showOrbits, bool& showLabels,
                               bool& wireframeMode, bool& texturesEnabled,
                               bool& isFullscreen,
                               int& screenWidth, int& screenHeight);

    /* Arrow keys for camera tilt/pan */
    static void handleSpecialKeys(int key, int x, int y, Camera& cam);

    /* Mouse buttons: left-drag orbit, right-drag pan, scroll zoom, click UI */
    static void handleMouse(int button, int state, int x, int y,
                            Camera& cam, SolarSystem& sys,
                            int screenWidth, int screenHeight);

    /* Mouse motion: drag to orbit/pan */
    static void handleMouseMotion(int x, int y, Camera& cam);
};

#endif /* INPUT_H */
