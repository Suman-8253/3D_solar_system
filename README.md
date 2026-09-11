# Interactive 3D Solar System Simulation Using Geometric Transformations and Animation

## 1. Project Title
**Interactive 3D Solar System Simulation Using Geometric Transformations and Animation**

## 2. Introduction
This project is an interactive 3D Solar System simulation developed using C++ and OpenGL. It demonstrates the application of fundamental Computer Graphics concepts to create a real-time visualization of planets orbiting the Sun.

## 3. Problem Statement
To design and implement a 3D environment that visually simulates the solar system, providing an interactive way to observe planetary motion, while applying core computer graphics principles such as transformations, projection, and lighting.

## 4. Objectives
* To create an interactive 3D Solar System.
* To implement 3D geometric transformations.
* To demonstrate translation, rotation and scaling.
* To implement hierarchical transformations.
* To demonstrate perspective projection.
* To implement real-time animation.
* To implement camera/view transformations.
* To demonstrate basic lighting.
* To provide interactive controls.
* To visualize planetary motion in a simple educational environment.

## 5. Scope
The project covers the 8 major planets and Earth's moon. The distances and sizes are scaled for educational and visual purposes rather than strict astronomical accuracy.

## 6. Technologies Used
* **Language**: C++
* **Graphics API**: OpenGL
* **Utility Toolkit**: FreeGLUT (or GLUT)
* **Build System**: CMake

## 7. Computer Graphics Concepts Used
* **3D Modeling**: Utilizing `glutSolidSphere` and `glutSolidTorus` to construct planets and Saturn's rings.
* **Transformations**: `glTranslatef` for positioning, `glRotatef` for spinning and orbiting, `glScalef` for sizing.
* **Hierarchical Modeling**: Matrix stack operations (`glPushMatrix`, `glPopMatrix`) to create parent-child relationships (e.g., Earth and Moon).
* **Lighting & Shading**: Ambient, Diffuse, and Specular lighting with material properties.
* **Projection**: `gluPerspective` for 3D depth and `gluOrtho2D` for 2D UI overlay.
* **View/Camera**: `gluLookAt` for dynamic camera positioning.
* **Double Buffering**: To prevent screen tearing during animations.

## 8. System Architecture
The code is modularized into several classes:
* `main.cpp`: Entry point, GLUT initialization, window management, and input mapping.
* `SolarSystem`: Scene graph manager that creates and updates all celestial bodies.
* `Planet`: Encapsulates physical properties, rendering, and transformations of a single body.
* `Camera`: Manages view vectors, pitch, yaw, and zoom.
* `Renderer`: Handles OpenGL state initialization, lighting, background, and HUD.

## 9. Transformation Techniques
The core visualization relies on transformations applied in specific orders:
1. **Push Matrix**: Save current coordinate state.
2. **Orbit (Revolution)**: Rotate by orbit angle, then translate by orbit radius.
3. **Push Matrix**: Save planet's position for moons.
4. **Self-Rotation**: Rotate on local Y-axis for day/night cycle.
5. **Draw Object**: Render the sphere.
6. **Pop Matrix**: Restore to planet's position.
7. **Draw Moons**: Apply similar transformations relative to the planet.
8. **Pop Matrix**: Restore back to origin (Sun).

## 10. Mathematical Model
### Circular Orbit
Planetary motion is approximated as circular orbits:
* `x = R * cos(theta)`
* `z = R * sin(theta)`
(Implemented via sequential `glRotatef(theta)` and `glTranslatef(R, 0, 0)`)

### Planet Rotation
* `rotationAngle = rotationAngle + (rotationSpeed * timeStep)`

### Planet Revolution
* `orbitAngle = orbitAngle + (orbitSpeed * timeStep)`

## 11. Algorithm
1. Initialize OpenGL, Camera, and Solar System objects.
2. For each frame in the Display loop:
   a. Clear Color and Depth Buffers.
   b. Setup Projection and View Matrices.
   c. Draw background stars.
   d. Setup Lighting.
   e. For each Planet: Draw orbit paths, apply transformations, render planet, render moons.
   f. Switch to Orthographic projection and draw HUD.
   g. Swap Buffers.
3. In Idle loop, update angles based on elapsed time and request redisplay.

## 12. Animation Method
Animation is achieved using `glutIdleFunc`. We calculate the delta time since the last frame and use it to increment the `rotationAngle` and `orbitAngle` of each planet. This ensures frame-rate independent smooth motion.

## 13. Camera and Projection
* **Projection**: `gluPerspective(45.0, aspectRatio, 1.0, 500.0)` is used to give objects realistic size scaling based on distance.
* **Camera**: A spherical coordinate system converts yaw and pitch angles into Cartesian `posX`, `posY`, `posZ` for `gluLookAt`.

## 14. Lighting
A single directional light source (`GL_LIGHT0`) acts as the Sun. Planets use `glMaterialfv` to define how they react to Ambient, Diffuse, and Specular light. The Sun emits light via the `GL_EMISSION` material property.

## 15. Features
* 3D visualization of the Sun, 8 planets, and Moon.
* Independent revolution and rotation speeds.
* Hierarchical Moon orbit.
* Saturn's Rings.
* Starry background.
* Toggleable UI, lighting, and orbit paths.
* Smooth WASD and mouse-style keyboard camera controls.

## 16. Hardware Requirements
* **Processor**: 1.0 GHz or higher.
* **RAM**: 512 MB or higher.
* **Graphics**: Any GPU supporting OpenGL 2.1 or higher.

## 17. Software Requirements
* Windows / Linux / macOS.
* C++ Compiler (GCC / MSVC).
* CMake (Optional but recommended).
* FreeGLUT or GLUT development libraries.

## 18. Installation Steps
For Windows using MSYS2 / MinGW:
1. Install MSYS2.
2. Open MSYS2 UCRT64 terminal.
3. Install dependencies: `pacman -S mingw-w64-ucrt-x86_64-gcc mingw-w64-ucrt-x86_64-cmake mingw-w64-ucrt-x86_64-freeglut`

## 19. Compilation Steps
1. Navigate to the `SolarSystem` directory.
2. Create a build directory: `mkdir build && cd build`
3. Generate makefiles: `cmake .. -G "MinGW Makefiles"`
4. Build the executable: `cmake --build .`

*Alternative (without CMake):*
`g++ src/*.cpp -o SolarSystem.exe -lfreeglut -lopengl32 -lglu32`

## 20. Execution Steps
Run the generated executable:
`./SolarSystem.exe`

## 21. Expected Output
A window will open displaying the Sun at the center with 8 planets orbiting it. A starfield sits in the background. The HUD displays controls and selected planet information. Pressing Space will pause the simulation.

## 22. Limitations
* Orbits are perfectly circular, whereas real planetary orbits are elliptical.
* Sizes and distances are heavily scaled down; realistic scales would make planets invisible single pixels.
* No complex texture mapping is used (fallback colors used to ensure cross-platform compatibility out of the box).

## 23. Future Enhancements
* Implementation of elliptical orbits using Kepler's laws.
* Addition of high-resolution texture mapping via `stb_image.h`.
* Mouse picking to select planets directly from the 3D view.
* Skybox implementation for a more realistic space environment.

## 24. Conclusion
This project successfully demonstrates the core principles of Computer Graphics. By utilizing OpenGL's matrix stack, projection mathematics, and lighting pipeline, a complex and interactive 3D hierarchical simulation was developed. It serves as a robust foundation for understanding 3D transformations and real-time rendering.
