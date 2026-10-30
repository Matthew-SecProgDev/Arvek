#include <yforge/core/engine.h>
#include <yforge/core/logger.h>

#include <cstdlib>

class Application
{
};

int main()
{
    yforge::Logger::setMinLevel(yforge::LogLevel::Trace);

    YF_LOG_INFO("YForge sandbox started");
    YF_LOG_TRACE("Using C++{}", 17);

    yforge::Engine engine;

    engine.initialize();
    YF_LOG_WARN("Vulkan renderer is not integrated yet");
    engine.run<Application>();
    engine.shutdown();
    YF_LOG_INFO("YForge sandbox shutdown complete");

    return EXIT_SUCCESS;
}