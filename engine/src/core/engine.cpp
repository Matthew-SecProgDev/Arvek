#include "arvek/core/engine.h"

#include <arvek/core/logger.h>
#include "service_registry.h"

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

        serviceRegistry_ = std::make_unique<ServiceRegistry>();

        state_ = State::Initialized;

        ARV_LOG_INFO("Arvek Engine initialized successfully.");
    }

    void Engine::shutdown()
    {
        if (state_ == State::Shutdown)
        {
            return;
        }

        if (serviceRegistry_)
        {
            serviceRegistry_->clear();
            serviceRegistry_.reset();
        }

        state_ = State::Shutdown;

        ARV_LOG_INFO("Arvek Engine shutdown complete.");
    }
}