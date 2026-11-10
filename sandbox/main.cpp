#include <arvek/core/engine.h>
#include <arvek/core/logger.h>
#include <arvek/core/window.h>

#include <cstdlib>
#include <string>
#include <exception>

int main()
{
    try
    {
        arvek::Engine engine;

        engine.initialize(true);

        auto& window = engine.window();

        ARV_LOG_INFO("Arvek window initialized.");

        while (!window.shouldClose())
        {
            window.waitEvents();
        }

        engine.shutdown();

        ARV_LOG_INFO("Arvek shutdown complete.");
    }
    catch (const std::exception& e)
    {
        ARV_LOG_ERROR("Error: " + std::string(e.what()));

        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}