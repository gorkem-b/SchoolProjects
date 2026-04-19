# SolarSystemGL

Interactive solar system visualization built with C++17 and OpenGL.

## Scope

The scene includes:

- Sun with emissive surface, corona shell, and halo shell
- Mercury, Venus, Earth, Mars, Jupiter, Saturn, Uranus, Neptune, and Pluto
- Representative major moons for supported planets
- Main asteroid belt with a small set of visible bodies
- Sparse outer belt bodies and low-detail dust rings
- Skybox background
- Fly camera controls

## Controls

- `W`, `A`, `S`, `D`: move camera
- Mouse: look around
- `Esc`: close the application

## Tech Stack

- C++17
- OpenGL 3.3 Core
- GLFW
- GLAD
- GLM
- stb_image
- CMake

## Build

```bash
cmake -S . -B build -G Ninja
cmake --build build
```

## Run

### Windows

```bash
build\SolarSystemGL.exe
```

### VS Code tasks

- `CMake: Configure`
- `CMake: Build SolarSystemGL`
- `Run: SolarSystemGL`

## Texture Generation

The repository includes `scripts/generate_solar_assets.py` for refreshing downloaded and generated textures.

Requirements:

- Python 3
- Pillow

Install Pillow:

```bash
pip install pillow
```

Run the script:

```bash
python scripts/generate_solar_assets.py
```

The script downloads available planetary textures and generates project-specific assets such as:

- `sun_corona.png`
- `dust_ring.png`
- asteroid textures
- moon surface textures
- procedural Pluto texture

## Project Structure

```text
SolarSystemGL/
├── CMakeLists.txt
├── GUIDE.md
├── README.md
├── res/
│   ├── shaders/
│   └── textures/
├── scripts/
│   └── generate_solar_assets.py
├── src/
│   ├── Core/
│   ├── Graphics/
│   └── Scene/
└── vendor/
```

## Rendering Notes

- The scene is organized as a scene graph.
- Planets and moons use parent-child orbital transforms.
- Selected visual overlays inherit parent scale when required.
- The Sun corona and halo use additive blending.
- Transparent rings and dust layers use opacity textures.

## Repository Notes

- `build/` is generated output and should not be committed.
- `.vscode/` contains workspace settings and tasks used by this repository.
