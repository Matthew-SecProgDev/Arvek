#include <arvek/core/logger.h>
#include "instance.h"
#include "surface.h"
#include "physical_device.h"
#include "device.h"
#include "window_test_fixture.h"

#include <exception>
#include <string>
#include <cstdlib>

int main()
{
    try
    {
        arvek::WindowTestFixture fixture;

        {
            arvek::Instance instance;

            arvek::Surface surface(fixture.window(), instance);

            arvek::PhysicalDevice physicalDevice(instance, surface);

            arvek::Device device(physicalDevice);

            if (device.handle() == VK_NULL_HANDLE)
            {
                return EXIT_FAILURE;
            }

            if (device.graphicsQueue() == VK_NULL_HANDLE)
            {
                return EXIT_FAILURE;
            }

            if (device.presentationQueue() == VK_NULL_HANDLE)
            {
                return EXIT_FAILURE;
            }

            ARV_LOG_INFO("Logical device created successfully.");

            ARV_LOG_INFO(
                "GPU: " +
                std::string(
                    physicalDevice.properties().deviceName
                )
            );
        }
    }
    catch (const std::exception& e)
    {
        ARV_LOG_ERROR(
            std::string("Vulkan device test failed: ") +
            e.what()
        );

        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}