@echo off
:: ─────────────────────────────────────────────────────────────────────────────
:: build.bat — Build the GameEngine using MinGW-w64 (MSYS2)
:: Usage:
::   build.bat         → Debug build
::   build.bat release → Release build
::   build.bat clean   → Clean build directory
:: ─────────────────────────────────────────────────────────────────────────────
setlocal

set MSYS2_ROOT=C:\msys64
set MINGW_BIN=%MSYS2_ROOT%\mingw64\bin
set BUILD_DIR=build
set BUILD_TYPE=Debug

if /I "%1"=="release" set BUILD_TYPE=Release
if /I "%1"=="clean" (
    echo Cleaning build directory...
    if exist %BUILD_DIR% rmdir /s /q %BUILD_DIR%
    echo Done.
    exit /b 0
)

:: Add MinGW to PATH
set PATH=%MINGW_BIN%;%PATH%

:: Verify compiler
where gcc >nul 2>&1
if errorlevel 1 (
    echo ERROR: gcc not found. Make sure MSYS2 MinGW-w64 is installed.
    echo Run: C:\msys64\usr\bin\bash.exe -lc "pacman -S mingw-w64-x86_64-gcc mingw-w64-x86_64-cmake mingw-w64-x86_64-ninja"
    exit /b 1
)

for /f "tokens=*" %%i in ('gcc --version 2^>^&1 ^| findstr "gcc"') do echo Compiler: %%i
for /f "tokens=*" %%i in ('cmake --version 2^>^&1 ^| findstr "cmake version"') do echo CMake: %%i

:: Configure
echo.
echo === Configuring (%BUILD_TYPE%) ===
cmake -S . -B %BUILD_DIR% ^
    -G "Ninja" ^
    -DCMAKE_BUILD_TYPE=%BUILD_TYPE% ^
    -DCMAKE_C_COMPILER="%MINGW_BIN%\gcc.exe" ^
    -DCMAKE_CXX_COMPILER="%MINGW_BIN%\g++.exe" ^
    -DCMAKE_MAKE_PROGRAM="%MINGW_BIN%\ninja.exe"

if errorlevel 1 (
    echo ERROR: CMake configuration failed
    exit /b 1
)

:: Build
echo.
echo === Building ===
cmake --build %BUILD_DIR% --config %BUILD_TYPE% -j%NUMBER_OF_PROCESSORS%

if errorlevel 1 (
    echo ERROR: Build failed
    exit /b 1
)

echo.
echo === Build successful ===
echo Executables in: %BUILD_DIR%\bin\
dir /b %BUILD_DIR%\bin\*.exe 2>nul

endlocal
