#include "DetailView.h"
#include "Planet.h"
#include "PlanetInfo.h"
#include "Renderer.h"

#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

#include <math.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

/* ============================================================
   Lifecycle
   ============================================================ */

void DetailView_init(DetailView* dv, Planet* p) {
    dv->planet   = p;
    dv->rotAngle = 0.0f;
    dv->animTime = 0.0f;
    PlanetInfo_get(p->name, &dv->info);
}

void DetailView_update(DetailView* dv, float dt) {
    dv->rotAngle += 22.0f * dt;   /* ~22 deg/s slow spin */
    if (dv->rotAngle > 360.0f) dv->rotAngle -= 360.0f;
    dv->animTime += dt;
}

/* ============================================================
   Private – 3-D scene helpers
   ============================================================ */

/* Additive glow halo around any planet */
static void drawGlowHalo(Planet* p, float t) {
    float scales[] = { 1.22f, 1.55f, 2.05f, 2.80f };
    float alphas[] = { 0.16f, 0.10f, 0.06f, 0.03f };
    int   n = 4, i;
    float pulse = 0.70f + 0.30f * sinf(t * 1.8f);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE);   /* Additive */
    glDisable(GL_LIGHTING);
    glDisable(GL_CULL_FACE);

    for (i = 0; i < n; i++) {
        glColor4f(p->color[0] * 0.85f + 0.15f,
                  p->color[1] * 0.85f + 0.15f,
                  p->color[2] * 0.85f + 0.15f,
                  alphas[i] * pulse);
        glutSolidSphere(p->radius * scales[i], 24, 24);
    }

    glEnable(GL_CULL_FACE);
    glEnable(GL_LIGHTING);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glDisable(GL_BLEND);
}

/* Sun-specific corona (more dramatic, warm orange) */
static void drawSunCorona(Planet* p, float t) {
    float scales[] = { 1.18f, 1.45f, 1.95f, 2.70f, 3.60f };
    float alphas[] = { 0.28f, 0.18f, 0.10f, 0.05f, 0.02f };
    int   n = 5, i;
    float pulse = 0.80f + 0.20f * sinf(t * 2.2f);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE);
    glDisable(GL_LIGHTING);
    glDisable(GL_CULL_FACE);

    for (i = 0; i < n; i++) {
        glColor4f(1.0f,
                  0.62f - (float)i * 0.06f,
                  0.01f + (float)i * 0.01f,
                  alphas[i] * pulse);
        glutSolidSphere(p->radius * scales[i], 28, 28);
    }

    glEnable(GL_CULL_FACE);
    glEnable(GL_LIGHTING);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glDisable(GL_BLEND);
}

/* Material setup for the detail sphere */
static void setDetailMaterial(Planet* p) {
    int     isSun = (strcmp(p->name, "Sun") == 0);
    GLfloat amb[4], diff[4], spec[4], emis[4], shine;

    diff[0] = p->color[0]; diff[1] = p->color[1];
    diff[2] = p->color[2]; diff[3] = 1.0f;

    amb[0]  = p->color[0] * 0.18f; amb[1] = p->color[1] * 0.18f;
    amb[2]  = p->color[2] * 0.18f; amb[3] = 1.0f;

    spec[0] = spec[1] = spec[2] = 0.45f; spec[3] = 1.0f;

    emis[0] = emis[1] = emis[2] = emis[3] = 0.0f;
    shine = 35.0f;

    if (isSun) {
        emis[0] = p->color[0] * 0.95f;
        emis[1] = p->color[1] * 0.75f;
        emis[2] = 0.0f;
        emis[3] = 1.0f;
        amb[0] = p->color[0];
        amb[1] = p->color[1];
        amb[2] = p->color[2];
        shine = 0.0f;
    }

    glMaterialfv(GL_FRONT, GL_AMBIENT,   amb);
    glMaterialfv(GL_FRONT, GL_DIFFUSE,   diff);
    glMaterialfv(GL_FRONT, GL_SPECULAR,  spec);
    glMaterialfv(GL_FRONT, GL_EMISSION,  emis);
    glMaterialf (GL_FRONT, GL_SHININESS, shine);
    glColor3f(p->color[0], p->color[1], p->color[2]);
}

