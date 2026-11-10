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
#include "window_test_fixture.h"

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

    std::vector<std::unique_ptr<arvek::Semaphore>> createRenderFinishedSemaphores(
        const arvek::Device& device,
        std::size_t imageCount)
    {
        std::vector<std::unique_ptr<arvek::Semaphore>> semaphores;

        semaphores.reserve(imageCount);

        for (std::size_t i = 0; i < imageCount; ++i)
        {
            semaphores.push_back(std::make_unique<arvek::Semaphore>(device));
        }

        return semaphores;
    }
}

int main()
{
    try
    {
        arvek::WindowTestFixture fixture;

        {
            arvek::Instance instance(true);
            arvek::Surface surface(fixture.window(), instance);

            arvek::PhysicalDevice physicalDevice(instance, surface);

            arvek::Device device(physicalDevice);

            auto swapchain = std::make_unique<arvek::Swapchain>(
                physicalDevice,
                device,
                surface,
                fixture.window()
            );

            arvek::CommandPool commandPool(device, physicalDevice, 1);

            arvek::Fence inFlightFence(device);
            arvek::Semaphore imageAvailable(device);

            // Presentation semaphores belong to swapchain images.
            auto renderFinished = createRenderFinishedSemaphores(
                device,
                swapchain->images().size()
            );

            const VkCommandBuffer commandBuffer = commandPool.commandBuffers().at(0);

            auto& window = fixture.window();

            auto recreateSwapchain = [&]() -> bool
                {
                    // A minimized window can have a zero-sized framebuffer.
                    // Wait until it becomes drawable again.
                    auto extent = window.framebufferExtent();

                    while (extent.width == 0 || extent.height == 0)
                    {
                        if (window.shouldClose())
                        {
                            return false;
                        }

                        window.waitEvents();
                        extent = window.framebufferExtent();
                    }

                    // Stop GPU submissions before replacing resources.
                    check(
                        vkDeviceWaitIdle(device.handle()),
                        "Wait for device idle before swapchain recreation"
                    );

                    // Release existing swapchain resources first.
                    renderFinished.clear();
                    swapchain.reset();

                    // The native window no longer has our old swapchain.
                    swapchain = std::make_unique<arvek::Swapchain>(
                        physicalDevice,
                        device,
                        surface,
                        window
                    );

                    renderFinished = createRenderFinishedSemaphores(
                        device,
                        swapchain->images().size()
                    );

                    window.acknowledgeResize();

                    ARV_LOG_INFO("Swapchain recreated successfully.");

                    return true;
                };

            bool submissionPending = false;
            std::uint64_t frameCount = 0;
            bool recreateRequested = false;

            while (!window.shouldClose())
            {
                window.pollEvents();

                if (window.wasResized() || recreateRequested)
                {
                    if (!recreateSwapchain())
                    {
                        break;
                    }

                    recreateRequested = false;
                    submissionPending = false;

                    // Begin the next iteration with the new swapchain.
                    continue;
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
                        swapchain->handle(),
                        UINT64_MAX,
                        imageAvailable.handle(),
                        VK_NULL_HANDLE,
                        &imageIndex
                );

                if (acquireResult == VK_ERROR_OUT_OF_DATE_KHR)
                {
                    recreateRequested = true;
                    continue;
                }

                if (acquireResult != VK_SUCCESS &&
                    acquireResult != VK_SUBOPTIMAL_KHR)
                {
                    check(acquireResult, "Acquire swapchain image");
                }

                if (imageIndex >= swapchain->images().size())
                {
                    throw std::runtime_error("Invalid swapchain image index!");
                }

                commandPool.reset();

                recordClearCommands(
                    commandBuffer,
                    swapchain->images().at(imageIndex)
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

                const VkSwapchainKHR swapchainHandle = swapchain->handle();

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

                if (presentResult != VK_SUCCESS &&
                    presentResult != VK_SUBOPTIMAL_KHR &&
                    presentResult != VK_ERROR_OUT_OF_DATE_KHR)
                {
                    check(presentResult, "Present swapchain image");
                }

                if (acquireResult == VK_SUBOPTIMAL_KHR ||
                    presentResult == VK_SUBOPTIMAL_KHR ||
                    presentResult == VK_ERROR_OUT_OF_DATE_KHR ||
                    window.wasResized())
                {
                    recreateRequested = true;
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
    }
    catch (const std::exception& e)
    {
        ARV_LOG_ERROR(std::string("Frame loop failed: ") + e.what());

        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}