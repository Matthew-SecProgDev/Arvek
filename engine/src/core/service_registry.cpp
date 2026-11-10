#include "service_registry.h"

#include <arvek/core/service_provider.h>
#include <arvek/core/service_registrar.h>

namespace arvek
{
    ServiceRegistry::~ServiceRegistry()
    {
        clear();
    }

    ServiceProvider ServiceRegistry::provider() noexcept
    {
        return ServiceProvider(*this);
    }

    ServiceRegistrar ServiceRegistry::gameRegistrar() noexcept
    {
        return ServiceRegistrar(*this);
    }

    void ServiceRegistry::clear() noexcept
    {
        // Lookup is disabled during destruction.
        services_.clear();

        // Destroy game services first.
        while (!gameServices_.empty())
        {
            gameServices_.pop_back();
        }

        // Then destroy engine services.
        while (!engineServices_.empty())
        {
            engineServices_.pop_back();
        }
    }

    std::size_t ServiceRegistry::size() const noexcept
    {
        return services_.size();
    }

    void ServiceRegistry::addErased(
        std::type_index type,
        std::unique_ptr<IServiceHolder> holder,
        ServiceCategory category)
    {
        if (!holder || !holder->get())
        {
            throw std::invalid_argument(
                "Cannot register a null service!"
            );
        }

        if (services_.find(type) != services_.end())
        {
            throw std::logic_error(
                "Service type is already registered!"
            );
        }

        auto& storage = category == ServiceCategory::Engine
            ? engineServices_
            : gameServices_;

        // Complete potentially throwing allocation first.
        storage.reserve(storage.size() + 1);

        void* instance = holder->get();

        // Insert lookup information before transferring ownership.
        services_.emplace(type, instance);

        // No allocation should occur after reserve().
        // Moving a unique_ptr is noexcept.
        storage.push_back(std::move(holder));
    }

    void* ServiceRegistry::find(std::type_index type) const noexcept
    {
        const auto it = services_.find(type);

        if (it == services_.end())
        {
            return nullptr;
        }

        return it->second;
    }
}