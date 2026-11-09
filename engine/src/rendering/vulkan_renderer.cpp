#include "vulkan_renderer.h"

#include "device.h"
#include "framebuffer.h"
#include "graphics_pipeline.h"
#include "instance.h"
#include "physical_device.h"
#include "pipeline_layout.h"
#include "render_pass.h"
#include "shader_module.h"
#include "surface.h"
#include "swapchain.h"
#include "command_pool.h"
#include "fence.h"
#include "semaphore.h"
#include <arvek/core/window.h>
#include <arvek/core/logger.h>

#include <cstdint>
#include <string>
#include <memory>
#include <stdexcept>
#include <cstddef>
#include <vector>

namespace arvek
{
    VulkanRenderer::VulkanRenderer(Window& window,
        const std::vector<std::uint32_t>& vertexSpirv,
        const std::vector<std::uint32_t>& fragmentSpirv)
        : window_(window)
    {
        instance_ = std::make_unique<Instance>();

        surface_ = std::make_unique<Surface>(window_, *instance_);

        physicalDevice_ = std::make_unique<PhysicalDevice>(*instance_, *surface_);

        device_ = std::make_unique<Device>(*physicalDevice_);

        vertexShader_ = std::make_unique<ShaderModule>(
            *device_,
            vertexSpirv
        );

        fragmentShader_ = std::make_unique<ShaderModule>(
            *device_,
            fragmentSpirv
        );

        pipelineLayout_ = std::make_unique<PipelineLayout>(*device_);

        commandPool_ = std::make_unique<CommandPool>(
            *device_,
            *physicalDevice_,
            1
        );

        // GPU synchronization
        inFlightFence_ = std::make_unique<Fence>(*device_);
        imageAvailable_ = std::make_unique<Semaphore>(*device_);

        // Initial swapchain creation requires a drawable window.
        const auto extent = window_.framebufferExtent();
        if (extent.width == 0 || extent.height == 0)
        {
            throw std::runtime_error(
                "Cannot initialize VulkanRenderer with "
                "a zero-sized framebuffer!"
            );
        }

        createSwapchainResources();

        ARV_LOG_INFO("VulkanRenderer initialized.");
    }

    VulkanRenderer::~VulkanRenderer()
    {
        if (device_ && device_->handle() != VK_NULL_HANDLE)
        {
            // Member resources are destroyed after this body.
            (void)vkDeviceWaitIdle(device_->handle());
        }
    }

    bool VulkanRenderer::drawFrame()
    {
        // Handle a resize before acquiring another image
        if (window_.wasResized())
        {
            return recreateSwapchain();
        }

        // One frame in flight: wait before reusing
        // the command buffer and acquisition semaphore.
        if (submissionPending_)
        {
            inFlightFence_->wait();
            submissionPending_ = false;
        }

        std::uint32_t imageIndex = 0;

        const VkResult acquireResult = vkAcquireNextImageKHR(
            device_->handle(),
            swapchain_->handle(),
            UINT64_MAX,
            imageAvailable_->handle(),
            VK_NULL_HANDLE,
            &imageIndex
        );

        if (acquireResult == VK_ERROR_OUT_OF_DATE_KHR)
        {
            return recreateSwapchain();
        }

        if (acquireResult != VK_SUCCESS &&
            acquireResult != VK_SUBOPTIMAL_KHR)
        {
            throw std::runtime_error(
                "Failed to acquire swapchain image. VkResult: " +
                std::to_string(static_cast<int>(acquireResult))
            );
        }

        if (imageIndex >= framebuffers_.size() ||
            imageIndex >= renderFinished_.size())
        {
            throw std::runtime_error(
                "Acquired invalid swapchain image index!"
            );
        }

        // The previous submission has completed,
        // so the command pool can be reset.
        commandPool_->reset();

        recordCommands(imageIndex);

        const VkCommandBuffer commandBuffer = commandPool_->commandBuffers().at(0);

        const VkSemaphore waitSemaphore = imageAvailable_->handle();

        const VkSemaphore signalSemaphore = renderFinished_.at(imageIndex)->handle();

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

        // Reset only when we're ready to submit.
        inFlightFence_->reset();

        const VkResult submitResult = vkQueueSubmit(
            device_->graphicsQueue(),
            1,
            &submitInfo,
            inFlightFence_->handle()
        );

        if (submitResult != VK_SUCCESS)
        {
            throw std::runtime_error(
                "Failed to submit rendering commands. VkResult: " +
                std::to_string(static_cast<int>(submitResult))
            );
        }

        submissionPending_ = true;

        const VkSwapchainKHR swapchainHandle = swapchain_->handle();

        VkPresentInfoKHR presentInfo{};
        presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
        presentInfo.waitSemaphoreCount = 1;
        presentInfo.pWaitSemaphores = &signalSemaphore;
        presentInfo.swapchainCount = 1;
        presentInfo.pSwapchains = &swapchainHandle;
        presentInfo.pImageIndices = &imageIndex;

        const VkResult presentResult = vkQueuePresentKHR(
            device_->presentationQueue(),
            &presentInfo
        );

        if (presentResult != VK_SUCCESS &&
            presentResult != VK_SUBOPTIMAL_KHR &&
            presentResult != VK_ERROR_OUT_OF_DATE_KHR)
        {
            throw std::runtime_error(
                "Failed to present swapchain image. VkResult: " +
                std::to_string(static_cast<int>(presentResult))
            );
        }

        // Rebuild resources when the surface changes.
        if (acquireResult == VK_SUBOPTIMAL_KHR ||
            presentResult == VK_SUBOPTIMAL_KHR ||
            presentResult == VK_ERROR_OUT_OF_DATE_KHR ||
            window_.wasResized())
        {
            return recreateSwapchain();
        }

        return true;
    }

