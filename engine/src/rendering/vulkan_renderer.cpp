#include "vulkan_renderer.h"

#include "device.h"
#include "framebuffer.h"
#include "instance.h"
#include "physical_device.h"
#include "render_pass.h"
#include "surface.h"
#include "swapchain.h"

#include <memory>
#include <stdexcept>

namespace arvek
{
    VulkanRenderer::VulkanRenderer(Window& window) : window_(window)
    {
        instance_ = std::make_unique<Instance>();

        surface_ = std::make_unique<Surface>(window_, *instance_);

        physicalDevice_ = std::make_unique<PhysicalDevice>(*instance_, *surface_);

        device_ = std::make_unique<Device>(*physicalDevice_);

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
    }

    VulkanRenderer::~VulkanRenderer() = default;
}