# 3D Solar System

An interactive 3D solar-system simulation written in C with OpenGL and FreeGLUT. It demonstrates hierarchical modeling, matrix transformations, lighting, perspective projection, animation, camera controls, and planet selection.

## Features

- Sun, eight planets, Earth's Moon, and Saturn's rings
- Animated rotation and circular planetary orbits
- Hierarchical planet and Moon transformations
- Starfield background and OpenGL lighting
- Free camera orbit, zoom, and pitch controls
- Click a planet to open a detailed information view
- HUD with selected-planet data and simulation state
- Adjustable animation speed, orbit visibility, and lighting

The sizes and distances are intentionally scaled for visibility and are not to scale with the real solar system. Orbits are circular approximations.

## Project Layout

```text
src/       C source and header files
lib/       Bundled third-party headers
textures/  Reserved for texture assets
run.ps1    Build and run with the configured MSYS2 UCRT64 toolchain
build.bat  Windows batch build script
CMakeLists.txt
```

## Requirements

- Windows, Linux, or macOS
- OpenGL and GLU development libraries
- FreeGLUT development libraries
- GCC or another C compiler
- CMake 3.10 or newer when using the CMake workflow

### Windows with MSYS2 UCRT64

Install MSYS2, then run the following in the UCRT64 terminal:

```bash
pacman -S --needed mingw-w64-ucrt-x86_64-gcc \
  mingw-w64-ucrt-x86_64-cmake \
  mingw-w64-ucrt-x86_64-freeglut
```

The included `run.ps1` and `build.bat` currently expect the UCRT64 installation at `D:\ucrt64`. Change that path in the script if MSYS2 is installed elsewhere.

## Build and Run

### PowerShell on the configured Windows toolchain

From the repository root:

```powershell
powershell -ExecutionPolicy Bypass -File .\run.ps1
```

This compiles all application sources, creates `SolarSystem.exe`, and launches it. The executable and other generated build output are local artifacts and should not be committed.

### CMake

From the repository root:

```bash
cmake -S . -B build
cmake --build build
```

Run the generated `SolarSystemSimulation` executable from the `build` directory. On Windows, make sure the FreeGLUT DLL directory is on `PATH` when launching it.

## Controls

| Input | Action |
| --- | --- |
| `W` / `S` | Zoom in / out |
| `A` / `D` | Orbit camera left / right |
| `Q` / `E` | Tilt camera up / down |
| `R` | Reset the camera to the full solar-system view |
| Arrow keys | Adjust camera pitch and yaw |
| Left mouse drag | Orbit the camera |
| Mouse wheel | Zoom in / out |
| Left click on a planet | Open its detail view |
| `0` | Return focus to the full solar system |
| `1` - `8` | Focus Mercury through Neptune |
| `Space` / `P` | Pause or resume animation |
| `+` / `-` | Increase or decrease animation speed |
| `O` | Show or hide orbit paths |
| `L` | Enable or disable lighting |
| `Esc` | Leave detail view, or exit the application |

## Graphics Concepts

- OpenGL fixed-function lighting and materials
- Perspective and orthographic projections
- `glPushMatrix` / `glPopMatrix` hierarchical transformations
- Translation, rotation, and scaling for celestial bodies
- Delta-time animation for frame-rate-independent motion
- Screen-space planet picking with `gluProject`

## Limitations and Future Work

- Planetary scales, distances, and speeds are educational approximations.
- Orbits are circular rather than elliptical.
- Texture mapping and a skybox are not currently enabled.
- Possible future additions include elliptical orbits, richer textures, and expanded interaction in the detail view.

## License

No license file is currently included in this repository.