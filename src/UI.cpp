/*  ═══════════════════════════════════════════════════════════════
    UI.cpp  –  2-D heads-up display
    CG Concepts: Orthographic projection (gluOrtho2D), 2-D blending,
                 bitmap text (glutBitmapCharacter), raster position
    ═══════════════════════════════════════════════════════════════ */

#include "UI.h"
#include <cstdio>
#include <cstring>
#include <cmath>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

/* ── Helpers ──────────────────────────────────────────────────── */

void UI::drawText(float x, float y, const std::string& text, void* font) {
    if (!font) font = GLUT_BITMAP_HELVETICA_12;
    glRasterPos2f(x, y);
    for (char c : text)
        glutBitmapCharacter(font, (int)(unsigned char)c);
}

void UI::drawWrappedText(float x, float* py, float lineH,
                         const std::string& text, void* font, float maxPx) {
    std::string line, word;
    for (size_t i = 0; i <= text.length(); ++i) {
        char c = (i < text.length()) ? text[i] : '\0';
        if (c == ' ' || c == '\n' || c == '\0') {
            if (!word.empty()) {
                std::string test = line.empty() ? word : line + " " + word;
                float tw = (float)glutBitmapLength(font,
                               (const unsigned char*)test.c_str());
                if (tw > maxPx && !line.empty()) {
                    drawText(x, *py, line, font);
                    *py -= lineH;
                    line = word;
                } else {
                    line = test;
                }
                word = "";
            }
            if (c == '\n' && !line.empty()) {
                drawText(x, *py, line, font);
                *py -= lineH;
                line = "";
            }
        } else {
            word += c;
        }
    }
    if (!line.empty()) { drawText(x, *py, line, font); *py -= lineH; }
}

/* ── Panel primitives ─────────────────────────────────────────── */

static void fillRect(float x, float y, float w, float h,
                     float r, float g, float b, float a) {
    glColor4f(r, g, b, a);
    glBegin(GL_QUADS);
    glVertex2f(x,     y);
    glVertex2f(x + w, y);
    glVertex2f(x + w, y + h);
    glVertex2f(x,     y + h);
    glEnd();
}

static void strokeRect(float x, float y, float w, float h,
                       float r, float g, float b, float a, float lw = 1.0f) {
    glColor4f(r, g, b, a);
    glLineWidth(lw);
    glBegin(GL_LINE_LOOP);
    glVertex2f(x,     y);
    glVertex2f(x + w, y);
    glVertex2f(x + w, y + h);
    glVertex2f(x,     y + h);
    glEnd();
    glLineWidth(1.0f);
}

/* Glassmorphism-style panel: dark translucent fill + coloured border */
static void drawPanel(float x, float y, float w, float h,
                      float br, float bg, float bb, float alpha = 0.72f) {
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    /* Dark tinted background */
    fillRect(x, y, w, h, 0.04f, 0.06f, 0.10f, alpha);

    /* Top-edge highlight */
    glColor4f(br * 0.8f, bg * 0.8f, bb * 0.8f, 0.4f);
    glLineWidth(1.0f);
    glBegin(GL_LINES);
    glVertex2f(x + 2.0f, y + h);
    glVertex2f(x + w - 2.0f, y + h);
    glEnd();

    /* Border */
    strokeRect(x, y, w, h, br, bg, bb, 0.55f, 1.2f);

    glDisable(GL_BLEND);
}

/* Horizontal progress bar */
static void drawBar(float x, float y, float w, float h,
                    float value01,            /* 0..1 fill */
                    float fr, float fg, float fb) {
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    fillRect(x,  y, w,          h, 0.1f, 0.1f, 0.1f, 0.6f);
    fillRect(x,  y, w * value01, h, fr,   fg,   fb,   0.85f);
    strokeRect(x, y, w, h, 0.4f, 0.4f, 0.4f, 0.5f);
    glDisable(GL_BLEND);
}

/* ── Main HUD ─────────────────────────────────────────────────── */

