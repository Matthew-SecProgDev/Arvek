#include <arvek/core/engine.h>

#include <arvek/core/logger.h>
#include <arvek/core/window.h>
#include "service_registry.h"
#include "glfw_runtime.h"
#include "window_config.h"
#include "../rendering/vulkan_renderer.h"
#include "window_factory.h"

#include <cstdint>
#include <memory>
#include <stdexcept>

namespace arvek
{
    Engine::Engine() = default;

    Engine::~Engine()
    {
        shutdown();
    }

    void Engine::initialize(bool enableRenderer)
    {
        if(state_ != State::Constructed)
        {
            throw std::logic_error(
                "Engine cannot be initialized more than once!"
            );
        }

        auto registry = std::make_unique<ServiceRegistry>();

        auto runtime = std::make_unique<GlfwRuntime>();

        WindowConfig config;
        config.width = 1024;
        config.height = 768;
        config.title = "Arvek Engine";

        auto window = WindowFactory::create(config);

        std::unique_ptr<VulkanRenderer> renderer;

        if (enableRenderer)
        {
            renderer = std::make_unique<VulkanRenderer>(*window);
        }

        serviceRegistry_ = std::move(registry);
        glfwRuntime_ = std::move(runtime);
        window_ = std::move(window);
        renderer_ = std::move(renderer);

        state_ = State::Initialized;

        ARV_LOG_INFO("Arvek Engine initialized successfully.");
    }

    void Engine::shutdown()
    {
        if (state_ == State::Shutdown)
        {
            return;
        }

        // Stop application and scene activity first
        // when those systems are implemented.

        renderer_.reset();

        window_.reset();
        glfwRuntime_.reset();
        serviceRegistry_.reset();

        state_ = State::Shutdown;

        ARV_LOG_INFO("Arvek Engine shutdown complete.");
    }

    Window& Engine::window()
    {
        if (state_ != State::Initialized || !window_)
        {
            throw std::logic_error(
                "Engine window is not available!"
            );
        }

        return *window_;
    }
}