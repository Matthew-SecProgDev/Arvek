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
#include <cstdint>
#include <memory>
#include <vector>

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

            arvek::CommandPool commandPool(device, physicalDevice, 1);

            arvek::Fence inFlightFence(device);
            arvek::Semaphore imageAvailable(device);

            // Presentation semaphores belong to swapchain images.
            std::vector<std::unique_ptr<arvek::Semaphore>> renderFinished;

            renderFinished.reserve(swapchain.images().size());

            for (std::size_t i = 0; i < swapchain.images().size(); ++i)
            {
                renderFinished.push_back(
                    std::make_unique<arvek::Semaphore>(device)
                );
            }

            const VkCommandBuffer commandBuffer = commandPool.commandBuffers().at(0);

            auto& window = engine.window();

            bool submissionPending = false;
            std::uint64_t frameCount = 0;

            while (!window.shouldClose())
            {
                window.pollEvents();

                if (window.shouldClose())
                {
                    break;
                }

                // The previous graphics submission must complete
                // before reusing its command pool, acquisition
                // semaphore, or submission fence.
                if (submissionPending)
                {
                    inFlightFence.wait();
                    submissionPending = false;
                }

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
                    ARV_LOG_INFO("Swapchain is out of date. "
                        "Recreation will be added next."
                    );
                    break;
                }

                if (acquireResult != VK_SUCCESS &&
                    acquireResult != VK_SUBOPTIMAL_KHR)
                {
                    check(acquireResult, "Acquire image");
                }

                if (imageIndex >= swapchain.images().size())
                {
                    throw std::runtime_error("Invalid swapchain image index!");
                }

                commandPool.reset();

                recordClearCommands(
                    commandBuffer,
                    swapchain.images().at(imageIndex)
                );

                const VkSemaphore waitSemaphore = imageAvailable.handle();

                const VkSemaphore signalSemaphore = renderFinished.at(imageIndex)->handle();

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

                // Reset only after acquiring an image successfully.
                inFlightFence.reset();

                check(
                    vkQueueSubmit(
                        device.graphicsQueue(),
                        1,
                        &submitInfo,
                        inFlightFence.handle()
                    ),
                    "Submit clear commands"
                );

                submissionPending = true;

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
                    ARV_LOG_INFO("Swapchain became out of date "
                        "during presentation."
                    );
                    break;
                }

                if (presentResult != VK_SUCCESS &&
                    presentResult != VK_SUBOPTIMAL_KHR)
                {
                    check(presentResult, "Present image");
                }

                ++frameCount;

                if (frameCount % 120 == 0)
                {
                    ARV_LOG_INFO("Frames presented: " + std::to_string(frameCount));
                }

                if (acquireResult == VK_SUBOPTIMAL_KHR ||
                    presentResult == VK_SUBOPTIMAL_KHR)
                {
                    ARV_LOG_INFO("Swapchain is suboptimal. "
                        "Recreation will be added next."
                    );
                    break;
                }
            }

            check(
                vkDeviceWaitIdle(device.handle()),
                "Wait for device idle"
            );

            ARV_LOG_INFO("Frame loop finished. Frames: " +
                std::to_string(frameCount)
            );
        }

        engine.shutdown();
    }
    catch (const std::exception& e)
    {
        ARV_LOG_ERROR(std::string("Frame loop failed: ") + e.what());

        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}