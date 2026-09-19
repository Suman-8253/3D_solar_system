# Architecture Overview

The simulation is built using a clean, object-oriented C++ architecture, separating the rendering pipeline, input handling, UI, and data management.

## Core Classes

### 1. `SolarSystem` (Manager)
- Orchestrates the simulation.
- Holds instances of `Planet` (and its moons) and the `AsteroidBelt`.
- Responsible for parsing `planets.dat` and assigning textures via `TextureManager`.

### 2. `Planet` (Entity)
- Represents any celestial body (Sun, Earth, Moon).
- Contains rendering logic for the body sphere, rings, atmosphere, and sun glow.
- Uses **Hierarchical Transformations**: Pushes a matrix, applies orbital rotation, translates to orbit radius, applies axial tilt and self-rotation, draws the body, and iterates over its child moons.

### 3. `Camera` (Viewing)
- Maintains spherical coordinates (`angleX`, `angleY`, `distance`) and a focus point (`focusX`, `focusY`, `focusZ`).
- Uses Exponential Decay Interpolation (Lerp) for smooth camera movements between targets.
- Supports Free Orbit mode and Planet Focus mode.

### 4. `UI` (2D Overlay)
- Renders the HUD using orthographic projection (`gluOrtho2D`).
- Draws semi-transparent panels and bitmap fonts.
- Does not affect the 3D depth buffer.

### 5. `Renderer` & `Lighting`
- `Renderer`: Manages the OpenGL state machine (depth, blending, smoothing) and renders the 3000-point procedurally generated starfield.
- `Lighting`: Configures `GL_LIGHT0` at the origin (the Sun) and sets global ambient parameters.

## Rendering Pipeline
1. `glClear` (Color and Depth)
2. `gluPerspective` + `gluLookAt` (Camera setup)
3. `Renderer::drawStarBackground()` (No Lighting, No Depth Test)
4. `Lighting::setup(true)`
5. `SolarSystem::draw()` (Draws all 3D bodies)
6. `gluOrtho2D` (Switch to 2D)
7. `UI::drawHUD()` (Draws 2D overlay without Lighting/Depth)
8. `glutSwapBuffers()` (Double Buffering)
