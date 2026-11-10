#include <arvek/core/engine.h>
#include <arvek/core/logger.h>
#include "command_pool.h"
#include "device.h"
#include "instance.h"
#include "physical_device.h"
#include "surface.h"

#include <exception>
#include <string>
#include <cstdlib>

int main()
{
    try
    {
        arvek::Engine engine;
        engine.initialize(false);

        {
            arvek::Instance instance(true);

            arvek::Surface surface(engine.window(), instance);

            arvek::PhysicalDevice physicalDevice(instance, surface);

            arvek::Device device(physicalDevice);

            arvek::CommandPool commandPool(device, physicalDevice, 2);

            if (commandPool.handle() == VK_NULL_HANDLE)
            {
                return EXIT_FAILURE;
            }

            if (commandPool.commandBuffers().size() != 2)
            {
                return EXIT_FAILURE;
            }

            for (VkCommandBuffer buffer : commandPool.commandBuffers())
            {
                if (buffer == VK_NULL_HANDLE)
                {
                    return EXIT_FAILURE;
                }

                VkCommandBufferBeginInfo beginInfo{};
                beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

                if (vkBeginCommandBuffer(buffer, &beginInfo) != VK_SUCCESS)
                {
                    return EXIT_FAILURE;
                }

                // No GPU commands yet.

                if (vkEndCommandBuffer(buffer) != VK_SUCCESS)
                {
                    return EXIT_FAILURE;
                }
            }

            // Both buffers are executable but have not
            // been submitted, so resetting is safe.
            commandPool.reset();

            ARV_LOG_INFO(
                "Vulkan command pool and buffers initialized."
            );
        }

        engine.shutdown();
    }
    catch (const std::exception& e)
    {
        ARV_LOG_ERROR(
            std::string("Command pool test failed: ") +
            e.what()
        );

        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}