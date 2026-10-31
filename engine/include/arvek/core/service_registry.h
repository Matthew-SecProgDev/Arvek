#pragma once

#include <cstddef>
#include <memory>
#include <stdexcept>
#include <typeindex>
#include <type_traits>
#include <unordered_map>
#include <utility>
#include <vector>

namespace arvek
{
    class ServiceRegistry final
    {
    public:
        ServiceRegistry() = default;
        ~ServiceRegistry();

        ServiceRegistry(const ServiceRegistry&) = delete;
        ServiceRegistry& operator=(const ServiceRegistry&) = delete;
        ServiceRegistry(ServiceRegistry&&) = delete;
        ServiceRegistry& operator=(ServiceRegistry&&) = delete;

        template<typename T>
        T& add(std::unique_ptr<T> service);

        template<typename T>
        [[nodiscard]]
        T* find() const noexcept;

        template<typename T>
        [[nodiscard]]
        T& require() const;

        void clear() noexcept;

        [[nodiscard]]
        std::size_t size() const noexcept;

    private:
        struct IServiceHolder
        {
            virtual ~IServiceHolder() = default;
        };

        template<typename T>
        struct ServiceHolder final : IServiceHolder
        {
            explicit ServiceHolder(std::unique_ptr<T> value)
                : service(std::move(value))
            {
            }

            std::unique_ptr<T> service;
        };

        std::unordered_map<std::type_index, void*> services_;
        std::vector<std::unique_ptr<IServiceHolder>> ownedServices_;
    };
}

template <typename T>
T& arvek::ServiceRegistry::add(std::unique_ptr<T> service)
{
    static_assert(!std::is_void_v<T>, "Service type cannot be void");

    if (!service)
    {
        throw std::invalid_argument("Cannot register a null service");
    }

    const std::type_index key{ typeid(T) };

    if (services_.find(key) != services_.end())
    {
        throw std::logic_error("Service type is already registered");
    }

    T* instance = service.get();

    ownedServices_.push_back(
        std::make_unique<ServiceHolder<T>>(std::move(service))
    );

    try
    {
        services_.emplace(key, instance);
    }
    catch (...)
    {
        ownedServices_.pop_back();
        throw;
    }

    return *instance;
}

template <typename T>
T* arvek::ServiceRegistry::find() const noexcept
{
    const auto it = services_.find(std::type_index(typeid(T)));

    if (it == services_.end())
    {
        return nullptr;
    }

    return static_cast<T*>(it->second);
}

template <typename T>
T& arvek::ServiceRegistry::require() const
{
    T* service = find<T>();

    if (!service)
    {
        throw std::logic_error("Required service is not registered");
    }

    return *service;
}