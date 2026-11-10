#include <arvek/core/logger.h>
#include "instance.h"
#include "surface.h"
#include "physical_device.h"
#include "window_test_fixture.h"

#include <exception>
#include <cstdlib>
#include <string>

int main()
{
    try
    {
        arvek::WindowTestFixture fixture;

        {
            arvek::Instance instance;
            arvek::Surface surface(fixture.window(), instance);
            arvek::PhysicalDevice device(instance, surface);

            if (device.handle() == VK_NULL_HANDLE)
            {
                return EXIT_FAILURE;
            }

            ARV_LOG_INFO(
                "Selected Vulkan GPU: " +
                std::string(device.properties().deviceName)
            );

            ARV_LOG_INFO(
                "Graphics queue family: " +
                std::to_string(device.graphicsFamilyIndex())
            );

            ARV_LOG_INFO(
                "Presentation queue family: " +
                std::to_string(device.presentationFamilyIndex())
            );
        }
    }
    catch (const std::exception& e)
    {
        ARV_LOG_ERROR(std::string(e.what()));
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}