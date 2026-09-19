cmake_minimum_required(VERSION 3.10)
project(SolarSystemSimulation CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find OpenGL
find_package(OpenGL REQUIRED)

# Find GLUT (FreeGLUT)
find_package(GLUT REQUIRED)

# Include directories
include_directories(
    ${OPENGL_INCLUDE_DIR}
    ${GLUT_INCLUDE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/src
    ${CMAKE_CURRENT_SOURCE_DIR}/lib
)

# Source files
set(SOURCES
    src/main.cpp
    src/SolarSystem.cpp
    src/Planet.cpp
    src/AsteroidBelt.cpp
    src/Camera.cpp
    src/Renderer.cpp
    src/Lighting.cpp
    src/UI.cpp
    src/Input.cpp
    src/TextureManager.cpp
)

# Add executable
add_executable(${PROJECT_NAME} ${SOURCES})

# Link libraries
target_link_libraries(${PROJECT_NAME}
    ${OPENGL_LIBRARIES}
    ${GLUT_LIBRARIES}
)
