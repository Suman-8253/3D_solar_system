@echo off
setlocal
set PATH=C:\ucrt64\bin;%PATH%

echo Building DLL...

if not exist build mkdir build

C:\ucrt64\bin\g++.exe -shared ^
  src\main.cpp ^
  src\Planet.cpp ^
  src\Renderer.cpp ^
  src\SolarSystem.cpp ^
  src\Camera.cpp ^
  src\AsteroidBelt.cpp ^
  src\Lighting.cpp ^
  src\UI.cpp ^
  src\Input.cpp ^
  src\TextureManager.cpp ^
  -o build\SolarSystemSimulation.dll ^
  -Isrc -Ilib ^
  -IC:\ucrt64\include ^
  -LC:\ucrt64\lib ^
  -lfreeglut -lopengl32 -lglu32 ^
  -fno-lto

if %ERRORLEVEL%==0 (
    echo *** DLL BUILD SUCCESS ***
    exit /b 0
) else (
    echo *** DLL BUILD FAILED ***
    exit /b %ERRORLEVEL%
)