    void VulkanRenderer::recordCommands(std::uint32_t imageIndex)
    {
        // Command buffer belongs to this renderer.
        const VkCommandBuffer commandBuffer = commandPool_->commandBuffers().at(0);

        VkCommandBufferBeginInfo beginInfo{};
        beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
        beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

        if (vkBeginCommandBuffer(commandBuffer, &beginInfo) != VK_SUCCESS)
        {
            throw std::runtime_error(
                "Failed to begin Vulkan command buffer!"
            );
        }

        VkClearValue clearValue{};
        clearValue.color = { {
            0.04f, 0.06f, 0.12f, 1.0f
        } };

        VkRenderPassBeginInfo renderPassInfo{};
        renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
        renderPassInfo.renderPass = renderPass_->handle();
        renderPassInfo.framebuffer = framebuffers_.at(imageIndex)->handle();
        renderPassInfo.renderArea.offset = { 0, 0 };
        renderPassInfo.renderArea.extent = swapchain_->extent();
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
            graphicsPipeline_->handle()
        );

        const VkExtent2D extent = swapchain_->extent();

        // Dynamic viewport
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

        // Dynamic scissor
        VkRect2D scissor{};
        scissor.offset = { 0, 0 };
        scissor.extent = extent;

        vkCmdSetScissor(
            commandBuffer,
            0,
            1,
            &scissor
        );

        // Draw a triangle generated by gl_VertexIndex
        vkCmdDraw(
            commandBuffer,
            3,
            1,
            0,
            0
        );

        vkCmdEndRenderPass(commandBuffer);

        if (vkEndCommandBuffer(commandBuffer) != VK_SUCCESS)
        {
            throw std::runtime_error(
                "Failed to end Vulkan command buffer!"
            );
        }
    }

    void VulkanRenderer::createSwapchainResources()
    {
        swapchain_ = std::make_unique<Swapchain>(
            *physicalDevice_,
            *device_,
            *surface_,
            window_
        );

        renderPass_ = std::make_unique<RenderPass>(
            *device_,
            swapchain_->imageFormat()
        );

        graphicsPipeline_ = std::make_unique<GraphicsPipeline>(
            *device_,
            *renderPass_,
            *pipelineLayout_,
            *vertexShader_,
            *fragmentShader_
        );

        // One framebuffer per swapchain image view.
        const auto& imageViews = swapchain_->imageViews();

        if (imageViews.empty())
        {
            throw std::runtime_error("Swapchain has no image views!");
        }

        framebuffers_.reserve(imageViews.size());

        for (VkImageView imageView : imageViews)
        {
            framebuffers_.push_back(
                std::make_unique<Framebuffer>(
                    *device_,
                    *renderPass_,
                    imageView,
                    swapchain_->extent())
            );
        }

        // Presentation semaphores are associated with swapchain images
        renderFinished_.reserve(swapchain_->images().size());
        for (std::size_t i = 0; i < swapchain_->images().size(); ++i)
        {
            renderFinished_.push_back(
                std::make_unique<Semaphore>(*device_)
            );
        }
    }

    void VulkanRenderer::destroySwapchainResources()
    {
        // Framebuffers reference swapchain image views.
        framebuffers_.clear();

        // Graphics pipeline was created for this render pass.
        graphicsPipeline_.reset();

        renderPass_.reset();

        // Presentation semaphores belong to this swapchain generation.
        renderFinished_.clear();

        // Releases image views and the swapchain itself.
        swapchain_.reset();
    }

    bool VulkanRenderer::recreateSwapchain()
    {
        auto extent = window_.framebufferExtent();
        // Do not create a swapchain with a zero-sized framebuffer.
        while (extent.width == 0 || extent.height == 0)
        {
            if (window_.shouldClose())
            {
                return false;
            }

            window_.waitEvents();
            extent = window_.framebufferExtent();
        }

        if (window_.shouldClose())
        {
            return false;
        }

        // Currently the synchronization strategy in our prototype involves
        // waiting until the graphics device to become idle state.
        const VkResult result = vkDeviceWaitIdle(device_->handle());
        if (result != VK_SUCCESS)
        {
            throw std::runtime_error(
                "Failed to wait for device before "
                "swapchain recreation. VkResult: " +
                std::to_string(static_cast<int>(result))
            );
        }

        destroySwapchainResources();

        createSwapchainResources();

        // The old command buffer is no longer executing.
        submissionPending_ = false;

        window_.acknowledgeResize();

        ARV_LOG_INFO("VulkanRenderer swapchain recreated.");

        return true;
    }
}