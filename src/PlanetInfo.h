#ifndef PLANETINFO_H
#define PLANETINFO_H

/* ============================================================
   PlanetInfo  –  rich descriptive data for each solar body
   ============================================================ */

typedef struct {
    char description[400];  /* Long description (word-wrapped at draw time) */
    char type[48];          /* "Rocky Planet", "Gas Giant", "Star" …        */
    char diameter[40];
    char distanceSun[40];
    char dayLength[48];
    char yearLength[48];
    char avgTemp[40];
    char gravity[40];
    char numMoons[24];
    char atmosphere[80];
    char funFact[180];
} PlanetInfo;

/* Fill *out with data for the body named 'name'.
   Returns 1 if found, 0 if unknown (placeholder data used). */
int PlanetInfo_get(const char* name, PlanetInfo* out);

#endif /* PLANETINFO_H */
