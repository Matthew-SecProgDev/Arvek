#include <arvek/core/engine.h>
#include <arvek/core/logger.h>
#include "vulkan_renderer.h"

#include <exception>
#include <string>
#include <cstdlib>

int main()
{
    try
    {
        arvek::Engine engine;
        engine.initialize();

        {
            // Renderer is created after the window.
            arvek::VulkanRenderer renderer(engine.window());

            ARV_LOG_INFO("VulkanRenderer initialized successfully.");
        }

        // Renderer and its Vulkan resources are destroyed
        // before the engine destroys its window.
        engine.shutdown();

        ARV_LOG_INFO("VulkanRenderer destroyed successfully.");
    }
    catch (const std::exception& e)
    {
        ARV_LOG_ERROR(
            std::string("VulkanRenderer test failed: ") +
            e.what()
        );

        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}