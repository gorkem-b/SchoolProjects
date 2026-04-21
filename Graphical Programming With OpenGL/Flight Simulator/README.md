# Flight Simulator

An OpenGL 3.3 / C++17 flight-simulator-style course project featuring a heightmap terrain, skybox, aircraft model, HUD, and a third-person chase camera.

The current version focuses on a playable third-person flight experience rather than a full study-level simulator. The aircraft uses a simplified semi-realistic flight model with persistent engine thrust, pitch/bank controls, terrain wrapping, and HUD telemetry.

## Features

- Heightmap-based terrain rendering
- Procedural terrain fallback
- Cubemap skybox
- Third-person chase camera
- Aircraft model rendering
- HUD overlay with flight telemetry
- Semi-realistic flight model
  - constant engine thrust
  - lift / drag / gravity
  - AoA-based behavior
  - bank-driven turning behavior
- Normal-mapped terrain shading
- Fog and directional lighting
- Texture mipmapping and anisotropic filtering
- sRGB-aware texture workflow

## Controls

```text
W        : Pitch nose down
S        : Pitch nose up
A        : Bank / turn left
D        : Bank / turn right
P        : Pause / resume simulation
I        : Toggle terrain wireframe
R        : Reset aircraft position
ESC      : Exit
```

## Runtime Notes

- The program starts in fullscreen mode by default.
- Engine thrust is enabled by default and is not adjusted during gameplay.
- The world is designed to continue by wrapping at terrain boundaries instead of stopping at a hard map edge.

## Tech Stack

- **Language:** C++17
- **Graphics API:** OpenGL 3.3 Core Profile
- **Windowing / Input:** GLFW
- **OpenGL Loader:** GLAD
- **Math:** GLM
- **Image Loading:** SOIL
- **Build System:** CMake

## Build Instructions

### Configure

```bash
cmake -S . -B build
```

### Build

```bash
cmake --build build
```

### Run

```text
build/FlightSimGL.exe
```

> Note: The CMake build copies the `res/` directory next to the executable after build.

## Dependencies

This project uses:

- GLFW
- GLM
- GLAD
- SOIL

GLFW, GLM, and GLAD are fetched during CMake configure. SOIL is included in the repository under `vendor/`.

## Project Structure

```text
Flight Simulator/
├── CMakeLists.txt
├── README.md
├── GUIDE.md
├── res/
│   ├── shaders/
│   ├── textures/
│   └── models/
├── src/
│   ├── main.cpp
│   ├── Config/
│   ├── Core/
│   ├── Graphics/
│   └── Simulation/
└── vendor/
```

### Source Modules

- **Core**
  - `Window` for GLFW window/context management
  - `Input` for keyboard/mouse state handling

- **Graphics**
  - `Shader` for shader compilation and uniform uploads
  - `Mesh` for VAO/VBO/EBO management
  - `Texture` and `TextureManager` for texture loading/caching
  - `Skybox` for cubemap rendering
  - `Camera` for third-person chase camera behavior
  - `Hud` for HUD rendering
  - `Model` for aircraft OBJ rendering

- **Simulation**
  - `Terrain` for heightmap/procedural terrain generation and sampling
  - `Plane` for simplified flight dynamics and aircraft state

- **Config**
  - `Tuning.h` for central gameplay/render/flight tuning constants

## Current Scope

This repository is prepared as a clean source submission / GitHub upload for the current state of the project.

It is intended as:

- a graphical programming / OpenGL project
- a playable flight-scene prototype
- a structured C++ project with separated rendering, simulation, and configuration layers

It is **not** intended to be a full real-world flight simulator.

## Known Limitations

- Flight physics are simplified and gameplay-oriented
- Camera behavior is tuned for readability rather than full realism
- Terrain wrapping is used to avoid hard world termination
- Control model is intentionally limited to core WASD flight input

## GitHub Upload Readiness

This repository now includes a `.gitignore` suitable for:

- build folders
- generated CMake files
- binaries and object files
- IDE/editor metadata

This keeps the repository focused on source code, assets, and project configuration.