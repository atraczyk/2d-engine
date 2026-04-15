@echo off
setlocal enabledelayedexpansion

:: Check if vcpkg is installed
if not defined VCPKG_ROOT (
    echo VCPKG_ROOT is not set. Installing vcpkg...
    if not exist "vcpkg" (
        git clone https://github.com/Microsoft/vcpkg.git
        cd vcpkg
        call bootstrap-vcpkg.bat
        cd ..
    ) else (
        cd vcpkg
        git pull
        call bootstrap-vcpkg.bat
        cd ..
    )
    set "VCPKG_ROOT=%CD%\vcpkg"
)

:: Create and enter build directory
if not exist "build" mkdir build
cd build

:: Configure with CMake
:: Change the generator below if you use a different Visual Studio version
echo Configuring project with CMake...
cmake .. -G "Visual Studio 17 2022" -A x64 -DCMAKE_TOOLCHAIN_FILE="%VCPKG_ROOT%\scripts\buildsystems\vcpkg.cmake" -DCMAKE_BUILD_TYPE=Release
if %errorlevel% neq 0 (
    echo CMake configuration failed.
    goto end
)

:: Build
echo Building project...
cmake --build . --config Release
if %errorlevel% neq 0 (
    echo Build failed.
    goto end
)

echo.
echo Build completed successfully! The executable should be in build\engine\Release\ (or build\engine\Debug\ if you built in Debug mode)
echo.

:end