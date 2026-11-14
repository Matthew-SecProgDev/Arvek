# Arvek

**Arvek** is an early-stage, open-source game engine written in **C++17**, with a **Vulkan** rendering backend. It is being built incrementally, with an emphasis on clear subsystem boundaries, explicit resource ownership, testability, and performance-conscious design.

The long-term goal is a general-purpose engine for creating games, particularly fast-paced 3D experiences. Arvek is **under active development** and is not yet ready for building complete games.

## Current capabilities

### Core

- Engine initialization and shutdown with explicit lifecycle checks.
- Logging and debug assertions.
- Service registration and lookup, with distinct provider and registrar responsibilities.
- GLFW-based window management and event handling.
- An initial `Application` abstraction with `onStart()` and `onShutdown()` callbacks.
- `ApplicationStartupContext` for access to the existing service infrastructure during startup.
- `Engine::run<TApp>()` integration for application startup and shutdown (**not yet a continuous game loop**).

### Rendering

- Vulkan instance, surface, physical-device selection, and logical-device setup.
- Swapchain creation and recreation, with image views.
- Render passes and framebuffers.
- Command pools, command buffers, fences, and semaphores.
- SPIR-V shader modules, pipeline layouts, and graphics pipelines.
- A `VulkanRenderer` owned by the engine, with a default clear-only rendering configuration.
- Manual examples/tests for clearing a window, presenting frames, rendering a triangle, and handling resize scenarios.

### Testing

- Automated tests using CTest, including engine lifecycle, services, window-related behavior, and Vulkan subsystems.
- Separate window test fixtures so low-level Vulkan tests do not initialize a competing engine-owned renderer.
- Manual graphics tests for behavior that requires visual inspection and a Vulkan-capable environment.

## Technology stack

| Area | Technology |
| --- | --- |
| Language | C++17 |
| Build system | CMake |
| Primary development platform | Windows / Visual Studio 2022 / MSVC |
| Graphics API | Vulkan |
| Windowing | GLFW |
| Logging and formatting | {fmt} |
| Testing | CTest |
| Shader compilation | Vulkan SDK tools (`glslc`) |

Dependency discovery and installation are governed by the repository's CMake configuration. The project is currently developed and tested primarily on Windows; other platforms should not be considered supported without verification.

## Getting started

### Prerequisites

Install:

1. **Visual Studio 2022** with C++ desktop development tools.
2. **CMake** and a supported build tool (for example, Ninja).
3. **Vulkan SDK**, including a compatible Vulkan driver and shader compilation tools.
4. The project dependencies required by its CMake configuration, including **GLFW** and **{fmt}**.

### Configure and build

From the repository root, using a **Developer PowerShell for VS 2022** terminal:

```powershell
cmake -S . -B build
cmake --build build --config Debug
```

If your local setup requires a specific generator or dependency paths, supply them to CMake during configuration.

### Run tests

```powershell
ctest --test-dir build -C Debug --output-on-failure
```

To validate Release as well:

```powershell
cmake --build build --config Release
ctest --test-dir build -C Release --output-on-failure
```

**Note:** Some graphics tests are manual and are not necessarily executed by ordinary CTest runs. Check the targets defined in `tests/CMakeLists.txt` and run the relevant manual executables separately. A successful automated test run does not establish that all rendering scenarios have been visually verified.

## Application API (current milestone)

The application lifecycle is intentionally minimal at this point:

```cpp
#include <arvek/core/application.h>
#include <arvek/core/application_startup_context.h>
#include <arvek/core/engine.h>

class MyGame final : public arvek::Application
{
public:
    void onStart(arvek::ApplicationStartupContext& context) override
    {
        // Register or access game services here.
        (void)context;
    }

    void onShutdown() override
    {
        // Clean up application-specific resources here.
    }
};

int main()
{
    arvek::Engine engine;
    engine.initialize();
    engine.run<MyGame>();
    engine.shutdown();
}
```

**Important:** In the current development stage, `run<MyGame>()` invokes application startup and shutdown but does **not** yet keep a window running in an engine-controlled frame loop. That work is planned next. The snippet demonstrates lifecycle usage rather than a complete playable application; production applications should also handle exceptions appropriately.

## Architecture direction

Arvek is being designed around distinct responsibilities:

- **Engine:** owns the core runtime, controls initialization and shutdown, and will coordinate application execution and frames.
- **Application:** configures the game at startup and participates in the application lifecycle.
- **Service infrastructure:** separates service registration from service retrieval and controls the lifetime of registered services.
- **Window layer:** abstracts platform/window operations from higher-level engine code.
- **VulkanRenderer:** owns low-level Vulkan rendering resources and GPU frame execution.
- **Future scene and system layers:** will manage world state, ECS entities/components, and game systems without exposing Vulkan implementation details to game code.

## Roadmap

This is a development direction, **not a release schedule**.

- [x] Core utilities, service infrastructure, and engine lifecycle.
- [x] Windowing and foundational Vulkan resource management.
- [x] Vulkan triangle rendering and renderer ownership in `Engine`.
- [x] Isolated Vulkan tests using independent window fixtures.
- [x] Basic `Application` / `ApplicationStartupContext` integration.
- [ ] Engine-controlled application/rendering loop and exit handling.
- [ ] Improve application execution-state handling and lifecycle tests.
- [ ] Harden Vulkan synchronization, presentation, resize, and resource lifetime.
- [ ] Introduce game timing and input abstractions.
- [ ] Develop ECS, scenes, scene contexts, and system scheduling.
- [ ] Add a game-facing renderer API and mesh/material/texture submission.
- [ ] Expand toward gameplay tooling and a broader game engine feature set.

## Project layout

```text
Arvek/
├── engine/
│   ├── include/arvek/core/     # Public core and application APIs
│   └── src/
│       ├── core/               # Engine, services, windowing
│       └── rendering/          # Vulkan implementation
├── tests/                      # Automated and manual tests
└── sandbox/                    # Development sandbox
```

## License

Arvek is intended to be distributed under the **Apache License 2.0**. See the repository's `LICENSE` file for the authoritative license terms.
