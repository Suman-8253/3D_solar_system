#include "Input.h"
#include <cstdlib>

/*  ═══════════════════════════════════════════════════════════════
    Input::handleKeyboard
    Key bindings:
      W/S          Zoom in/out
      A/D          Rotate view left/right
      Q/E          Pitch view up/down
      R            Reset camera
      Space        Pause / Resume
      +/=          Speed up
      -/_          Slow down
      O            Toggle orbit paths
      L            Toggle planet labels
      T            Toggle textures
      V            Toggle wireframe
      F            Toggle fullscreen
      1            Overview (system view)
      2            Top-down view
      3            Side view
      4            Front view
      5            Focus selected planet
      0–9          Select planet by index (0=Sun,1=Mercury,...,9=Pluto)
      ESC          If in planet view → go to overview; else exit
    ═══════════════════════════════════════════════════════════════ */

void Input::handleKeyboard(unsigned char key, int x, int y,
                           Camera& cam, SolarSystem& sys,
                           bool& isPaused, float& speedMult,
                           bool& showOrbits, bool& showLabels,
                           bool& wireframeMode, bool& texturesEnabled,
                           bool& isFullscreen,
                           int& screenWidth, int& screenHeight)
{
    /* Camera movement */
    cam.processKeyboard(key);

    switch (key) {
        /* ── System ── */
        case 27: /* ESC */
            if (cam.currentView == VIEW_PLANET) {
                cam.setViewPreset(VIEW_OVERVIEW);
            } else {
                exit(0);
            }
            break;

        /* ── Simulation ── */
        case ' ':
            isPaused = !isPaused;
            break;

        case '+': case '=':
            speedMult *= 2.0f;
            if (speedMult > 100.0f) speedMult = 100.0f;
            break;

        case '-': case '_':
            speedMult *= 0.5f;
            if (speedMult < 0.1f) speedMult = 0.1f;
            break;

        /* ── Visual Toggles ── */
        case 'o': case 'O':
            showOrbits = !showOrbits;
            break;

        case 'l': case 'L':
            showLabels = !showLabels;
            break;

        case 't': case 'T':
            texturesEnabled = !texturesEnabled;
            break;

        case 'v': case 'V':
            wireframeMode = !wireframeMode;
            break;

        case 'f': case 'F':
            isFullscreen = !isFullscreen;
            if (isFullscreen) {
                glutFullScreen();
            } else {
                glutReshapeWindow(screenWidth, screenHeight);
                glutPositionWindow(100, 100);
            }
            break;

        /* ── Camera presets ── */
        case '1': cam.setViewPreset(VIEW_OVERVIEW); break;
        case '2': cam.setViewPreset(VIEW_TOP);      break;
        case '3': cam.setViewPreset(VIEW_SIDE);     break;
        case '4': cam.setViewPreset(VIEW_FRONT);    break;
        case '5':
            if (Planet* p = sys.getSelectedPlanet()) {
                cam.focusPlanet(p->posX, p->posY, p->posZ, p->radius);
            }
            break;

        /* ── Direct planet selection (0=Sun, 1=Mercury ... 9=Pluto) ── */
        case '0': case '6': case '7': case '8': case '9': {
            int idx = (key == '0') ? 0 : (key - '0');
            if (idx < (int)sys.planets.size()) {
                sys.selectPlanet(idx);
                Planet* p = sys.planets[idx];
                cam.focusPlanet(p->posX, p->posY, p->posZ, p->radius);
            }
            break;
        }
    }
}

void Input::handleSpecialKeys(int key, int x, int y, Camera& cam) {
    cam.processSpecialKeys(key);
}

void Input::handleMouse(int button, int state, int x, int y,
                        Camera& cam, SolarSystem& sys,
                        int screenWidth, int screenHeight)
{
    cam.processMouse(button, state, x, y);

    /* Click on bottom planet selector bar to select & focus planet */
    if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
        float my = (float)(screenHeight - y);  /* flip Y: OpenGL origin is bottom-left */

        /* Bottom bar occupies Y ∈ [20, 80] in our ortho space */
        if (my >= 20.0f && my <= 80.0f) {
            float px = 20.0f;
            for (size_t i = 0; i < sys.planets.size(); ++i) {
                if ((float)x >= px && (float)x <= px + 70.0f) {
                    sys.selectPlanet((int)i);
                    cam.focusPlanet(sys.planets[i]->posX,
                                    sys.planets[i]->posY,
                                    sys.planets[i]->posZ,
                                    sys.planets[i]->radius);
                    break;
                }
                px += 78.0f;
            }
        }
    }
}

void Input::handleMouseMotion(int x, int y, Camera& cam) {
    cam.processMouseMotion(x, y);
}
