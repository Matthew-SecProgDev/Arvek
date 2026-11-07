#include <arvek/core/engine.h>
#include <arvek/core/logger.h>
#include "device.h"
#include "instance.h"
#include "physical_device.h"
#include "surface.h"
#include "swapchain.h"

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
            arvek::Instance instance(true);

            arvek::Surface surface(engine.window(), instance);

            arvek::PhysicalDevice physicalDevice(instance, surface);

            arvek::Device device(physicalDevice);

            arvek::Swapchain swapchain(
                physicalDevice,
                device,
                surface,
                engine.window()
            );

            if (swapchain.handle() == VK_NULL_HANDLE)
            {
                return EXIT_FAILURE;
            }

            if (swapchain.images().empty())
            {
                return EXIT_FAILURE;
            }

            if (swapchain.images().size() != swapchain.imageViews().size())
            {
                return EXIT_FAILURE;
            }

            if (swapchain.imageFormat() == VK_FORMAT_UNDEFINED)
            {
                return EXIT_FAILURE;
            }

            if (swapchain.extent().width == 0 ||
                swapchain.extent().height == 0)
            {
                return EXIT_FAILURE;
            }

            ARV_LOG_INFO(
                "Vulkan swapchain and image views created successfully."
            );
        }

        engine.shutdown();
    }
    catch (const std::exception& e)
    {
        ARV_LOG_ERROR(
            std::string("Swapchain test failed: ") +
            e.what()
        );

        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}