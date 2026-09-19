/*  ═══════════════════════════════════════════════════════════════════
    3D Solar System Simulation  –  main.cpp
    MCA Computer Graphics Final Project

    Entry point: initialises GLUT, creates window, registers callbacks
    and enters the main rendering loop.

    CG Concepts demonstrated:
      • Perspective projection (gluPerspective)
      • Camera transformation (gluLookAt)
      • Hierarchical transforms (glPushMatrix / glPopMatrix)
      • Lighting (GL_LIGHT0 – Sun, GL_LIGHT1 – fill)
      • Texture mapping (stb_image → OpenGL textures)
      • Blending & transparency
      • Depth buffering (GL_DEPTH_TEST)
      • Frame-rate independent animation (delta time)
      • Orbit / revolution / axial rotation
    ═══════════════════════════════════════════════════════════════════ */

#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

#include "SolarSystem.h"
#include "Camera.h"
#include "Renderer.h"
#include "Lighting.h"
#include "UI.h"
#include "Input.h"

/* ─────────────────────────────────────────────────────────────────
   Application State
   ───────────────────────────────────────────────────────────────── */

SolarSystem* g_solarSystem = nullptr;
Camera       g_camera;
Renderer     g_renderer;

/* Window */
int  g_screenWidth  = 1280;
int  g_screenHeight = 720;
bool g_isFullscreen = false;

/* Simulation toggles */
bool  g_isPaused        = false;   /* Start running immediately */
float g_speedMult       = 1.0f;
bool  g_showOrbits      = true;
bool  g_showLabels      = true;
bool  g_wireframeMode   = false;
bool  g_texturesEnabled = true;
bool  g_lightingEnabled = true;

/* Timing & FPS */
int   g_lastTime   = 0;
int   g_frameCount = 0;
float g_fps        = 0.0f;
int   g_fpsTimer   = 0;

/* ─────────────────────────────────────────────────────────────────
   GLUT Callbacks
   ───────────────────────────────────────────────────────────────── */

void display() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    /* 1. Set up 3-D perspective projection */
    g_renderer.setPerspective(g_screenWidth, g_screenHeight);

    /* 2. Apply camera view transform (gluLookAt) */
    g_camera.applyView();

    /* 3. Draw deep-space background (stars + nebula) – no depth write */
    g_renderer.drawStarBackground();

    /* 4. Set up Sun lighting */
    Lighting::setup(g_lightingEnabled);

    /* 5. Draw all planets, orbits, moons, asteroid belt */
    g_solarSystem->draw(g_showOrbits, g_texturesEnabled, g_wireframeMode);

    /* 6. Draw 2-D planet name labels (projected from 3-D positions) */
    if (g_showLabels) {
        UI::drawPlanetLabels(g_screenWidth, g_screenHeight,
                             g_solarSystem->planets, &g_camera);
    }

    /* 7. Draw 2-D HUD overlay */
    UI::drawHUD(g_screenWidth, g_screenHeight,
                g_solarSystem->getSelectedPlanet(),
                g_solarSystem->planets,
                g_speedMult, g_isPaused,
                g_showOrbits, g_showLabels,
                g_lightingEnabled, g_texturesEnabled,
                g_wireframeMode, g_camera.currentView,
                g_fps);

    glutSwapBuffers();
    g_frameCount++;
}

void reshape(int w, int h) {
    if (h == 0) h = 1;
    g_screenWidth  = w;
    g_screenHeight = h;
    glViewport(0, 0, w, h);
}

void idle() {
    int timeNow = glutGet(GLUT_ELAPSED_TIME);
    float dt = (timeNow - g_lastTime) / 1000.0f;
    g_lastTime = timeNow;

    /* Cap dt to avoid spiral of death after lag spikes */
    if (dt > 0.05f) dt = 0.05f;

    /* Update FPS counter every second */
    g_fpsTimer += (int)(dt * 1000.0f);
    if (g_fpsTimer >= 1000) {
        g_fps      = (float)g_frameCount;
        g_frameCount = 0;
        g_fpsTimer -= 1000;
    }

    /* Update simulation */
    if (!g_isPaused) {
        g_solarSystem->update(dt, g_speedMult);
    }

    /* Keep camera locked onto selected planet when in planet-focus mode */
    if (g_camera.currentView == VIEW_PLANET) {
        if (Planet* p = g_solarSystem->getSelectedPlanet()) {
            g_camera.focusPlanet(p->posX, p->posY, p->posZ, p->radius);
        }
    }

    g_camera.smoothUpdate(dt);
    glutPostRedisplay();
}

void keyboard(unsigned char key, int x, int y) {
    Input::handleKeyboard(key, x, y,
                          g_camera, *g_solarSystem,
                          g_isPaused, g_speedMult,
                          g_showOrbits, g_showLabels,
                          g_wireframeMode, g_texturesEnabled,
                          g_isFullscreen,
                          g_screenWidth, g_screenHeight);
}

void special(int key, int x, int y) {
    Input::handleSpecialKeys(key, x, y, g_camera);
}

void mouse(int button, int state, int x, int y) {
    Input::handleMouse(button, state, x, y,
                       g_camera, *g_solarSystem,
                       g_screenWidth, g_screenHeight);
}

void motion(int x, int y) {
    Input::handleMouseMotion(x, y, g_camera);
}

/* ─────────────────────────────────────────────────────────────────
   Entry Point
   ───────────────────────────────────────────────────────────────── */

int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH | GLUT_MULTISAMPLE);
    glutInitWindowSize(g_screenWidth, g_screenHeight);
    glutInitWindowPosition(80, 60);
    glutCreateWindow("3D Solar System Simulation  |  MCA Computer Graphics Project");

    /* Initialise OpenGL state and load resources */
    g_renderer.initOpenGL();
    g_solarSystem = new SolarSystem();
    g_lastTime    = glutGet(GLUT_ELAPSED_TIME);

    /* Set initial camera to a nice reference view and hard-snap (no initial tween) */
    g_camera.setViewPreset(VIEW_REFERENCE);
    g_camera.angleX   = g_camera.targetAngleX;
    g_camera.angleY   = g_camera.targetAngleY;
    g_camera.distance = g_camera.targetDistance;
    g_camera.focusX   = g_camera.targetFocusX;
    g_camera.focusY   = g_camera.targetFocusY;
    g_camera.focusZ   = g_camera.targetFocusZ;

    /* Register GLUT callbacks */
    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutIdleFunc(idle);
    glutKeyboardFunc(keyboard);
    glutSpecialFunc(special);
    glutMouseFunc(mouse);
    glutMotionFunc(motion);

    /* Enter render loop — never returns */
    glutMainLoop();

    delete g_solarSystem;
    return 0;
}
