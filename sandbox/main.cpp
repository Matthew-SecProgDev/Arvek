#include <yforge/core/engine.h>
#include <yforge/core/logger.h>
#include <yforge/core/assert.h>

#include <cstdlib>

class Application
{
};

int main()
{
    int value = 42;
    YF_ASSERT(value == 42, "Value must equal 42");
    //YF_ASSERT(value == 100, "Value must equal 100");

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