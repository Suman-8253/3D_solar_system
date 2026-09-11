@echo off
set PATH=D:\ucrt64\bin;%PATH%

D:\ucrt64\bin\gcc.exe ^
  src\main.c ^
  src\Planet.c ^
  src\Renderer.c ^
  src\SolarSystem.c ^
  src\Camera.c ^
  -o SolarSystem.exe ^
  -Isrc -Ilib ^
  -ID:\ucrt64\include ^
  -LD:\ucrt64\lib ^
  -lfreeglut -lopengl32 -lglu32 ^
  -mwindows

if %ERRORLEVEL%==0 (
    echo.
    echo *** BUILD SUCCESS ***
    echo Running SolarSystem.exe...
    echo.
    start SolarSystem.exe
) else (
    echo.
    echo *** BUILD FAILED ***
)
