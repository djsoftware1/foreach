@rem https://github.com/djsoftware1/foreach
@rem Created 2 Jan 2026 by David Joffe

@echo off
setlocal

REM Simple build helper for foreach (Windows)
REM Requires: CMake and a C++ compiler (MSVC, clang, or mingw)

if "%BUILD_DIR%"=="" set BUILD_DIR=build-windows
if "%BUILD_TYPE%"=="" set BUILD_TYPE=Release

echo ==> Building foreach
echo     build dir : %BUILD_DIR%
echo     build type: %BUILD_TYPE%

cmake -S . -B %BUILD_DIR% -DCMAKE_BUILD_TYPE=%BUILD_TYPE%
if errorlevel 1 exit /b 1

cmake --build %BUILD_DIR% --config %BUILD_TYPE%
if errorlevel 1 exit /b 1

endlocal
