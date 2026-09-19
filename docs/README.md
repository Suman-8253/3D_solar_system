# 3D Solar System Simulation

A professional desktop 3D simulation of our Solar System, built from scratch using C++, OpenGL, and FreeGLUT. This project serves as an educational tool and a comprehensive demonstration of core Computer Graphics concepts.

## Features
- **Accurate Planetary Hierarchy**: Sun, 8 Planets, Pluto, Earth's Moon, and a procedurally generated Asteroid Belt.
- **Rich Visuals**: Texture mapping, Saturn's rings, planetary atmospheres, and Sun's emissive corona glow.
- **Dynamic Camera System**: Spherical orbit camera with smooth interpolation (lerping), right-click panning, scroll zooming, and 5 view presets.
- **Professional UI/HUD**: Glassmorphism-style 2D overlay with a bottom-bar planet selector, real-time data panels, and state toggles.
- **Physics Simulation**: Delta-time based animation for orbital revolution and axial rotation.
- **Data-Driven**: Loads planetary facts dynamically from `data/planets.dat`.

## Tech Stack
- **Language**: C++17
- **Graphics API**: OpenGL (Fixed Function Pipeline) + GLU
- **Windowing**: FreeGLUT
- **Image Loading**: stb_image (single-header C library)

## Computer Graphics Concepts Demonstrated
- 3D Coordinate Systems & Projections (`gluPerspective`, `gluOrtho2D`)
- Hierarchical Transformations (Matrix Stack, `glPushMatrix`, `glPopMatrix`)
- Translation, Rotation, Scaling
- Camera Viewing Transformations (`gluLookAt`)
- Ambient, Diffuse, and Specular Lighting (`glLightfv`, `glMaterialfv`)
- Texture Mapping & Mipmaps
- Alpha Blending (Additive and Normal)
- Depth Buffering (`GL_DEPTH_TEST`)
- Custom Geometry Generation (`GL_TRIANGLE_STRIP` for rings)
- Procedural Generation (Asteroid Belt & Starfield)

## Building
Run `build.bat` on Windows using MSYS2/MinGW GCC.
Alternatively, use the provided `CMakeLists.txt`.

## Running
Run `run.ps1` to build and launch the executable.
