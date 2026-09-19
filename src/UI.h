#ifndef UI_H
#define UI_H

/*  ═══════════════════════════════════════════════════════════════
    UI  –  2-D heads-up display rendered in orthographic projection
    CG Concepts: Orthographic projection, bitmap text, 2-D blending
    ═══════════════════════════════════════════════════════════════ */

#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

#include <string>
#include <vector>

#include "Planet.h"
#include "Camera.h"

class UI {
public:
    /* Draw 2-D HUD (call after all 3-D rendering is done) */
    static void drawHUD(int width, int height,
                        Planet* selectedPlanet,
                        const std::vector<Planet*>& allPlanets,
                        float speedMult, bool isPaused,
                        bool orbitsEnabled, bool labelsEnabled,
                        bool lightingEnabled, bool texturesEnabled,
                        bool wireframeEnabled,
                        ViewPreset viewPreset,
                        float fps);

    /* Project planet 3-D world positions to screen and draw name labels */
    static void drawPlanetLabels(int width, int height,
                                 const std::vector<Planet*>& planets,
                                 Camera* cam);

private:
    /* Helpers */
    static void drawText(float x, float y, const std::string& text,
                         void* font = nullptr);
    static void drawWrappedText(float x, float* py, float lineH,
                                const std::string& text,
                                void* font, float maxPx);
};

#endif /* UI_H */
