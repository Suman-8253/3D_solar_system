#ifndef DETAILVIEW_H
#define DETAILVIEW_H

/* ============================================================
   DetailView  –  isolated 3-D planet scene + info panel
   ============================================================ */

#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

#include "Planet.h"
#include "PlanetInfo.h"
#include "Renderer.h"
#include <stdbool.h>

typedef struct {
    float      rotAngle;   /* Planet auto-rotation angle (degrees)   */
    float      animTime;   /* Seconds elapsed since entering mode     */
    Planet*    planet;     /* Borrowed pointer – do not free here     */
    PlanetInfo info;       /* Cached fact-sheet for this body         */
} DetailView;

void DetailView_init  (DetailView* dv, Planet* p);
void DetailView_update(DetailView* dv, float dt);
void DetailView_draw  (DetailView* dv, int width, int height,
                       bool lightingEnabled, Renderer* renderer);

#endif /* DETAILVIEW_H */
