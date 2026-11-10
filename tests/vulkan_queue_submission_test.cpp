#include <arvek/core/engine.h>
#include <arvek/core/logger.h>
#include "command_pool.h"
#include "device.h"
#include "fence.h"
#include "instance.h"
#include "physical_device.h"
#include "surface.h"

#include <exception>
#include <stdexcept>
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

            arvek::CommandPool commandPool(device, physicalDevice, 1);

            arvek::Fence fence(device);

            VkCommandBuffer commandBuffer = commandPool.commandBuffers().at(0);

            for (int iteration = 0; iteration < 2; ++iteration)
            {
                VkCommandBufferBeginInfo beginInfo{};
                beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
                beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

                if (vkBeginCommandBuffer(commandBuffer, &beginInfo) != VK_SUCCESS)
                {
                    throw std::runtime_error("Failed to begin command buffer!");
                }

                // No drawing commands yet.

                if (vkEndCommandBuffer(commandBuffer) != VK_SUCCESS)
                {
                    throw std::runtime_error("Failed to end command buffer!");
                }

                VkSubmitInfo submitInfo{};
                submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
                submitInfo.commandBufferCount = 1;
                submitInfo.pCommandBuffers = &commandBuffer;

                if (vkQueueSubmit(
                    device.graphicsQueue(),
                    1,
                    &submitInfo,
                    fence.handle()) != VK_SUCCESS)
                {
                    throw std::runtime_error("Failed to submit command buffer!");
                }

                // Wait until the submitted work completes.
                fence.wait();

                // The GPU no longer uses this buffer or fence.
                commandPool.reset();
                fence.reset();
            }

            ARV_LOG_INFO("Vulkan queue submission and fence synchronization passed.");
        }

        engine.shutdown();
    }
    catch (const std::exception& e)
    {
        ARV_LOG_ERROR(
            std::string("Queue submission test failed: ") +
            e.what()
        );

        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}