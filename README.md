# Requirements

- Windows, Linux, or macOS
- Git
- CMake 3.15 or later
- A C++ toolchain for your platform
- Internet connection (for first-time build)

# Description

A 2D platform game engine with:

- (static/dynamic/sloped) tile and sprite collision resolution
- Keyboard/mouse/joystick support for game and menus

A map editor to edit:

- 3 layers:
  - background
  - foreground
  - collisions (optional)

# Building the Project

## Quick Start (Windows)

1. Clone the repository:
   ```bash
   git clone https://github.com/yourusername/2d-engine.git
   cd 2d-engine
   ```

2. Run the build script:
   ```bash
   build.bat
   ```

The script will automatically:
- Install vcpkg if not present
- Set up the build environment
- Build the project in Release mode

The executable will be available in `build/engine/Release/`.

## Quick Start (Linux/macOS)

```bash
./build.sh
```

The executable will be available in `build/engine/`.

## Manual Build

If you prefer to build manually:

1. Install vcpkg:
   ```bash
   git clone https://github.com/microsoft/vcpkg.git
   cd vcpkg
   # Windows:
   ./bootstrap-vcpkg.bat
   # Linux/macOS:
   ./bootstrap-vcpkg.sh
   ```

2. Set the VCPKG_ROOT environment variable to your vcpkg installation path

3. Create and enter a build directory:
   ```bash
   mkdir build
   cd build
   ```

4. Configure and build:
   ```bash
   cmake -S . -B build -DCMAKE_TOOLCHAIN_FILE="$VCPKG_ROOT/scripts/buildsystems/vcpkg.cmake" -DCMAKE_BUILD_TYPE=Release
   cmake --build build --config Release
   ```

# Runtime Resources

- Runtime assets live under `resources/`.
- The executable searches in this order:
  - `ENGINE_RESOURCE_ROOT`
  - executable-adjacent `resources/`
  - repo-local `resources/`
  - legacy relative paths

# Controls

## Game
- **Movement:** Joy hat, `W/A/S/D`
- **Run:** Joy button 2, `M`
- **Jump/Swim:** Joy button 0, `N`
- **Attack:** Joy button 3, `Y`
- **Unlock camera:** `T`
- **Move camera:** `Up/Left/Down/Right`
- **Menu toggle:** Joy button 7, `Space`, RightClick
- **Menu select:** Joy button 0, `Return`, LeftClick
- **Quit:** `Escape`, joy button 6
- **Fullscreen:** `F1`
- **Decrease size:** `J`
- **Increase size:** `K`
- **Slow/Fast:** Joy button 4 / joy button 5
- **Layers toggle:** `1-7`
- **Debug:** `F2`
- **Show collisions:** `F3`
- **Show hit boxes:** `F4`
- **Doublebuffer:** `F6`
- **Bilinear and scan lines:** `F7`
- **Curvature:** `F8`
- **Adjust curvature:** `8 / 9`

## Editor
- **Move camera:** `W/A/S/D`, RightDown drag
- **Select:** LeftClick
- **Paint:** LeftControl + LeftClick
- **Edit front:** `M`
- **Edit back:** `N`
- **Edit collisions:** `C`
- **Edit image:** `V`
- **Draw front:** `1`
- **Draw back:** `2`
- **Draw collisions:** `3`
- **Fullscreen:** `F1`
- **Decrease size:** `J`
- **Increase size:** `K`
- **Doublebuffer:** `F6`
- **Quit:** `Escape`, joy button 6
