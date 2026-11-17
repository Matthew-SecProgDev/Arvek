#include <arvek/core/engine.h>
#include <arvek/core/logger.h>
#include <arvek/core/application_startup_context.h>
#include <arvek/core/application.h>

#include <cstdlib>
#include <string>
#include <exception>

namespace
{
    class SandboxGame final : public arvek::Application
    {
    public:
        void onStart(arvek::ApplicationStartupContext& context) override
        {
            ARV_LOG_INFO("Sandbox application started.");
        }

        void onShutdown() override
        {
            ARV_LOG_INFO("Sandbox application shutting down.");
        }
    };
}

int main()
{
    try
    {
        arvek::Engine engine;

        engine.initialize();
        engine.run<SandboxGame>();
        engine.shutdown();

        ARV_LOG_INFO("Sandbox execution completed.");

        return EXIT_SUCCESS;
    }
    catch (const std::exception& exception)
    {
        ARV_LOG_ERROR(
            "Sandbox failure: " +
            std::string(exception.what())
        );

        return EXIT_FAILURE;
    }
}
