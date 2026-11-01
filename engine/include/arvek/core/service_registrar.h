#pragma once

#include <arvek/core/service_holder.h>

#include <memory>
#include <utility>
#include <typeindex>
#include <type_traits>
#include <stdexcept>

namespace arvek
{
    class ServiceRegistry;

    class ServiceRegistrar final
    {
    public:
        template<typename T>
        T& addGameService(std::unique_ptr<T> service);

    private:
        friend class ServiceRegistry;

        explicit ServiceRegistrar(ServiceRegistry& registry) noexcept;

        void addGameService(
            std::type_index type,
            std::unique_ptr<IServiceHolder> holder
        );

        ServiceRegistry* registry_{ nullptr };
    };
}

template <typename T>
T& arvek::ServiceRegistrar::addGameService(std::unique_ptr<T> service)
{
    static_assert(
        std::is_object_v<T> && !std::is_const_v<T>,
        "Service must be a non-const object type!"
        );

    if (!service)
    {
        throw std::invalid_argument(
            "Cannot register a null service!"
        );
    }

    T* instance = service.get();

    addGameService(
        std::type_index(typeid(T)),
        std::make_unique<ServiceHolder<T>>(
            std::move(service)
        )
    );

    return *instance;
}