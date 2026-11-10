#pragma once

namespace arvek
{
    class Engine;
    class ServiceProvider;
    class ServiceRegistrar;

    class ApplicationStartupContext final
    {
    public:
        ApplicationStartupContext() = delete;
        ~ApplicationStartupContext() = default;

        ApplicationStartupContext(const ApplicationStartupContext&) = delete;
        ApplicationStartupContext& operator=(const ApplicationStartupContext&) = delete;

        [[nodiscard]]
        ServiceProvider& serviceProvider() noexcept
        {
            return provider_;
        }

        [[nodiscard]]
        ServiceRegistrar& serviceRegistrar() noexcept
        {
            return registrar_;
        }

    private:
        friend class Engine;

        ApplicationStartupContext(ServiceProvider& provider,
            ServiceRegistrar& registrar) noexcept
            : provider_(provider),
            registrar_(registrar)
        {
        }

        ServiceProvider& provider_;
        ServiceRegistrar& registrar_;
    };
}