void UI::drawHUD(int width, int height,
                 Planet* selectedPlanet,
                 const std::vector<Planet*>& allPlanets,
                 float speedMult, bool isPaused,
                 bool orbitsEnabled, bool labelsEnabled,
                 bool lightingEnabled, bool texturesEnabled,
                 bool wireframeEnabled,
                 ViewPreset viewPreset,
                 float fps)
{
    glDisable(GL_LIGHTING);
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_TEXTURE_2D);

    /* Switch to 2-D orthographic projection */
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    gluOrtho2D(0, width, 0, height);

    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();

    char buf[256];

    /* ════════════════════════════════════════════════════════════
       TOP-LEFT: Title panel
       ════════════════════════════════════════════════════════════ */
    {
        float W = 340.0f, H = 64.0f;
        float X = 16.0f,  Y = height - H - 16.0f;
        drawPanel(X, Y, W, H, 0.30f, 0.70f, 1.00f);

        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glColor3f(1.0f, 1.0f, 1.0f);
        drawText(X + 14.0f, Y + H - 23.0f,
                 "3D SOLAR SYSTEM SIMULATION", GLUT_BITMAP_HELVETICA_18);
        glColor3f(0.50f, 0.80f, 1.00f);
        drawText(X + 14.0f, Y + 10.0f,
                 "MCA Computer Graphics  |  OpenGL + FreeGLUT",
                 GLUT_BITMAP_HELVETICA_12);
        glDisable(GL_BLEND);
    }

    /* ════════════════════════════════════════════════════════════
       TOP-RIGHT: Controls & toggles
       ════════════════════════════════════════════════════════════ */
    {
        float W = 260.0f, H = 200.0f;
        float X = width - W - 16.0f, Y = height - H - 16.0f;
        drawPanel(X, Y, W, H, 0.30f, 0.70f, 1.00f);

        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        float ty = Y + H - 20.0f;
        glColor3f(1.0f, 0.80f, 0.20f);
        drawText(X + 14.0f, ty, "CONTROLS", GLUT_BITMAP_HELVETICA_12); ty -= 18.0f;

        glColor3f(0.75f, 0.75f, 0.75f);
        drawText(X + 14.0f, ty, "Drag  : Rotate camera");   ty -= 15.0f;
        drawText(X + 14.0f, ty, "Scroll: Zoom in / out");   ty -= 15.0f;
        drawText(X + 14.0f, ty, "RMB   : Pan view");        ty -= 15.0f;
        drawText(X + 14.0f, ty, "Click : Select planet");   ty -= 18.0f;

        /* Toggle indicators */
        auto toggle = [&](const char* label, bool on, float r1, float g1, float b1,
                                                       float r0, float g0, float b0) {
            glColor3f(on ? r1 : r0, on ? g1 : g0, on ? b1 : b0);
            drawText(X + 14.0f, ty, label);
            ty -= 15.0f;
        };
        toggle("[O] Orbits",   orbitsEnabled,  0.40f, 1.00f, 0.40f, 0.60f, 0.60f, 0.60f);
        toggle("[L] Labels",   labelsEnabled,  0.40f, 1.00f, 0.40f, 0.60f, 0.60f, 0.60f);
        toggle("[T] Textures", texturesEnabled,0.40f, 1.00f, 0.40f, 0.60f, 0.60f, 0.60f);
        toggle("[V] Wireframe",wireframeEnabled,0.60f, 0.60f, 0.60f,0.40f, 1.00f, 0.40f);

        /* FPS */
        snprintf(buf, sizeof(buf), "FPS: %.0f", fps);
        glColor3f(0.50f, 0.80f, 0.50f);
        drawText(X + 14.0f, ty, buf);

        glDisable(GL_BLEND);
    }

    /* ════════════════════════════════════════════════════════════
       BOTTOM BAR: planet selector + sim controls
       ════════════════════════════════════════════════════════════ */
    {
        float W = width - 32.0f, H = 68.0f;
        float X = 16.0f, Y = 12.0f;
        drawPanel(X, Y, W, H, 0.30f, 0.70f, 1.00f);

        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        /* Planet icons */
        float px = X + 12.0f;
        float iconY = Y + H * 0.5f - 12.0f;
        for (size_t i = 0; i < allPlanets.size(); ++i) {
            Planet* p = allPlanets[i];
            bool isSel = (p == selectedPlanet);

            /* Selection highlight box */
            if (isSel) {
                fillRect(px - 2.0f, iconY - 4.0f, 74.0f, 34.0f,
                         p->color[0] * 0.3f, p->color[1] * 0.3f, p->color[2] * 0.3f, 0.55f);
                strokeRect(px - 2.0f, iconY - 4.0f, 74.0f, 34.0f,
                           p->color[0], p->color[1], p->color[2], 0.8f, 1.5f);
            }

            /* Texture icon (small square) */
            if (p->textureID) {
                glEnable(GL_TEXTURE_2D);
                glBindTexture(GL_TEXTURE_2D, p->textureID);
                glColor4f(1, 1, 1, 1);
                float isz = 22.0f;
                glBegin(GL_QUADS);
                glTexCoord2f(0, 0); glVertex2f(px,       iconY);
                glTexCoord2f(1, 0); glVertex2f(px + isz, iconY);
                glTexCoord2f(1, 1); glVertex2f(px + isz, iconY + isz);
                glTexCoord2f(0, 1); glVertex2f(px,       iconY + isz);
                glEnd();
                glDisable(GL_TEXTURE_2D);
            } else {
                /* Coloured dot fallback */
                glColor4f(p->color[0], p->color[1], p->color[2], 1.0f);
                glPointSize(10.0f);
                glBegin(GL_POINTS);
                glVertex2f(px + 11.0f, iconY + 11.0f);
                glEnd();
                glPointSize(1.0f);
            }

            /* Planet name */
            glColor3f(isSel ? 1.0f : 0.75f,
                      isSel ? 1.0f : 0.75f,
                      isSel ? 1.0f : 0.75f);
            drawText(px + 26.0f, iconY + 6.0f, p->name, GLUT_BITMAP_HELVETICA_12);

            px += 78.0f;
        }

        /* Sim controls on the far right of the bottom bar */
        float cy = Y + H * 0.5f + 8.0f;
        float cx = width - 340.0f;

        /* Pause/Play */
        glColor3f(isPaused ? 1.0f : 0.40f,
                  isPaused ? 0.60f : 1.0f,
                  0.20f);
        snprintf(buf, sizeof(buf), "[Space] %s", isPaused ? "▶ PLAY" : "⏸ PAUSE");
        drawText(cx, cy, buf, GLUT_BITMAP_HELVETICA_12); cy -= 18.0f;

        /* Speed */
        glColor3f(0.80f, 0.80f, 0.80f);
        snprintf(buf, sizeof(buf), "[+/-] Speed: %.1fx", speedMult);
        drawText(cx, cy, buf, GLUT_BITMAP_HELVETICA_12);

        /* Speed bar */
        float barX = cx + 140.0f;
        float log_val = logf(speedMult / 0.1f) / logf(100.0f / 0.1f);
        if (log_val < 0.0f) log_val = 0.0f;
        if (log_val > 1.0f) log_val = 1.0f;
        drawBar(barX, cy - 1.0f, 80.0f, 10.0f, log_val, 0.30f, 0.80f, 1.00f);

        /* View label */
        cy -= 0.0f;
        const char* viewStr = "FREE";
        if      (viewPreset == VIEW_OVERVIEW) viewStr = "OVERVIEW";
        else if (viewPreset == VIEW_TOP)      viewStr = "TOP";
        else if (viewPreset == VIEW_SIDE)     viewStr = "SIDE";
        else if (viewPreset == VIEW_FRONT)    viewStr = "FRONT";
        else if (viewPreset == VIEW_PLANET)   viewStr = "FOCUS";
        glColor3f(0.50f, 0.80f, 1.00f);
        snprintf(buf, sizeof(buf), "Cam: %s  [1-5]", viewStr);
        drawText(cx + 240.0f, Y + H - 22.0f, buf, GLUT_BITMAP_HELVETICA_12);

        glDisable(GL_BLEND);
    }

    /* ════════════════════════════════════════════════════════════
       RIGHT PANEL: Selected planet information
       Show whenever a planet is selected (not just in planet-focus view)
       ════════════════════════════════════════════════════════════ */
    if (selectedPlanet) {
        float W = 270.0f, H = 380.0f;
        float X = width - W - 16.0f;
        float Y = 96.0f;

        /* Tint panel with planet's colour */
        float pr = selectedPlanet->color[0];
        float pg = selectedPlanet->color[1];
        float pb = selectedPlanet->color[2];
        drawPanel(X, Y, W, H, pr, pg, pb);

        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        float ry = Y + H - 22.0f;

        /* Planet name (large) */
        glColor3f(1.0f, 1.0f, 1.0f);
        drawText(X + 14.0f, ry,
                 selectedPlanet->name, GLUT_BITMAP_HELVETICA_18);

        /* Type badge */
        glColor3f(pr * 0.8f + 0.2f, pg * 0.8f + 0.2f, pb * 0.8f + 0.2f);
        if (!selectedPlanet->type.empty())
            drawText(X + 14.0f, ry - 16.0f,
                     selectedPlanet->type, GLUT_BITMAP_HELVETICA_12);

        /* Planet thumbnail in panel */
        if (selectedPlanet->textureID) {
            glEnable(GL_TEXTURE_2D);
            glBindTexture(GL_TEXTURE_2D, selectedPlanet->textureID);
            glColor4f(1, 1, 1, 1);
            float tsz = 72.0f;
            float tx  = X + W - tsz - 12.0f;
            float tty = Y + H - tsz - 32.0f;
            glBegin(GL_QUADS);
            glTexCoord2f(0, 0); glVertex2f(tx,       tty);
            glTexCoord2f(1, 0); glVertex2f(tx + tsz, tty);
            glTexCoord2f(1, 1); glVertex2f(tx + tsz, tty + tsz);
            glTexCoord2f(0, 1); glVertex2f(tx,       tty + tsz);
            glEnd();
            glDisable(GL_TEXTURE_2D);
        }

        ry -= 30.0f;

        /* Divider line */
        glColor4f(pr, pg, pb, 0.5f);
        glBegin(GL_LINES);
        glVertex2f(X + 10.0f, ry); glVertex2f(X + W - 10.0f, ry);
        glEnd();
        ry -= 14.0f;

        /* Info rows */
        auto infoRow = [&](const char* label, const std::string& val) {
            if (val.empty()) return;
            glColor3f(0.65f, 0.75f, 0.85f);
            drawText(X + 14.0f, ry, label, GLUT_BITMAP_HELVETICA_12);
            glColor3f(1.0f, 1.0f, 1.0f);
            drawText(X + 118.0f, ry, val, GLUT_BITMAP_HELVETICA_12);
            ry -= 15.0f;
        };

        infoRow("Diameter:",    selectedPlanet->diameter);
        infoRow("Distance:",    selectedPlanet->distFromSun);
        infoRow("Day Length:",  selectedPlanet->dayLength);
        infoRow("Year Length:", selectedPlanet->yearLength);
        infoRow("Avg Temp:",    selectedPlanet->avgTemp);
        infoRow("Gravity:",     selectedPlanet->gravityStr);
        infoRow("Moons:",       selectedPlanet->numMoonsStr);
        infoRow("Atmosphere:",  selectedPlanet->atmosphereStr);

        ry -= 6.0f;
        /* Divider */
        glColor4f(pr, pg, pb, 0.4f);
        glBegin(GL_LINES);
        glVertex2f(X + 10.0f, ry); glVertex2f(X + W - 10.0f, ry);
        glEnd();
        ry -= 14.0f;

        /* Fun fact */
        if (!selectedPlanet->funFact.empty()) {
            glColor3f(1.0f, 0.82f, 0.30f);
            drawText(X + 14.0f, ry, "Fun Fact", GLUT_BITMAP_HELVETICA_12);
            ry -= 13.0f;
            glColor3f(0.92f, 0.92f, 0.92f);
            drawWrappedText(X + 14.0f, &ry, 13.0f,
                            selectedPlanet->funFact,
                            GLUT_BITMAP_HELVETICA_12,
                            W - 28.0f);
        }

        glDisable(GL_BLEND);
    }

    /* ── Restore matrices ── */
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);
    glPopMatrix();

    glEnable(GL_DEPTH_TEST);
    if (true) glEnable(GL_LIGHTING); /* always re-enable; main controls it */
}

