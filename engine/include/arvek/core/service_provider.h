#pragma once

#include <typeindex>
#include <stdexcept>

namespace arvek
{
    class ServiceRegistry;

    class ServiceProvider final
    {
    public:
        template<typename T>
        [[nodiscard]]
    	T* find() const noexcept;

        template<typename T>
        [[nodiscard]]
    	T& require() const;

    private:
        friend class ServiceRegistry;

        explicit ServiceProvider(ServiceRegistry& registry) noexcept;

        [[nodiscard]]
        void* find(std::type_index type) const noexcept;

        ServiceRegistry* registry_{ nullptr };
    };
}

template <typename T>
T* arvek::ServiceProvider::find() const noexcept
{
    return static_cast<T*>(find(std::type_index(typeid(T))));
}

template <typename T>
T& arvek::ServiceProvider::require() const
{
    T* service = find<T>();

    if (!service)
    {
        throw std::logic_error(
            "Required service is not registered!"
        );
    }

    return *service;
}