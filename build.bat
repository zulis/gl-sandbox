@echo off
setlocal

REM --- Configuration ---
set BUILD_DIR=build
set SOURCE_DIR=src

echo.
echo ==================================================
echo Starting Build Process
echo ==================================================
echo.

REM 1. Clean up previous build artifacts
echo [STEP 1/3] Cleaning previous build directory (%BUILD_DIR%)...
if exist "%BUILD_DIR%" (
    rmdir /s /q "%BUILD_DIR%"
)
mkdir "%BUILD_DIR%"

REM 2. Configure the build system (e.g., CMake)
REM NOTE: This assumes you are using CMake. Adjust this section if you use a different generator (like Meson, etc.)
echo [STEP 2/3] Configuring the build system using CMake...
cmake -S "%SOURCE_DIR%" -B "%BUILD_DIR%"

REM 3. Build the project
echo [STEP 3/3] Compiling the project using Make...
cmake --build "%BUILD_DIR%"

echo.
echo ==================================================
echo Build process completed successfully!
echo The executable(s) should be located in the "%BUILD_DIR%" directory.
echo ==================================================

endlocal
pause