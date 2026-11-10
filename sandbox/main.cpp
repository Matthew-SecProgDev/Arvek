#include <arvek/core/engine.h>
#include <arvek/core/logger.h>
#include <arvek/core/application_startup_context.h>
#include <arvek/core/application.h>

#include <cstdlib>
#include <string>
#include <exception>

class SandboxApplication final : public arvek::Application
{
public:
    void onStart(arvek::ApplicationStartupContext& context) override
    {
        (void)context;

        ARV_LOG_INFO("Sandbox application started.");
    }

    void onShutdown() override
    {
        ARV_LOG_INFO("Sandbox application stopped.");
    }
};

int main()
{
    try
    {
        arvek::Engine engine;

        engine.initialize();
        engine.run<SandboxApplication>();
        engine.shutdown();

        return EXIT_SUCCESS;
    }
    catch (const std::exception& e)
    {
        ARV_LOG_ERROR(std::string("Sandbox failed: ") + e.what());

        return EXIT_FAILURE;
    }
}