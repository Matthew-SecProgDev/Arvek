#include <arvek/core/engine.h>
#include <arvek/core/logger.h>
#include "instance.h"
#include "surface.h"
#include "physical_device.h"
#include "device.h"

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
            // Explicitly require validation in Debug and Release.
            arvek::Instance instance(true);

            arvek::Surface surface(engine.window(), instance);

            arvek::PhysicalDevice physicalDevice(instance, surface);

            arvek::Device device(physicalDevice);

            if (device.handle() == VK_NULL_HANDLE)
            {
                return EXIT_FAILURE;
            }

            ARV_LOG_INFO(
                "Vulkan initialized with validation enabled."
            );
        }

        engine.shutdown();
    }
    catch (const std::exception& e)
    {
        ARV_LOG_ERROR(
            std::string("Vulkan validation test failed: ") +
            e.what()
        );

        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}