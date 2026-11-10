#include <arvek/core/application.h>
#include <arvek/core/application_startup_context.h>
#include <arvek/core/service_provider.h>
#include <arvek/core/service_registrar.h>

#include <cstdlib>
#include <type_traits>
#include <utility>

namespace
{
    class TestApplication final : public arvek::Application
    {
    public:
        void onStart(arvek::ApplicationStartupContext&) override
        {
        }

        void onShutdown() override
        {
            shutdownCalled_ = true;
        }

        [[nodiscard]]
        bool shutdownCalled() const noexcept
        {
            return shutdownCalled_;
        }

    private:
        bool shutdownCalled_{ false };
    };
}

int main()
{
    static_assert(std::is_abstract_v<arvek::Application>);

    static_assert(std::is_base_of_v<arvek::Application, TestApplication>);

    static_assert(!std::is_copy_constructible_v<arvek::ApplicationStartupContext>);

    static_assert(std::is_same_v<
        decltype(std::declval<arvek::ApplicationStartupContext&>().serviceProvider()),
        arvek::ServiceProvider&>
        );

    static_assert(std::is_same_v<
        decltype(std::declval<arvek::ApplicationStartupContext&>().serviceRegistrar()),
        arvek::ServiceRegistrar&>
        );

    TestApplication app;

    if (app.shutdownCalled())
    {
        return EXIT_FAILURE;
    }

    app.onShutdown();

    if (!app.shutdownCalled())
    {
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}