#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif
#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>
#include <math.h>

#include "SolarSystem.h"
#include "Camera.h"
#include "Renderer.h"
#include "DetailView.h"

/* ============================================================
   Application state
   ============================================================ */

typedef enum {
    APP_SOLAR_SYSTEM = 0,   /* Normal solar-system view */
    APP_PLANET_DETAIL = 1   /* Isolated planet + info panel */
} AppState;

/* ============================================================
   Globals
   ============================================================ */

static SolarSystem* solarSystem  = NULL;
static Camera*      camera       = NULL;
static Renderer*    renderer     = NULL;

static AppState     appState     = APP_SOLAR_SYSTEM;
static DetailView   detailView;

static int   windowWidth  = 1280;
static int   windowHeight = 720;
static bool  isPaused     = false;
static float speedMultiplier = 1.0f;

/* Toggles */
static bool showOrbits      = true;
static bool lightingEnabled = true;

/* Time tracking */
static int lastTime = 0;

/* ── Planet screen-space positions (updated each solar-system frame) ── */
static float planetScreenX[MAX_PLANETS];
static float planetScreenY[MAX_PLANETS];
static int   numPlanetScreenPos = 0;

/* ── Mouse click tracking (to distinguish click vs drag) ── */
static int mouseDownX = 0;
static int mouseDownY = 0;

/* ============================================================
   Focus helper
   ============================================================ */

static int pendingFocusIndex = -1;

static void applyPendingFocus(void) {
    if (pendingFocusIndex < 0) return;

    if (pendingFocusIndex == 0) {
        Camera_focusSolarSystem(camera);
    } else {
        float px, py, pz;
        Planet* p = SolarSystem_getPlanet(solarSystem, pendingFocusIndex);
        SolarSystem_getPlanetWorldPos(solarSystem, pendingFocusIndex,
                                      &px, &py, &pz);
        if (p) Camera_focusPlanet(camera, px, py, pz, p->radius);
    }
    pendingFocusIndex = -1;
}

/* ============================================================
   Planet picking helpers
   ============================================================ */

/* Called after SolarSystem_draw() to project every planet's
   eye-space position into window coordinates for later picking.
   planet->posX/Y/Z holds eye-space position (set in Planet_draw). */
static void updatePlanetScreenPositions(void) {
    /* Identity modelview: treat posX/Y/Z as eye-space */
    GLdouble identity[16] = {
        1,0,0,0,
        0,1,0,0,
        0,0,1,0,
        0,0,0,1
    };
    GLdouble projMat[16];
    GLint    viewport[4];
    int      i;

    glGetDoublev(GL_PROJECTION_MATRIX, projMat);
    glGetIntegerv(GL_VIEWPORT, viewport);

    numPlanetScreenPos = solarSystem->numPlanets;
    for (i = 0; i < solarSystem->numPlanets; i++) {
        Planet*  p = solarSystem->planets[i];
        GLdouble winX, winY, winZ;
        if (gluProject((GLdouble)p->posX, (GLdouble)p->posY, (GLdouble)p->posZ,
                       identity, projMat, viewport,
                       &winX, &winY, &winZ) == GL_TRUE
            && winZ > 0.0 && winZ < 1.0)
        {
            planetScreenX[i] = (float)winX;
            planetScreenY[i] = (float)winY;  /* OpenGL Y: bottom-up */
        } else {
            planetScreenX[i] = -9999.0f;
            planetScreenY[i] = -9999.0f;
        }
    }
}

/* Try to pick a planet at window click (x, y).
   Returns 1 and enters detail mode if a planet is hit. */
static int tryPickPlanet(int x, int y) {
    int   i, picked = -1;
    float bestDist  = 1e9f;
    int   glY       = windowHeight - y;  /* Flip to OpenGL bottom-up Y */

    for (i = 0; i < numPlanetScreenPos; i++) {
        float dx   = (float)x - planetScreenX[i];
        float dy   = (float)glY - planetScreenY[i];
        float dist = sqrtf(dx*dx + dy*dy);

        /* Click radius: fixed floor + scales with planet visual size */
        float threshold = 22.0f + solarSystem->planets[i]->radius * 14.0f;
        if (dist < threshold && dist < bestDist) {
            bestDist = dist;
            picked   = i;
        }
    }

    if (picked >= 0) {
        DetailView_init(&detailView, solarSystem->planets[picked]);
        appState = APP_PLANET_DETAIL;
        return 1;
    }
    return 0;
}

/* ============================================================
   Display callback
   ============================================================ */

void display(void) {
    /* ── Detail mode ── */
    if (appState == APP_PLANET_DETAIL) {
        DetailView_draw(&detailView, windowWidth, windowHeight,
                        lightingEnabled, renderer);
        glutSwapBuffers();
        return;
    }

    /* ── Normal solar-system view ── */
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    Renderer_setPerspective(renderer, windowWidth, windowHeight);
    glLoadIdentity();
    Camera_updateView(camera);

    Renderer_setupLighting(renderer, lightingEnabled);
    Renderer_drawStarBackground(renderer);
    SolarSystem_draw(solarSystem, showOrbits, false);

    /* Capture planet screen positions for picking in mouseButton */
    updatePlanetScreenPositions();

    Renderer_drawHUD(renderer, windowWidth, windowHeight,
                     SolarSystem_getSelectedPlanet(solarSystem),
                     speedMultiplier, isPaused,
                     showOrbits, lightingEnabled, false,
                     (int)camera->mode);

    glutSwapBuffers();
}

/* ============================================================
   Reshape callback
   ============================================================ */

void reshape(int w, int h) {
    if (h == 0) h = 1;
    windowWidth  = w;
    windowHeight = h;
    glViewport(0, 0, w, h);
}

