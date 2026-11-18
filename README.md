# Arvek

**Arvek** is an early-stage, open-source game engine written in **C++17**, with a **Vulkan** rendering backend. It is being built incrementally, with an emphasis on clear subsystem boundaries, explicit resource ownership, testability, and performance-conscious design.

The long-term goal is a general-purpose engine for creating games, particularly fast-paced 3D experiences. Arvek is **under active development** and is not yet ready for building complete games.

## Current capabilities

### Core

- Engine initialization and shutdown with explicit lifecycle checks and execution-state handling.
- Logging and debug assertions.
- Service registration and lookup, with distinct provider and registrar responsibilities.
- GLFW-based window management and event handling.
- An `Application` abstraction with a required `onStart()` callback and an optional `onShutdown()` callback.
- `ApplicationStartupContext` for controlled access to service registration and lookup during startup.
- An engine-controlled event/rendering loop, with graceful exit requests and application lifecycle cleanup.
- `arvek::runApplication<TApp>()` as a high-level entry point that creates and runs the engine, handles top-level exceptions, and returns a process exit code.

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

## Application API

For ordinary game applications, use the high-level runtime entry point. Game code does not need to construct or shut down `Engine` directly:

```cpp
#include <arvek/core/application.h>
#include <arvek/core/application_runtime.h>
#include <arvek/core/application_startup_context.h>

class MyGame final : public arvek::Application
{
public:
    void onStart(arvek::ApplicationStartupContext& context) override
    {
        // Register game services and configure startup here.
        (void)context;
    }

    // onShutdown() is optional; use RAII for resource ownership.
};

int main()
{
    return arvek::runApplication<MyGame>();
}
```

`runApplication<TApp>()` manages engine creation, initialization, application execution, shutdown, and top-level exception reporting. The engine runs a continuous event/rendering loop until the window closes or an exit is requested. The default engine renderer clears/presents frames; the example is **not yet a complete game**.

## Architecture direction

Arvek is being designed around distinct responsibilities:

- **Runtime entry point:** manages engine construction, execution, cleanup, and top-level error reporting.
- **Engine:** owns the core runtime and renderer, controls subsystem initialization/shutdown, and coordinates the event/rendering loop.
- **Application:** configures the game during startup; shutdown customization is optional.
- **Service infrastructure:** separates service registration from service retrieval and controls the lifetime of registered services.
- **Window layer:** abstracts platform/window operations from higher-level engine code.
- **VulkanRenderer:** owns low-level Vulkan rendering resources and GPU frame execution.
- **Future time, input, scene, and system layers:** will measure time, process input, own game worlds and ECS entities/components, and coordinate gameplay behavior without exposing Vulkan implementation details to game code.

## Roadmap

This is a development direction, **not a release schedule**.

- [x] Core utilities, service infrastructure, and engine lifecycle.
- [x] Windowing and foundational Vulkan resource management.
- [x] Vulkan triangle rendering and renderer ownership in `Engine`.
- [x] Isolated Vulkan tests using independent window fixtures.
- [x] Basic `Application` / `ApplicationStartupContext` integration.
- [x] Engine-controlled application/rendering loop and exit handling.
- [x] Application execution-state handling and expanded lifecycle tests.
- [x] High-level `runApplication<TApp>()` runtime entry point.
- [x] Refine `Application` lifecycle (`onStart()` required; `onShutdown()` optional).
- [ ] Add game timing (frame delta and elapsed time), followed by fixed-step simulation support.
- [ ] Add input-state tracking and event processing.
- [ ] Develop ECS, scenes, scene contexts, and system scheduling.
- [ ] Harden Vulkan synchronization, presentation, resize, and resource lifetime.
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
