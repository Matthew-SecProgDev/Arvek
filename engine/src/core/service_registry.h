#pragma once

#include <arvek/core/service_holder.h>

#include <memory>
#include <stdexcept>
#include <typeindex>
#include <type_traits>
#include <typeinfo>
#include <unordered_map>
#include <utility>
#include <vector>

namespace arvek
{
    class ServiceRegistrar;
    class ServiceProvider;

    class ServiceRegistry final
    {
    public:
        ServiceRegistry() = default;
        ~ServiceRegistry();

        ServiceRegistry(const ServiceRegistry&) = delete;
        ServiceRegistry& operator=(const ServiceRegistry&) = delete;
        ServiceRegistry(ServiceRegistry&&) = delete;
        ServiceRegistry& operator=(ServiceRegistry&&) = delete;

        [[nodiscard]]
        ServiceProvider provider() noexcept;

        [[nodiscard]]
        ServiceRegistrar gameRegistrar() noexcept;

        template<typename T>
        T& addEngineService(std::unique_ptr<T> service);

        void clear() noexcept;

        [[nodiscard]]
        std::size_t size() const noexcept;

    private:
        friend class ServiceRegistrar;
        friend class ServiceProvider;

        enum class ServiceCategory
        {
            Engine,
            Game
        };

        void addErased(
            std::type_index type,
            std::unique_ptr<IServiceHolder> holder,
            ServiceCategory category
        );

        [[nodiscard]]
        void* find(std::type_index type) const noexcept;

        std::unordered_map<std::type_index, void*> services_;
        std::vector<std::unique_ptr<IServiceHolder>> engineServices_;
        std::vector<std::unique_ptr<IServiceHolder>> gameServices_;
    };
}

template <typename T>
T& arvek::ServiceRegistry::addEngineService(std::unique_ptr<T> service)
{
    static_assert(std::is_object_v<T> && !std::is_const_v<T>,
        "Service must be a non-const object type!"
        );

    if (!service)
    {
        throw std::invalid_argument(
            "Cannot register a null service!"
        );
    }

    T* instance = service.get();

    addErased(
        std::type_index(typeid(T)),
        std::make_unique<ServiceHolder<T>>(
            std::move(service)
        ),
        ServiceCategory::Engine
    );

    return *instance;
}