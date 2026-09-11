#include "PlanetInfo.h"
#include <string.h>

/* ============================================================
   Internal helper – copy all fields in one call
   ============================================================ */

static void fill(PlanetInfo* o,
                 const char* desc,  const char* type,
                 const char* diam,  const char* dist,
                 const char* day,   const char* year,
                 const char* temp,  const char* grav,
                 const char* moons, const char* atm,
                 const char* fact)
{
#define CP(dst, src)  strncpy(o->dst, src, sizeof(o->dst)-1); \
                      o->dst[sizeof(o->dst)-1] = '\0'
    CP(description, desc);
    CP(type,        type);
    CP(diameter,    diam);
    CP(distanceSun, dist);
    CP(dayLength,   day);
    CP(yearLength,  year);
    CP(avgTemp,     temp);
    CP(gravity,     grav);
    CP(numMoons,    moons);
    CP(atmosphere,  atm);
    CP(funFact,     fact);
#undef CP
}

/* ============================================================
   Public API
   ============================================================ */

int PlanetInfo_get(const char* name, PlanetInfo* out)
{
    memset(out, 0, sizeof(*out));

    /* ── Sun ──────────────────────────────────────────────── */
    if (strcmp(name, "Sun") == 0) {
        fill(out,
             "The star at the heart of our Solar System. Formed 4.6 billion years ago, it contains 99.86% of the total mass of the Solar System and will continue to burn for another 5 billion years.",
             "Yellow Dwarf Star  (G-type)",
             "1,392,700 km",
             "0 AU  (center)",
             "25 Earth days (equator)",
             "N/A",
             "5,778 K  (surface)",
             "28.0 g",
             "8 orbiting planets",
             "Hydrogen 73%,  Helium 25%,  traces of O, C, Ne",
             "Every second, the Sun fuses 600 million tonnes of hydrogen into helium — releasing energy equivalent to 9 × 10^10 megatons of TNT.");
        return 1;
    }

    /* ── Mercury ─────────────────────────────────────────── */
    if (strcmp(name, "Mercury") == 0) {
        fill(out,
             "The smallest planet and closest to the Sun. With virtually no atmosphere to retain heat, surface temperatures swing from -180 C at night to 430 C by day. Ancient impact craters cover its rocky surface.",
             "Rocky Planet  (Terrestrial)",
             "4,879 km",
             "0.39 AU",
             "58.6 Earth days",
             "88 Earth days",
             "-30 C  (mean)",
             "0.38 g",
             "0",
             "Trace exosphere — O, Na, H, He",
             "Mercury has a disproportionately large iron core — its core makes up roughly 85% of the planet's total radius.");
        return 1;
    }

    /* ── Venus ───────────────────────────────────────────── */
    if (strcmp(name, "Venus") == 0) {
        fill(out,
             "The hottest planet despite not being the closest to the Sun. A thick CO2 atmosphere traps heat in a runaway greenhouse effect. Venus rotates backwards, so the Sun rises in the west.",
             "Rocky Planet  (Terrestrial)",
             "12,104 km",
             "0.72 AU",
             "243 Earth days  (retrograde)",
             "225 Earth days",
             "465 C  (mean)",
             "0.91 g",
             "0",
             "CO2 96.5%,  N2 3.5%,  SO2 traces",
             "A day on Venus is longer than its year — it takes longer to spin once than to orbit the Sun.");
        return 1;
    }

    /* ── Earth ───────────────────────────────────────────── */
    if (strcmp(name, "Earth") == 0) {
        fill(out,
             "Our home world — the only known planet with liquid water on the surface and complex life. A protective magnetic field and ozone layer shield the surface from harmful solar radiation.",
             "Rocky Planet  (Terrestrial)",
             "12,742 km",
             "1.00 AU",
             "23 h  56 m",
             "365.25 days",
             "+15 C  (mean)",
             "1.00 g",
             "1  (Luna / Moon)",
             "N2 78%,  O2 21%,  Ar 1%,  CO2 0.04%",
             "Earth is the densest planet in the Solar System and the only one not named after a Greco-Roman deity.");
        return 1;
    }

    /* ── Mars ────────────────────────────────────────────── */
    if (strcmp(name, "Mars") == 0) {
        fill(out,
             "The Red Planet gets its colour from iron oxide dust on its surface. It hosts Olympus Mons — the tallest volcano in the Solar System — and Valles Marineris, a canyon longer than the USA.",
             "Rocky Planet  (Terrestrial)",
             "6,779 km",
             "1.52 AU",
             "24 h  37 m",
             "687 Earth days",
             "-65 C  (mean)",
             "0.38 g",
             "2  (Phobos, Deimos)",
             "CO2 95%,  N2 2.6%,  Ar 1.9%",
             "Olympus Mons on Mars is about 21 km tall — nearly three times the height of Mount Everest above sea level.");
        return 1;
    }

    /* ── Jupiter ─────────────────────────────────────────── */
    if (strcmp(name, "Jupiter") == 0) {
        fill(out,
             "The largest planet — so massive that all other planets could fit inside it with room to spare. Its iconic Great Red Spot is a storm larger than Earth that has raged for over 350 years.",
             "Gas Giant",
             "139,820 km",
             "5.20 AU",
             "9 h  56 m",
             "11.86 years",
             "-110 C  (cloud tops)",
             "2.53 g",
             "95 known",
             "H2 89%,  He 10%,  CH4, NH3, HD traces",
             "Jupiter acts as a cosmic shield: its immense gravity deflects many comets and asteroids that would otherwise threaten the inner Solar System.");
        return 1;
    }

    /* ── Saturn ──────────────────────────────────────────── */
    if (strcmp(name, "Saturn") == 0) {
        fill(out,
             "Famous for its breathtaking ring system made of billions of ice and rock particles. Saturn is the least dense planet in the Solar System — it would float on water if an ocean large enough existed.",
             "Gas Giant  (Ringed)",
             "116,460 km",
             "9.58 AU",
             "10 h  33 m",
             "29.5 years",
             "-140 C  (cloud tops)",
             "1.07 g",
             "146 known",
             "H2 96.3%,  He 3.3%,  CH4 0.4%",
             "Saturn's rings span up to 282,000 km in width yet are often only 10 to 100 metres thick — thinner than a sheet of paper at that scale.");
        return 1;
    }

    /* ── Uranus ───────────────────────────────────────────── */
    if (strcmp(name, "Uranus") == 0) {
        fill(out,
             "An ice giant that orbits completely on its side with an axial tilt of 97.8 degrees. Each pole experiences 42 years of continuous sunlight followed by 42 years of total darkness. It has 13 known rings.",
             "Ice Giant",
             "50,724 km",
             "19.22 AU",
             "17 h  14 m  (retrograde)",
             "84 years",
             "-195 C  (mean)",
             "0.89 g",
             "27 known",
             "H2 83%,  He 15%,  CH4 2.3%",
             "Uranus was the first planet discovered with a telescope, identified by William Herschel on 13 March 1781.");
        return 1;
    }

    /* ── Neptune ─────────────────────────────────────────── */
    if (strcmp(name, "Neptune") == 0) {
        fill(out,
             "The windiest planet — supersonic storms rage at over 2,100 km/h. It was the first planet located through mathematical prediction rather than direct observation. Its largest moon Triton orbits retrograde.",
             "Ice Giant",
             "49,244 km",
             "30.07 AU",
             "16 h  6 m",
             "164.8 years",
             "-200 C  (mean)",
             "1.14 g",
             "16 known",
             "H2 80%,  He 19%,  CH4 1.5%",
             "Neptune was predicted mathematically from irregularities in Uranus's orbit and confirmed by telescope on 23 September 1846.");
        return 1;
    }

    /* Unknown body */
    fill(out,
         "No detailed information is available for this celestial body.",
         "Unknown",
         "Unknown","Unknown","Unknown","Unknown",
         "Unknown","Unknown","Unknown","Unknown","Unknown");
    return 0;
}
