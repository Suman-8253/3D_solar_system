# ============================================================
#  run.ps1  —  Build & launch SolarSystemSimulation.exe
#  Usage: powershell -ExecutionPolicy Bypass -File .\run.ps1
# ============================================================

# Add compiler runtime DLLs to PATH
$env:PATH = "C:\ucrt64\bin;" + $env:PATH

Write-Host ""
Write-Host "============================================" -ForegroundColor Cyan
Write-Host "  3D Solar System Simulation" -ForegroundColor Cyan
Write-Host "  MCA Computer Graphics Project" -ForegroundColor Cyan
Write-Host "============================================" -ForegroundColor Cyan
Write-Host ""

# Step 1: Build
Write-Host "Building..." -ForegroundColor Yellow
.\build.bat
$buildResult = $LASTEXITCODE

if ($buildResult -ne 0) {
    Write-Host ""
    Write-Host "  BUILD FAILED (exit code: $buildResult)" -ForegroundColor Red
    Write-Host "  Fix compilation errors and try again." -ForegroundColor Red
    Write-Host "============================================" -ForegroundColor Cyan
    exit $buildResult
}

# Step 2: Verify EXE exists
$exePath = ".\build\SolarSystemSimulation.exe"
if (-not (Test-Path $exePath)) {
    Write-Host ""
    Write-Host "  ERROR: EXE not found at $exePath" -ForegroundColor Red
    Write-Host "  Build may have silently failed." -ForegroundColor Red
    exit 1
}

Write-Host ""
Write-Host "  BUILD SUCCESS" -ForegroundColor Green
Write-Host "  Launching $exePath ..." -ForegroundColor Green
Write-Host "============================================" -ForegroundColor Cyan
Write-Host ""

# Step 3: Launch the EXE
& $exePath