/* Faint equatorial ring drawn in the planet's local space */
static void drawEquatorialRing(float radius, float r, float g, float b) {
    int   segs = 72;
    int   i;
    float rr   = radius + 0.03f;

    glDisable(GL_LIGHTING);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glColor4f(r < 1.0f ? r + 0.3f : 1.0f,
              g < 1.0f ? g + 0.3f : 1.0f,
              b < 1.0f ? b + 0.3f : 1.0f,
              0.35f);
    glLineWidth(1.2f);
    glBegin(GL_LINE_LOOP);
    for (i = 0; i < segs; i++) {
        float theta = 2.0f * (float)M_PI * (float)i / (float)segs;
        glVertex3f(rr * cosf(theta), 0.0f, rr * sinf(theta));
    }
    glEnd();
    glLineWidth(1.0f);
    glEnable(GL_LIGHTING);
    glDisable(GL_BLEND);
}

/* ============================================================
   Private – 2-D panel helpers
   ============================================================ */

/* Word-wrap a string and draw lines starting at (x, *py). */
static void drawWrappedText(Renderer* r,
                            float x, float* py, float lineH,
                            const char* text, void* font, float maxPx)
{
    char        line[512] = "";
    char        word[128];
    const char* src = text;
    int         wi;

    for (;;) {
        wi = 0;
        while (*src && *src != ' ' && *src != '\n')
            if (wi < 126) word[wi++] = *src++;
            else { src++; }
        word[wi] = '\0';

        if (wi > 0) {
            char test[640];
            if (strlen(line) > 0)
                snprintf(test, sizeof(test), "%s %s", line, word);
            else
                snprintf(test, sizeof(test), "%s", word);

            float tw = (float)glutBitmapLength(font, (const unsigned char*)test);
            if (tw > maxPx && strlen(line) > 0) {
                Renderer_drawText(r, x, *py, line, font);
                *py -= lineH;
                strncpy(line, word, sizeof(line)-1);
                line[sizeof(line)-1] = '\0';
            } else {
                strncpy(line, test, sizeof(line)-1);
                line[sizeof(line)-1] = '\0';
            }
        }

        if (*src == '\0') break;
        if (*src == '\n') {
            if (strlen(line) > 0) {
                Renderer_drawText(r, x, *py, line, font);
                *py -= lineH;
                line[0] = '\0';
            }
        }
        if (*src) src++;
    }

    if (strlen(line) > 0) {
        Renderer_drawText(r, x, *py, line, font);
        *py -= lineH;
    }
}

static void drawHLine(float x1, float x2, float y, float ri, float gi, float bi, float a) {
    glColor4f(ri, gi, bi, a);
    glLineWidth(1.0f);
    glBegin(GL_LINES);
    glVertex2f(x1, y);
    glVertex2f(x2, y);
    glEnd();
}

static void drawStatRow(Renderer* r, float lx, float vx, float y,
                        const char* label, const char* value,
                        float lr, float lg, float lb)
{
    glColor3f(lr, lg, lb);
    Renderer_drawText(r, lx, y, label, GLUT_BITMAP_HELVETICA_12);
    glColor3f(0.93f, 0.93f, 0.93f);
    Renderer_drawText(r, vx, y, value, GLUT_BITMAP_HELVETICA_12);
}

static void drawSectionHeader(Renderer* r, float x, float y, const char* title) {
    glColor3f(1.0f, 0.85f, 0.22f);
    Renderer_drawText(r, x, y, title, GLUT_BITMAP_HELVETICA_12);
}

/* ============================================================
   Main entry point
   ============================================================ */

