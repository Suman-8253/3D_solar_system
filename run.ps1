# ============================================================
#  run.ps1 - Build & Run SolarSystem from PowerShell terminal
# ============================================================

$GCC      = "D:\ucrt64\bin\gcc.exe"
$SrcFiles = @(
    "src\main.c",
    "src\Planet.c",
    "src\Renderer.c",
    "src\SolarSystem.c",
    "src\Camera.c",
    "src\PlanetInfo.c",
    "src\DetailView.c"
)
$Output   = "SolarSystem.exe"
$Flags    = @(
    "-Isrc", "-Ilib",
    "-ID:\ucrt64\include",
    "-LD:\ucrt64\lib",
    "-lfreeglut", "-lopengl32", "-lglu32",
    "-mwindows"
)

Write-Host ""
Write-Host "============================================" -ForegroundColor Cyan
Write-Host "  Building Solar System Project..." -ForegroundColor Cyan
Write-Host "============================================" -ForegroundColor Cyan
Write-Host ""

# Add ucrt64 to PATH so DLLs can be found at runtime
$env:PATH = "D:\ucrt64\bin;" + $env:PATH

# Run gcc
$args = $SrcFiles + @("-o", $Output) + $Flags
& $GCC @args 2>&1

if ($LASTEXITCODE -eq 0) {
    Write-Host ""
    Write-Host "  BUILD SUCCESS" -ForegroundColor Green
    Write-Host "  Launching SolarSystem.exe ..." -ForegroundColor Green
    Write-Host "============================================" -ForegroundColor Cyan
    Write-Host ""
    # Run directly (not via 'start') so output stays in this terminal
    & ".\$Output"
} else {
    Write-Host ""
    Write-Host "  BUILD FAILED  (exit code: $LASTEXITCODE)" -ForegroundColor Red
    Write-Host "============================================" -ForegroundColor Cyan
    Write-Host ""
    exit $LASTEXITCODE
}
