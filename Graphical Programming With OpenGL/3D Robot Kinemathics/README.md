# 3D Physics-Based Articulated Figure Simulation with OpenGL

This project implements a **3D articulated figure simulation** using **C++**, **OpenGL**, **FreeGLUT**, and **CMake**. Instead of directly setting joint angles, the active arm is driven by a simplified **physics-based model** with **torque input**, **gravity**, **damping**, **joint limits**, and **torso self-collision prevention**.

The application renders a 3D character-like figure, simulates a physically driven two-link arm, and provides real-time interaction, scenario switching, and debug visualization.

---

## Tech Stack

- **Language:** C++11+
- **Graphics API:** OpenGL (fixed-function pipeline)
- **Windowing / Input:** FreeGLUT
- **Build System:** CMake
- **Generator Used in This Project:** Ninja

---

## Project Overview

At runtime, the program opens a 3D scene with:

- a floor plane and reference grid,
- an orbit camera,
- a simple articulated humanoid figure,
- a physics-driven right arm,
- an on-screen overlay showing simulation state.

The main focus of the project is the **active right arm**, modeled as a **two-link articulated rigid-segment system**:

- **upper arm**,
- **forearm**,
- connected through **shoulder** and **elbow** joints.

The arm evolves over time under:

- applied joint torques,
- gravitational torque,
- damping,
- joint-angle limits,
- simplified torso contact correction.

---

## Implemented Features

- 3D rendering with OpenGL
- Orbit camera with mouse drag and zoom
- Fixed timestep physics loop
- Torque-driven shoulder and elbow control
- Gravity and damping in the dynamic model
- Joint angle limits
- Simplified torso self-collision prevention
- Lighting toggle
- Wireframe / solid rendering toggle
- Collision debug volume visualization
- Runtime HUD / overlay with:
  - shoulder angle, velocity, and torque
  - elbow angle, velocity, and torque
  - kinetic, potential, and total energy
  - collision state and penetration amount

---

## Physics Model

The active arm is modeled as a simplified **two-link articulated rigid-body system** in a plane, visualized inside a 3D OpenGL scene.

### State variables

- shoulder angle
- elbow angle
- shoulder angular velocity
- elbow angular velocity

### Inputs

- shoulder torque
- elbow torque

### Physical effects

- gravity
- damping
- inertia / link mass properties
- joint limits
- torso contact correction

### Numerical update

The simulation uses a **fixed timestep loop** and updates the system every frame using the dynamic equations of a 2-link manipulator. The render loop and physics loop are separated so the simulation remains more stable than frame-dependent updates.

---

## Collision / Contact Handling

This project includes a **simplified self-collision prevention layer** between the active arm and the torso.

Current approach:

- the torso is treated as a simple box-like collision region,
- the active arm is sampled along the upper arm and forearm,
- if sampled points penetrate the torso proxy volume, a correction is applied,
- angular velocities are damped after contact to reduce unrealistic interpenetration.

This is **not a full rigid-body solver**, but it significantly improves physical plausibility compared to unconstrained joint motion.

---

## Controls

### Physics controls

- `Q / A` -> apply positive / negative shoulder torque (hold)
- `W / S` -> apply positive / negative elbow torque (hold)
- `1` -> load **Free Swing** scenario
- `2` -> load **Damped Release** scenario
- `3` -> load **Torque Control** scenario
- `R` -> reset the current scenario
- `Space` -> pause / resume physics

### Visualization controls

- `L` -> toggle lighting
- `M` -> toggle wireframe mode
- `B` -> toggle collision debug volumes

### Camera controls

- `Mouse Drag` -> orbit camera
- `Mouse Wheel` -> zoom
- `+ / -` -> zoom in / out

### General

- `Esc` -> exit

---

## Project Structure

```text
3D Robot Kinemathics/
├── CMakeLists.txt
├── README.md
├── build-ninja/
└── src/
    ├── main.cpp
    ├── App.h
    ├── App.cpp
    ├── Camera.h
    ├── Camera.cpp
    ├── Robot.h
    ├── Robot.cpp
    ├── Input.h
    └── Input.cpp
```

### File responsibilities

- **`main.cpp`**
  - Application entry point
  - Creates and runs the `App` object

- **`App.cpp / App.h`**
  - OpenGL initialization
  - render loop and fixed timestep update loop
  - overlay/HUD rendering
  - lighting and scene management

- **`Camera.cpp / Camera.h`**
  - orbit camera behavior
  - mouse interaction and zoom

- **`Robot.cpp / Robot.h`**
  - articulated figure rendering
  - physics state and dynamics of the active arm
  - joint limits
  - simplified self-collision correction
  - debug collision volume drawing

- **`Input.cpp / Input.h`**
  - keyboard and mouse callbacks
  - torque-based user input
  - scenario switching and debug toggles

---

## Build Instructions

### Configure

```powershell
cmake -S . -B build-ninja -G Ninja
```

### Build

```powershell
cmake --build build-ninja
```

### Run

```powershell
build-ninja\RobotKinematics.exe
```

---

## Recommended Workflow

If you modify the source code:

```powershell
cmake --build build-ninja
build-ninja\RobotKinematics.exe
```

If the build directory must be recreated:

```powershell
cmake -S . -B build-ninja -G Ninja
cmake --build build-ninja
build-ninja\RobotKinematics.exe
```

---

## Current Limitations

This project is intentionally scoped as a manageable course project, so it does **not** implement a full general-purpose rigid-body engine.

Current limitations include:

- only one arm is actively simulated by the physics model,
- the collision model is simplified,
- torso contact uses positional correction rather than a full impulse solver,
- there is no full-body articulated rigid-body solver,
- there is no full ground-contact / friction system.

---

## Possible Future Improvements

- full-body articulated dynamics
- better self-collision filtering and response
- ground contact and friction
- generalized rigid-body constraints
- improved collision proxies (capsules instead of simple sampled correction)
- richer scenario presets and parameter tuning UI