/* ============================================================
   Idle callback
   ============================================================ */

void idle(void) {
    int   currentTime = glutGet(GLUT_ELAPSED_TIME);
    float deltaTime   = (currentTime - lastTime) / 1000.0f;
    lastTime = currentTime;

    if (deltaTime > 0.1f) deltaTime = 0.1f;

    Camera_smoothUpdate(camera, deltaTime);
    applyPendingFocus();

    /* Update detail view animation when in detail mode */
    if (appState == APP_PLANET_DETAIL)
        DetailView_update(&detailView, deltaTime);

    /* Keep solar system running in background (planets move in real time) */
    if (!isPaused)
        SolarSystem_update(solarSystem, deltaTime, speedMultiplier);

    glutPostRedisplay();
}

/* ============================================================
   Keyboard callback
   ============================================================ */

void keyboard(unsigned char key, int x, int y) {
    switch (key) {

        case 27:   /* ESC */
            if (appState == APP_PLANET_DETAIL) {
                /* Return to solar system view */
                appState = APP_SOLAR_SYSTEM;
                glViewport(0, 0, windowWidth, windowHeight);
            } else {
                exit(0);
            }
            break;

        case ' ':
        case 'p': case 'P':
            isPaused = !isPaused;
            break;

        case '+': case '=':
            speedMultiplier += 0.2f;
            if (speedMultiplier > 10.0f) speedMultiplier = 10.0f;
            break;

        case '_': case '-':
            speedMultiplier -= 0.2f;
            if (speedMultiplier < 0.1f) speedMultiplier = 0.1f;
            break;

        case 'o': case 'O':
            showOrbits = !showOrbits;
            break;

        case 'l': case 'L':
            lightingEnabled = !lightingEnabled;
            break;

        /* Planet number shortcuts */
        case '0':
            Camera_focusSolarSystem(camera);
            SolarSystem_setSelectedPlanetIndex(solarSystem, 0);
            break;
        case '1':
            SolarSystem_setSelectedPlanetIndex(solarSystem, 1);
            pendingFocusIndex = 1;
            break;
        case '2':
            SolarSystem_setSelectedPlanetIndex(solarSystem, 2);
            pendingFocusIndex = 2;
            break;
        case '3':
            SolarSystem_setSelectedPlanetIndex(solarSystem, 3);
            pendingFocusIndex = 3;
            break;
        case '4':
            SolarSystem_setSelectedPlanetIndex(solarSystem, 4);
            pendingFocusIndex = 4;
            break;
        case '5':
            SolarSystem_setSelectedPlanetIndex(solarSystem, 5);
            pendingFocusIndex = 5;
            break;
        case '6':
            SolarSystem_setSelectedPlanetIndex(solarSystem, 6);
            pendingFocusIndex = 6;
            break;
        case '7':
            SolarSystem_setSelectedPlanetIndex(solarSystem, 7);
            pendingFocusIndex = 7;
            break;
        case '8':
            SolarSystem_setSelectedPlanetIndex(solarSystem, 8);
            pendingFocusIndex = 8;
            break;

        default:
            Camera_processKeyboard(camera, key, x, y);
            break;
    }
}

/* ============================================================
   Special-key callback (arrow keys)
   ============================================================ */

void specialKeys(int key, int x, int y) {
    Camera_processSpecialKeys(camera, key, x, y);
}

/* ============================================================
   Mouse callbacks
   ============================================================ */

void mouseButton(int button, int state, int x, int y) {
    /* In detail mode we don't need picking; just handle scroll for zoom
       and ignore left-button entirely (ESC is the only exit) */
    if (appState == APP_PLANET_DETAIL) {
        /* Allow scroll-wheel zoom even in detail mode (future use) */
        return;
    }

    /* Track mouse-down position to distinguish click vs drag */
    if (button == GLUT_LEFT_BUTTON) {
        if (state == GLUT_DOWN) {
            mouseDownX = x;
            mouseDownY = y;
        } else if (state == GLUT_UP) {
            int dx = x - mouseDownX;
            int dy = y - mouseDownY;
            /* If cursor moved < 6 pixels it is a click, not a drag */
            if (dx*dx + dy*dy <= 36) {
                if (tryPickPlanet(x, y))
                    return;   /* Picked a planet → enter detail mode */
            }
        }
    }

    Camera_processMouse(camera, button, state, x, y);
}

void mouseMotion(int x, int y) {
    if (appState == APP_PLANET_DETAIL) return;
    Camera_processMouseMotion(camera, x, y);
}

/* ============================================================
   Entry point
   ============================================================ */

int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(windowWidth, windowHeight);
    glutInitWindowPosition(80, 60);
    glutCreateWindow("Solar System Explorer  -  Interactive 3D Simulation");

    renderer    = (Renderer*)malloc(sizeof(Renderer));
    camera      = (Camera*)malloc(sizeof(Camera));
    solarSystem = (SolarSystem*)malloc(sizeof(SolarSystem));

    Renderer_init(renderer);
    Renderer_initOpenGL(renderer);
    Camera_init(camera);
    SolarSystem_init(solarSystem);

    lastTime = glutGet(GLUT_ELAPSED_TIME);

    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutIdleFunc(idle);
    glutKeyboardFunc(keyboard);
    glutSpecialFunc(specialKeys);
    glutMouseFunc(mouseButton);
    glutMotionFunc(mouseMotion);

    glutMainLoop();

    SolarSystem_destroy(solarSystem);
    free(solarSystem);
    Renderer_destroy(renderer);
    free(renderer);
    free(camera);
    return 0;
}
