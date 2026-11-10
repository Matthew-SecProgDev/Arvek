#include <arvek/core/engine.h>
#include <arvek/core/logger.h>
#include <arvek/core/window.h>
#include "command_pool.h"
#include "device.h"
#include "fence.h"
#include "framebuffer.h"
#include "graphics_pipeline.h"
#include "instance.h"
#include "physical_device.h"
#include "pipeline_layout.h"
#include "render_pass.h"
#include "semaphore.h"
#include "shader_module.h"
#include "surface.h"
#include "swapchain.h"

#include <cstdint>
#include <exception>
#include <fstream>
#include <memory>
#include <stdexcept>
#include <string>
#include <vector>
#include <cstdlib>

#ifndef ARVEK_TRIANGLE_VERTEX_SPV
#error "ARVEK_TRIANGLE_VERTEX_SPV must be defined by CMake"
#endif

#ifndef ARVEK_TRIANGLE_FRAGMENT_SPV
#error "ARVEK_TRIANGLE_FRAGMENT_SPV must be defined by CMake"
#endif

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

    std::vector<std::uint32_t> readSpirv(const std::string& path)
    {
        std::ifstream file(path, std::ios::binary | std::ios::ate);

        if (!file)
        {
            throw std::runtime_error("Cannot open SPIR-V file: " + path);
        }

        const std::streamoff size = file.tellg();

        if (size <= 0 || size % sizeof(std::uint32_t) != 0)
        {
            throw std::runtime_error("Invalid SPIR-V file size: " + path);
        }

        std::vector<std::uint32_t> code(static_cast<std::size_t>(size) / sizeof(std::uint32_t));

        file.seekg(0, std::ios::beg);

        if (!file.read(
            reinterpret_cast<char*>(code.data()),
            static_cast<std::streamsize>(size)))
        {
            throw std::runtime_error("Failed to read SPIR-V file: " + path);
        }

        return code;
    }

    void recordTriangleCommands(VkCommandBuffer commandBuffer,
        const arvek::RenderPass& renderPass,
        const arvek::Framebuffer& framebuffer,
        const arvek::GraphicsPipeline& pipeline,
        VkExtent2D extent)
    {
        VkCommandBufferBeginInfo beginInfo{};
        beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
        beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

        check(
            vkBeginCommandBuffer(commandBuffer, &beginInfo),
            "Begin triangle command buffer"
        );

        VkClearValue clearValue{};
        clearValue.color = { {
            0.04f, 0.06f, 0.12f, 1.0f
        } };

        VkRenderPassBeginInfo renderPassInfo{};
        renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
        renderPassInfo.renderPass = renderPass.handle();
        renderPassInfo.framebuffer = framebuffer.handle();
        renderPassInfo.renderArea.offset = { 0, 0 };
        renderPassInfo.renderArea.extent = extent;
        renderPassInfo.clearValueCount = 1;
        renderPassInfo.pClearValues = &clearValue;

        vkCmdBeginRenderPass(
            commandBuffer,
            &renderPassInfo,
            VK_SUBPASS_CONTENTS_INLINE
        );

        vkCmdBindPipeline(
            commandBuffer,
            VK_PIPELINE_BIND_POINT_GRAPHICS,
            pipeline.handle()
        );

        VkViewport viewport{};
        viewport.x = 0.0f;
        viewport.y = 0.0f;
        viewport.width = static_cast<float>(extent.width);
        viewport.height = static_cast<float>(extent.height);
        viewport.minDepth = 0.0f;
        viewport.maxDepth = 1.0f;

        vkCmdSetViewport(
            commandBuffer,
            0,
            1,
            &viewport
        );

        VkRect2D scissor{};
        scissor.offset = { 0, 0 };
        scissor.extent = extent;

        vkCmdSetScissor(
            commandBuffer,
            0,
            1,
            &scissor
        );

        vkCmdDraw(
            commandBuffer,
            3,
            1,
            0,
            0
        );

        vkCmdEndRenderPass(commandBuffer);

        check(
            vkEndCommandBuffer(commandBuffer),
            "End triangle command buffer"
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
        const auto vertexCode = readSpirv(ARVEK_TRIANGLE_VERTEX_SPV);
        const auto fragmentCode = readSpirv(ARVEK_TRIANGLE_FRAGMENT_SPV);

        arvek::Engine engine;
        engine.initialize(false);

        {
            arvek::Instance instance(true);

            arvek::Surface surface(engine.window(), instance);

            arvek::PhysicalDevice physicalDevice(instance, surface);

            arvek::Device device(physicalDevice);

            arvek::ShaderModule vertexShader(device, vertexCode);

            arvek::ShaderModule fragmentShader(device, fragmentCode);

            arvek::PipelineLayout pipelineLayout(device);

            arvek::CommandPool commandPool(device, physicalDevice, 1);

            arvek::Fence inFlightFence(device);
            arvek::Semaphore imageAvailable(device);

            const VkCommandBuffer commandBuffer = commandPool.commandBuffers().at(0);

            auto& window = engine.window();

            // Resources that depend on the swapchain.
            std::unique_ptr<arvek::Swapchain> swapchain;
            std::unique_ptr<arvek::RenderPass> renderPass;
            std::unique_ptr<arvek::GraphicsPipeline> pipeline;

            std::vector<std::unique_ptr<arvek::Framebuffer>> framebuffers;

            std::vector<std::unique_ptr<arvek::Semaphore>> renderFinished;

            auto destroySwapchainResources = [&]()
                {
                    framebuffers.clear();

                    pipeline.reset();
                    renderPass.reset();

                    renderFinished.clear();
                    swapchain.reset();
                };

            auto createSwapchainResources = [&]()
                {
                    swapchain = std::make_unique<arvek::Swapchain>(
                        physicalDevice,
                        device,
                        surface,
                        window
                    );

                    renderPass = std::make_unique<arvek::RenderPass>(
                        device,
                        swapchain->imageFormat()
                    );

                    pipeline = std::make_unique<arvek::GraphicsPipeline>(
                        device,
                        *renderPass,
                        pipelineLayout,
                        vertexShader,
                        fragmentShader
                    );

                    framebuffers.reserve(swapchain->imageViews().size());

                    for (VkImageView imageView : swapchain->imageViews())
                    {
                        framebuffers.push_back(
                            std::make_unique<arvek::Framebuffer>(
                                device,
                                *renderPass,
                                imageView,
                                swapchain->extent())
                        );
                    }

                    renderFinished = createRenderFinishedSemaphores(
                        device,
                        swapchain->images().size()
                    );
                };

            auto waitForDrawableWindow = [&]() -> bool
                {
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

                    return !window.shouldClose();
                };

            auto recreateSwapchain = [&]() -> bool
                {
                    if (!waitForDrawableWindow())
                    {
                        return false;
                    }

                    check(
                        vkDeviceWaitIdle(device.handle()),
                        "Wait for device idle before recreation"
                    );

                    destroySwapchainResources();

                    createSwapchainResources();

                    window.acknowledgeResize();

                    ARV_LOG_INFO("Triangle swapchain recreated.");

                    return true;
                };

            createSwapchainResources();

            bool submissionPending = false;
            bool recreateRequested = false;

            std::uint64_t frameCount = 0;

            ARV_LOG_INFO("Arvek Vulkan triangle renderer started.");

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

                    continue;
                }

                // Wait before reusing the command buffer
                // and image-acquisition semaphore.
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
                    check(
                        acquireResult,
                        "Acquire triangle image"
                    );
                }

                if (imageIndex >= framebuffers.size() ||
                    imageIndex >= renderFinished.size())
                {
                    throw std::runtime_error("Invalid swapchain image index!");
                }

                commandPool.reset();

                recordTriangleCommands(
                    commandBuffer,
                    *renderPass,
                    *framebuffers.at(imageIndex),
                    *pipeline,
                    swapchain->extent()
                );

                const VkSemaphore waitSemaphore = imageAvailable.handle();

                const VkSemaphore signalSemaphore = renderFinished.at(imageIndex)->handle();

                const VkPipelineStageFlags waitStage = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;

                VkSubmitInfo submitInfo{};
                submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
                submitInfo.waitSemaphoreCount = 1;
                submitInfo.pWaitSemaphores = &waitSemaphore;
                submitInfo.pWaitDstStageMask = &waitStage;
                submitInfo.commandBufferCount = 1;
                submitInfo.pCommandBuffers = &commandBuffer;
                submitInfo.signalSemaphoreCount = 1;
                submitInfo.pSignalSemaphores = &signalSemaphore;

                // Reset only when a submission is about to happen.
                inFlightFence.reset();

                check(
                    vkQueueSubmit(
                        device.graphicsQueue(),
                        1,
                        &submitInfo,
                        inFlightFence.handle()
                    ),
                    "Submit triangle commands"
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
                    check(
                        presentResult,
                        "Present triangle image"
                    );
                }

                ++frameCount;

                if (frameCount % 120 == 0)
                {
                    ARV_LOG_INFO("Frames submitted: " + std::to_string(frameCount));
                }

                if (acquireResult == VK_SUBOPTIMAL_KHR ||
                    presentResult == VK_SUBOPTIMAL_KHR ||
                    presentResult == VK_ERROR_OUT_OF_DATE_KHR ||
                    window.wasResized())
                {
                    recreateRequested = true;
                }
            }

            // Normal shutdown: finish device work before cleanup.
            check(
                vkDeviceWaitIdle(device.handle()),
                "Wait for Vulkan device idle"
            );

            destroySwapchainResources();

            ARV_LOG_INFO("Triangle renderer stopped. Frames: " 
                + std::to_string(frameCount)
            );
        }

        engine.shutdown();
    }
    catch (const std::exception& e)
    {
        ARV_LOG_ERROR(
            std::string("Triangle rendering failed: ") +
            e.what()
        );

        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}