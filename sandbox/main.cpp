#include <arvek/core/engine.h>
#include <arvek/core/logger.h>
#include <arvek/core/assert.h>

#include <cstdlib>

class Application
{
};

int main()
{
    int value = 42;
    ARV_ASSERT(value == 42, "Value must equal 42");
    //ARV_ASSERT(value == 100, "Value must equal 100");

    arvek::Logger::setMinLevel(arvek::LogLevel::Trace);

    ARV_LOG_INFO("Arvek sandbox started");
    ARV_LOG_TRACE("Using C++{}", 17);

    arvek::Engine engine;

    engine.initialize();
    ARV_LOG_WARN("Vulkan renderer is not integrated yet");
    engine.run<Application>();
    engine.shutdown();
    ARV_LOG_INFO("Arvek sandbox shutdown complete");

    return EXIT_SUCCESS;
}