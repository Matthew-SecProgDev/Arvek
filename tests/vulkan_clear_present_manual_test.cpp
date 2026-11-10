#include <arvek/core/engine.h>
#include <arvek/core/logger.h>
#include <arvek/core/window.h>
#include "command_pool.h"
#include "device.h"
#include "fence.h"
#include "instance.h"
#include "physical_device.h"
#include "semaphore.h"
#include "surface.h"
#include "swapchain.h"

#include <exception>
#include <stdexcept>
#include <string>
#include <cstdlib>

namespace
{
    void check(VkResult result, const char* operation)
    {
        if (result != VK_SUCCESS)
        {
            throw std::runtime_error(
                std::string(operation) +
                " failed. VkResult: " +
                std::to_string(static_cast<int>(result))
            );
        }
    }

    void recordClearCommands(VkCommandBuffer commandBuffer, VkImage image)
    {
        VkCommandBufferBeginInfo beginInfo{};
        beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
        beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

        check(
            vkBeginCommandBuffer(commandBuffer, &beginInfo),
            "Begin command buffer"
        );

        VkImageSubresourceRange range{};
        range.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        range.baseMipLevel = 0;
        range.levelCount = 1;
        range.baseArrayLayer = 0;
        range.layerCount = 1;

        // This is the first use of this swapchain image.
        // We don't need to preserve its previous contents.
        VkImageMemoryBarrier toTransfer{};
        toTransfer.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
        toTransfer.oldLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        toTransfer.newLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
        toTransfer.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        toTransfer.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        toTransfer.image = image;
        toTransfer.subresourceRange = range;
        toTransfer.srcAccessMask = 0;
        toTransfer.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;

        vkCmdPipelineBarrier(
            commandBuffer,
            VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT,
            VK_PIPELINE_STAGE_TRANSFER_BIT,
            0,
            0, nullptr,
            0, nullptr,
            1, &toTransfer
        );

        VkClearColorValue color{};
        color.float32[0] = 0.05f;
        color.float32[1] = 0.20f;
        color.float32[2] = 0.65f;
        color.float32[3] = 1.0f;

        vkCmdClearColorImage(
            commandBuffer,
            image,
            VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
            &color,
            1,
            &range
        );

        VkImageMemoryBarrier toPresent{};
        toPresent.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
        toPresent.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
        toPresent.newLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
        toPresent.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        toPresent.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        toPresent.image = image;
        toPresent.subresourceRange = range;
        toPresent.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
        toPresent.dstAccessMask = 0;

        vkCmdPipelineBarrier(
            commandBuffer,
            VK_PIPELINE_STAGE_TRANSFER_BIT,
            VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT,
            0,
            0, nullptr,
            0, nullptr,
            1, &toPresent
        );

        check(
            vkEndCommandBuffer(commandBuffer),
            "End command buffer"
        );
    }
}

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

            arvek::Swapchain swapchain(
                physicalDevice,
                device,
                surface,
                engine.window()
            );

            arvek::CommandPool commandPool(device, physicalDevice, 1);

            arvek::Fence submissionFence(device);
            arvek::Semaphore imageAvailable(device);
            arvek::Semaphore renderFinished(device);

            const VkCommandBuffer commandBuffer = commandPool.commandBuffers().at(0);

            // 1. Acquire an image.
            std::uint32_t imageIndex = 0;

            const VkResult acquireResult = vkAcquireNextImageKHR(
                device.handle(),
                swapchain.handle(),
                UINT64_MAX,
                imageAvailable.handle(),
                VK_NULL_HANDLE,
                &imageIndex
            );

            if (acquireResult == VK_ERROR_OUT_OF_DATE_KHR)
            {
                throw std::runtime_error(
                    "Swapchain became out of date during acquisition!"
                );
            }

            if (acquireResult != VK_SUCCESS &&
                acquireResult != VK_SUBOPTIMAL_KHR)
            {
                check(acquireResult, "Acquire swapchain image");
            }

            if (imageIndex >= swapchain.images().size())
            {
                throw std::runtime_error("Acquired invalid swapchain image index!");
            }

            // 2. Record a clear for the acquired image.
            recordClearCommands(
                commandBuffer,
                swapchain.images().at(imageIndex)
            );

            // 3. Submit: wait for acquisition, execute commands,
            // then signal render completion.
            const VkSemaphore waitSemaphore = imageAvailable.handle();

            const VkSemaphore signalSemaphore = renderFinished.handle();

            const VkPipelineStageFlags waitStage = VK_PIPELINE_STAGE_TRANSFER_BIT;

            VkSubmitInfo submitInfo{};
            submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
            submitInfo.waitSemaphoreCount = 1;
            submitInfo.pWaitSemaphores = &waitSemaphore;
            submitInfo.pWaitDstStageMask = &waitStage;
            submitInfo.commandBufferCount = 1;
            submitInfo.pCommandBuffers = &commandBuffer;
            submitInfo.signalSemaphoreCount = 1;
            submitInfo.pSignalSemaphores = &signalSemaphore;

            check(
                vkQueueSubmit(
                    device.graphicsQueue(),
                    1,
                    &submitInfo,
                    submissionFence.handle()
                ),
                "Submit clear commands"
            );

            // Wait for command execution to finish.
            submissionFence.wait();

            // 4. Queue the image for presentation.
            const VkSwapchainKHR swapchainHandle = swapchain.handle();

            VkPresentInfoKHR presentInfo{};
            presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
            presentInfo.waitSemaphoreCount = 1;
            presentInfo.pWaitSemaphores = &signalSemaphore;
            presentInfo.swapchainCount = 1;
            presentInfo.pSwapchains = &swapchainHandle;
            presentInfo.pImageIndices = &imageIndex;

            const VkResult presentResult = vkQueuePresentKHR(
                device.presentationQueue(),
                &presentInfo
            );

            if (presentResult == VK_ERROR_OUT_OF_DATE_KHR)
            {
                throw std::runtime_error(
                    "Swapchain became out of date during presentation!"
                );
            }

            if (presentResult != VK_SUCCESS &&
                presentResult != VK_SUBOPTIMAL_KHR)
            {
                check(presentResult, "Present swapchain image");
            }

            ARV_LOG_INFO("Vulkan clear and presentation submitted successfully.");

            // 5. Keep the window open for visual inspection.
            while (!engine.window().shouldClose())
            {
                engine.window().waitEvents();
            }

            // Simple prototype shutdown.
            check(
                vkDeviceWaitIdle(device.handle()),
                "Wait for Vulkan device idle"
            );
        }

        engine.shutdown();
    }
    catch (const std::exception& e)
    {
        ARV_LOG_ERROR(
            std::string("Vulkan presentation test failed: ") +
            e.what()
        );

        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}