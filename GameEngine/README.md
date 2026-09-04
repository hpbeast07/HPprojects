# GameEngine

A modular 3D game-engine foundation and visual editor built with C++20, CMake, and OpenGL 4.5.

## Prerequisites

- Windows 10/11
- MSYS2 with MinGW-w64 (`C:\msys64`)
- Python 3.x (for GLAD generation)
- GPU with OpenGL 4.5 support

## Quick Start

```bash
# 1. Generate the GLAD OpenGL 4.5 loader (only needed when regenerating GLAD)
python generate_glad.py

# 2. Build (Debug)
build.bat

# 3. Run the editor
build\bin\GameEngineEditor.exe
```

## Build Commands

```bash
build.bat           # Debug build
build.bat release   # Release build
build.bat clean     # Clean build directory
```

## Current Foundation Features

- ✅ OpenGL 4.5 core window (1280×720)
- ✅ Colored animated triangle
- ✅ FPS/frame-time overlay
- ✅ Keyboard input (F1=wireframe, ESC=quit)
- ✅ Fixed + variable timestep loop
- ✅ Logger (console + file)
- ✅ ImGui with docking support
- ✅ Separate Editor + Runtime executables
- ✅ Perspective camera and right-mouse fly-camera controller
- ✅ Basic scene graph with transforms, hierarchy, and world transforms

## Keyboard Controls

| Key | Action |
|-----|--------|
| `F1` | Toggle wireframe |
| `ESC` | Quit |
| Right mouse + `W/A/S/D/Q/E` | Fly-camera look and movement (available to layers that use `FlyCameraController`) |

## Architecture

```
GameEngine/
├── Engine/          ← Static library (engine core)
│   ├── Core/        ← Application, Window, Logger
│   ├── Input/       ← Keyboard, mouse, gamepad
│   ├── Math/        ← GLM aliases
│   ├── Renderer/    ← OpenGL backend, shaders, buffers, camera
│   └── Scene/       ← Scene graph and transforms
├── Editor/          ← Editor executable
├── Runtime/         ← Standalone game executable
└── vendor/          ← Third-party libraries (glad, stb)
```

## Dependencies (auto-fetched by CMake)

| Library | Version | Purpose |
|---------|---------|---------|
| GLFW | 3.4 | Window + input |
| GLM | 1.0.1 | Math |
| Dear ImGui | docking | Editor UI |
| GLAD | GL 4.5 | OpenGL loader |

## Roadmap

- **Phase 1** ✅ Foundation (window, loop, input, basic renderer)
- **Phase 2** 🔄 Scene rendering, meshes, materials, textures, framebuffers, editor viewport
- **Phase 3** ⏳ PBR meshes, materials, lighting, shadows
- **Phase 4** ⏳ ECS, scene graph, serialization
- **Phase 5** ⏳ Physics (Jolt), audio, animation
- **Phase 6** ⏳ Visual editor (hierarchy, inspector, gizmos)
- **Phase 7** ⏳ Particles, UI, scripting, AI
- **Phase 8** ⏳ Networking, streaming, LOD, jobs
- **Phase 9** ⏳ Optimization, profiler, packaging
