#include <arvek/core/engine.h>

#include <arvek/core/logger.h>
#include <arvek/core/window.h>
#include "service_registry.h"
#include "glfw_runtime.h"
#include "window_config.h"
#include "../rendering/vulkan_renderer.h"
#include "window_factory.h"
#include <arvek/core/application.h>
#include <arvek/core/application_startup_context.h>
#include <arvek/core/service_registrar.h>
#include <arvek/core/service_provider.h>

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

    void Engine::initialize()
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
        auto renderer = std::make_unique<VulkanRenderer>(*window);

        serviceRegistry_ = std::move(registry);
        glfwRuntime_ = std::move(runtime);
        window_ = std::move(window);
        renderer_ = std::move(renderer);

        state_ = State::Initialized;

        ARV_LOG_INFO("Arvek Engine initialized successfully.");
    }

    void Engine::requestExit() noexcept
    {
        if (state_ == State::Running)
        {
            exitRequested_ = true;
        }
    }

    void Engine::shutdown()
    {
        if (state_ == State::Shutdown)
        {
            return;
        }

        // Destroying engine resources while the frame loop
        // is executing would invalidate active references.
        if (state_ == State::Running)
        {
            throw std::logic_error(
                "Cannot shut down Engine while it is running! "
                "Use requestExit() instead."
            );
        }

        // Stop application and scene activity first
        // when those systems are implemented.

        renderer_.reset();

        window_.reset();
        glfwRuntime_.reset();
        serviceRegistry_.reset();

        exitRequested_ = false;
        state_ = State::Shutdown;

        ARV_LOG_INFO("Arvek Engine shutdown complete.");
    }

    Window& Engine::window()
    {
        if ((state_ != State::Initialized &&
            state_ != State::Running) ||
            !window_)
        {
            throw std::logic_error("Engine window is not available!");
        }

        return *window_;
    }

    void Engine::runApplication(Application& app)
    {
        if (state_ != State::Initialized ||
            !serviceRegistry_ ||
            !window_ ||
            !renderer_)
        {
            throw std::logic_error(
                "Engine is not ready to run an application!"
            );
        }

        // Enter the running state before onStart(), so a
        // startup callback can request an immediate exit.
        state_ = State::Running;
        exitRequested_ = false;

        auto provider = serviceRegistry_->provider();
        auto registrar = serviceRegistry_->gameRegistrar();

        ApplicationStartupContext context(
            provider,
            registrar
        );

        try
        {
            app.onStart(context);
        }
        catch (...)
        {
            // Startup never completed, so onShutdown()
            // is not called. RAII handles partial startup.
            serviceRegistry_->clearGameServices();

            exitRequested_ = false;
            state_ = State::Initialized;

            throw;
        }

        // Preserve the first failure, even if shutdown
        // subsequently throws another exception.
        std::exception_ptr failure;

        try
        {
            while (!exitRequested_ && !window_->shouldClose())
            {
                window_->pollEvents();

                // A false return means rendering cannot continue,
                // usually because the window is closing during swapchain recreation.
                if (!renderer_->drawFrame())
                {
                    requestExit();
                }
            }
        }
        catch (...)
        {
            failure = std::current_exception();
        }

        // This runs even if the frame loop threw.
        try
        {
            app.onShutdown();
        }
        catch (...)
        {
            if (!failure)
            {
                failure = std::current_exception();
            }
        }

        // Game-owned services must outlive onShutdown().
        // Engine-owned services remain registered.
        serviceRegistry_->clearGameServices();

        exitRequested_ = false;
        state_ = State::Initialized;

        if (failure)
        {
            std::rethrow_exception(failure);
        }
    }
}
