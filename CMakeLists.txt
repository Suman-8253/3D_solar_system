cmake_minimum_required(VERSION 3.10)
project(SolarSystemSimulation C)


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
    src/main.c
    src/SolarSystem.c
    src/Planet.c
    src/Camera.c
    src/Renderer.c
    src/PlanetInfo.c
    src/DetailView.c
)

# Add executable
add_executable(${PROJECT_NAME} ${SOURCES})

# Link libraries
target_link_libraries(${PROJECT_NAME}
    ${OPENGL_LIBRARIES}
    ${GLUT_LIBRARIES}
)
