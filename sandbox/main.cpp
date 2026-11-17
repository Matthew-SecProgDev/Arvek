#include <arvek/core/application_runtime.h>
#include <arvek/core/application_startup_context.h>
#include <arvek/core/logger.h>

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
    return arvek::runApplication<SandboxGame>();
}