/* ── Planet Labels ────────────────────────────────────────────── */

void UI::drawPlanetLabels(int width, int height,
                          const std::vector<Planet*>& planets,
                          Camera* cam)
{
    glDisable(GL_LIGHTING);
    glDisable(GL_DEPTH_TEST);

    /* Build projection & modelview matrices matching the 3-D render pass */
    GLdouble proj[16], mv[16];
    GLint    vp[4];

    glGetIntegerv(GL_VIEWPORT, vp);

    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    gluPerspective(50.0, (double)width / (double)height, 0.5, 600.0);
    glGetDoublev(GL_PROJECTION_MATRIX, proj);
    glPopMatrix();

    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();
    gluLookAt(cam->posX, cam->posY, cam->posZ,
              cam->lookX, cam->lookY, cam->lookZ,
              cam->upX,   cam->upY,   cam->upZ);
    glGetDoublev(GL_MODELVIEW_MATRIX, mv);
    glPopMatrix();

    /* Switch to 2-D ortho for drawing */
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    gluOrtho2D(0, width, 0, height);

    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    for (const auto* p : planets) {
        GLdouble wx, wy, wz;
        if (gluProject(p->posX, p->posY, p->posZ,
                       mv, proj, vp, &wx, &wy, &wz) != GL_TRUE) continue;
        if (wz <= 0.0 || wz >= 1.0) continue; /* behind camera or clipped */

        float lx = (float)wx;
        float ly = (float)wy + 20.0f;

        /* Leader line */
        glColor4f(p->color[0], p->color[1], p->color[2], 0.55f);
        glBegin(GL_LINES);
        glVertex2f((float)wx, (float)wy + 4.0f);
        glVertex2f(lx, ly - 2.0f);
        glEnd();

        /* Label text */
        float tw = (float)glutBitmapLength(GLUT_BITMAP_HELVETICA_12,
                       (const unsigned char*)p->name.c_str());
        glColor4f(p->color[0] * 0.7f + 0.3f,
                  p->color[1] * 0.7f + 0.3f,
                  p->color[2] * 0.7f + 0.3f, 0.95f);
        drawText(lx - tw * 0.5f, ly + 3.0f, p->name, GLUT_BITMAP_HELVETICA_12);
    }

    glDisable(GL_BLEND);

    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);
    glPopMatrix();

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_LIGHTING);
}
