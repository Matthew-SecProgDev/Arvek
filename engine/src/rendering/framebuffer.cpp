#include "framebuffer.h"

#include "device.h"
#include "render_pass.h"

#include <stdexcept>
#include <string>

namespace arvek
{
    Framebuffer::Framebuffer(const Device& device,
        const RenderPass& renderPass,
        VkImageView colorImageView,
        VkExtent2D extent) : device_(device.handle())
    {
        if (device_ == VK_NULL_HANDLE ||
            renderPass.handle() == VK_NULL_HANDLE ||
            colorImageView == VK_NULL_HANDLE)
        {
            throw std::invalid_argument(
                "Framebuffer requires valid Vulkan handles!"
            );
        }

        if (extent.width == 0 || extent.height == 0)
        {
            throw std::invalid_argument("Framebuffer extent must be nonzero!");
        }

        VkFramebufferCreateInfo createInfo{};
        createInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
        createInfo.renderPass = renderPass.handle();
        createInfo.attachmentCount = 1;
        createInfo.pAttachments = &colorImageView;
        createInfo.width = extent.width;
        createInfo.height = extent.height;
        createInfo.layers = 1;

        const VkResult result = vkCreateFramebuffer(
            device_,
            &createInfo,
            nullptr,
            &framebuffer_
        );

        if (result != VK_SUCCESS)
        {
            throw std::runtime_error(
                "Failed to create Vulkan framebuffer. VkResult: " +
                std::to_string(static_cast<int>(result))
            );
        }
    }

    Framebuffer::~Framebuffer()
    {
        if (framebuffer_ != VK_NULL_HANDLE)
        {
            vkDestroyFramebuffer(
                device_,
                framebuffer_,
                nullptr
            );
        }
    }

    VkFramebuffer Framebuffer::handle() const noexcept
    {
        return framebuffer_;
    }
}