void DetailView_draw(DetailView* dv, int width, int height,
                     bool lightingEnabled, Renderer* renderer)
{
    Planet* p     = dv->planet;
    int     isSun = (strcmp(p->name, "Sun") == 0);
    float   pulse = 0.5f + 0.5f * sinf(dv->animTime * 1.6f);

    /* Layout: 62 % 3-D scene | 38 % info panel */
    int   sceneW = (int)((float)width * 0.62f);
    float panelX = (float)sceneW;
    float panelW = (float)(width - sceneW);

    /* Accent colour from planet base colour, brightened if too dark */
    float ar = p->color[0] < 0.30f ? p->color[0] + 0.40f : p->color[0];
    float ag = p->color[1] < 0.30f ? p->color[1] + 0.40f : p->color[1];
    float ab = p->color[2] < 0.30f ? p->color[2] + 0.50f : p->color[2];
    if (ar > 1.0f) ar = 1.0f;
    if (ag > 1.0f) ag = 1.0f;
    if (ab > 1.0f) ab = 1.0f;

    /* ═══════════════════════════════════════════════════════
       1.  3-D PLANET SCENE  (left portion)
       ═══════════════════════════════════════════════════════ */

    glEnable(GL_SCISSOR_TEST);
    glScissor(0, 0, sceneW, height);
    glViewport(0, 0, sceneW, height);

    glClearColor(0.0f, 0.0f, 0.025f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    /* Perspective */
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(45.0, (double)sceneW / (double)height, 0.05, 500.0);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    /* Slowly orbiting camera */
    {
        float camT    = dv->animTime * 0.16f;
        float dist    = p->radius * 4.8f;
        if (dist < 2.8f)  dist = 2.8f;
        if (dist > 14.0f) dist = 14.0f;
        gluLookAt(dist * sinf(camT),  dist * 0.28f,  dist * cosf(camT),
                  0.0f, 0.0f, 0.0f,
                  0.0f, 1.0f, 0.0f);
    }

    /* Lighting – sun at upper-left */
    {
        GLfloat lpos[]  = { -8.0f, 10.0f, 6.0f, 1.0f };
        GLfloat ldiff[] = {  1.0f,  0.97f, 0.88f, 1.0f };
        GLfloat lamb[]  = {  0.07f, 0.07f, 0.10f, 1.0f };
        GLfloat lspec[] = {  1.0f,  1.0f,  1.0f,  1.0f };
        GLfloat gAmb[]  = {  0.03f, 0.03f, 0.05f, 1.0f };

        glEnable(GL_LIGHTING);
        glEnable(GL_LIGHT0);
        glLightfv(GL_LIGHT0, GL_POSITION, lpos);
        glLightfv(GL_LIGHT0, GL_DIFFUSE,  ldiff);
        glLightfv(GL_LIGHT0, GL_AMBIENT,  lamb);
        glLightfv(GL_LIGHT0, GL_SPECULAR, lspec);
        glLightModelfv(GL_LIGHT_MODEL_AMBIENT, gAmb);
        glLightModeli(GL_LIGHT_MODEL_LOCAL_VIEWER, GL_TRUE);
        glEnable(GL_COLOR_MATERIAL);
        glColorMaterial(GL_FRONT, GL_AMBIENT_AND_DIFFUSE);
        glEnable(GL_NORMALIZE);
    }

    /* Star background */
    Renderer_drawStarBackground(renderer);

    /* Glow halo (drawn before sphere so it sits behind) */
    if (isSun)
        drawSunCorona(p, dv->animTime);
    else
        drawGlowHalo(p, dv->animTime);

    /* ── Planet sphere ── */
    glEnable(GL_DEPTH_TEST);
    glPushMatrix();
    glRotatef(p->axialTilt, 0.0f, 0.0f, 1.0f);  /* Axial tilt */
    glRotatef(dv->rotAngle, 0.0f, 1.0f, 0.0f);  /* Self-rotation */
    setDetailMaterial(p);
    glutSolidSphere(p->radius, 64, 64);
    /* Equatorial highlight ring */
    if (!isSun)
        drawEquatorialRing(p->radius, p->color[0], p->color[1], p->color[2]);
    glPopMatrix();

    /* ── Atmosphere ── */
    if (p->hasAtmosphere)
        Planet_drawAtmosphere(p);

    /* ── Rings ── */
    if (p->hasRings && p->ringInnerRadius > 0.0f)
        Planet_drawRings(p);

    glDisable(GL_SCISSOR_TEST);

    /* ═══════════════════════════════════════════════════════
       2.  INFO PANEL  (right portion, 2-D ortho)
       ═══════════════════════════════════════════════════════ */

    glViewport(0, 0, width, height);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(0, width, 0, height);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    glDisable(GL_DEPTH_TEST);
    glDisable(GL_LIGHTING);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    /* ── Panel gradient background ── */
    glBegin(GL_QUADS);
    glColor4f(0.04f, 0.07f, 0.18f, 0.88f);   /* left edge (brighter) */
    glVertex2f(panelX,           0.0f);
    glVertex2f(panelX,           (float)height);
    glColor4f(0.01f, 0.02f, 0.09f, 0.93f);   /* right edge (darker)  */
    glVertex2f((float)width,     (float)height);
    glVertex2f((float)width,     0.0f);
    glEnd();

    /* ── Glowing border line (planet-accent colour) ── */
    {
        float bAlpha = 0.50f + 0.30f * pulse;
        /* Outer glow */
        glColor4f(ar, ag, ab, bAlpha * 0.35f);
        glLineWidth(5.0f);
        glBegin(GL_LINES);
        glVertex2f(panelX, 0.0f); glVertex2f(panelX, (float)height);
        glEnd();
        /* Inner crisp line */
        glColor4f(ar, ag, ab, bAlpha);
        glLineWidth(1.8f);
        glBegin(GL_LINES);
        glVertex2f(panelX + 1.0f, 0.0f); glVertex2f(panelX + 1.0f, (float)height);
        glEnd();
        glLineWidth(1.0f);
    }

    /* ── Text layout setup ── */
    float margin = panelX + 20.0f;
    float rEdge  = (float)width  - 14.0f;
    float maxW   = rEdge - margin;
    float ty     = (float)height - 32.0f;

    /* ── Planet name ── */
    {
        char title[64];
        snprintf(title, sizeof(title), "  %s  ", p->name);
        float nw  = (float)glutBitmapLength(GLUT_BITMAP_HELVETICA_18,
                                             (const unsigned char*)title);
        float nx  = panelX + (panelW - nw) * 0.5f;

        /* Pill background */
        glColor4f(ar*0.25f, ag*0.25f, ab*0.25f, 0.85f);
        glBegin(GL_QUADS);
        glVertex2f(nx - 6.0f,      ty - 5.0f);
        glVertex2f(nx + nw + 6.0f, ty - 5.0f);
        glVertex2f(nx + nw + 6.0f, ty + 22.0f);
        glVertex2f(nx - 6.0f,      ty + 22.0f);
        glEnd();
        /* Pill border */
        glColor4f(ar, ag, ab, 0.55f + 0.2f * pulse);
        glBegin(GL_LINE_LOOP);
        glVertex2f(nx - 6.0f,      ty - 5.0f);
        glVertex2f(nx + nw + 6.0f, ty - 5.0f);
        glVertex2f(nx + nw + 6.0f, ty + 22.0f);
        glVertex2f(nx - 6.0f,      ty + 22.0f);
        glEnd();

        glColor3f(ar, ag, ab);
        Renderer_drawText(renderer, nx, ty, title, GLUT_BITMAP_HELVETICA_18);
    }
    ty -= 10.0f;

    /* ── Type tag (centered) ── */
    {
        float tw = (float)glutBitmapLength(GLUT_BITMAP_HELVETICA_12,
                                            (const unsigned char*)dv->info.type);
        float tx = panelX + (panelW - tw) * 0.5f;
        ty -= 16.0f;
        glColor3f(0.50f, 0.80f, 0.50f);
        Renderer_drawText(renderer, tx, ty, dv->info.type, GLUT_BITMAP_HELVETICA_12);
    }

    ty -= 12.0f;
    drawHLine(margin, rEdge, ty, ar, ag, ab, 0.55f);
    ty -= 16.0f;

    /* ── Description ── */
    glColor3f(0.82f, 0.86f, 0.90f);
    drawWrappedText(renderer, margin, &ty, 15.0f,
                    dv->info.description, GLUT_BITMAP_HELVETICA_12, maxW);

    ty -= 10.0f;
    drawHLine(margin, rEdge, ty, ar*0.65f, ag*0.65f, ab*0.65f, 0.45f);
    ty -= 18.0f;

    /* ── Planetary Data ── */
    drawSectionHeader(renderer, margin, ty, "  PLANETARY DATA");
    ty -= 17.0f;

    float lx = margin + 8.0f;
    float vx = margin + 118.0f;

    drawStatRow(renderer, lx, vx, ty, "Diameter  :", dv->info.diameter,    0.55f, 0.75f, 0.95f); ty -= 16.0f;
    drawStatRow(renderer, lx, vx, ty, "Distance  :", dv->info.distanceSun, 0.55f, 0.75f, 0.95f); ty -= 16.0f;
    drawStatRow(renderer, lx, vx, ty, "Day Length:", dv->info.dayLength,   0.55f, 0.75f, 0.95f); ty -= 16.0f;
    drawStatRow(renderer, lx, vx, ty, "Year      :", dv->info.yearLength,  0.55f, 0.75f, 0.95f); ty -= 16.0f;
    drawStatRow(renderer, lx, vx, ty, "Avg Temp  :", dv->info.avgTemp,     0.55f, 0.75f, 0.95f); ty -= 16.0f;
    drawStatRow(renderer, lx, vx, ty, "Gravity   :", dv->info.gravity,     0.55f, 0.75f, 0.95f); ty -= 16.0f;
    drawStatRow(renderer, lx, vx, ty, "Moons     :", dv->info.numMoons,    0.55f, 0.75f, 0.95f); ty -= 16.0f;

    ty -= 8.0f;
    drawHLine(margin, rEdge, ty, ar*0.55f, ag*0.55f, ab*0.55f, 0.40f);
    ty -= 18.0f;

    /* ── Atmosphere ── */
    drawSectionHeader(renderer, margin, ty, "  ATMOSPHERE");
    ty -= 15.0f;
    glColor3f(0.75f, 0.85f, 0.75f);
    drawWrappedText(renderer, lx, &ty, 14.0f,
                    dv->info.atmosphere, GLUT_BITMAP_HELVETICA_12, maxW - 8.0f);

    ty -= 8.0f;
    drawHLine(margin, rEdge, ty, ar*0.50f, ag*0.50f, ab*0.50f, 0.38f);
    ty -= 18.0f;

    /* ── Did you know? ── */
    drawSectionHeader(renderer, margin, ty, "  DID YOU KNOW?");
    ty -= 15.0f;
    glColor3f(0.78f, 0.84f, 0.72f);
    drawWrappedText(renderer, lx, &ty, 14.0f,
                    dv->info.funFact, GLUT_BITMAP_HELVETICA_12, maxW - 8.0f);

    /* ── ESC hint (pinned to bottom centre) ── */
    {
        const char* hint = "[ ESC ]  Return to Solar System";
        float hw     = (float)glutBitmapLength(GLUT_BITMAP_HELVETICA_12,
                                                (const unsigned char*)hint);
        float hx     = panelX + (panelW - hw) * 0.5f;
        float hAlpha = 0.55f + 0.35f * sinf(dv->animTime * 2.2f);

        /* Subtle hint background */
        glColor4f(ar*0.15f, ag*0.15f, ab*0.15f, 0.70f);
        glBegin(GL_QUADS);
        glVertex2f(hx - 8.0f,      12.0f);
        glVertex2f(hx + hw + 8.0f, 12.0f);
        glVertex2f(hx + hw + 8.0f, 28.0f);
        glVertex2f(hx - 8.0f,      28.0f);
        glEnd();

        glColor4f(ar, ag, ab, hAlpha);
        Renderer_drawText(renderer, hx, 15.0f, hint, GLUT_BITMAP_HELVETICA_12);
    }

    glDisable(GL_BLEND);
    glEnable(GL_DEPTH_TEST);
    if (lightingEnabled) glEnable(GL_LIGHTING);

    /* Restore full viewport for the next frame */
    glViewport(0, 0, width, height);
}
