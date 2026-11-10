#pragma once

#include <vulkan/vulkan.h>

namespace arvek
{
    class Device;
    class RenderPass;

    class Framebuffer final
    {
    public:
        Framebuffer(const Device& device,
            const RenderPass& renderPass,
            VkImageView colorImageView,
            VkExtent2D extent);

        ~Framebuffer();

        Framebuffer(const Framebuffer&) = delete;
        Framebuffer& operator=(const Framebuffer&) = delete;
        Framebuffer(Framebuffer&&) = delete;
        Framebuffer& operator=(Framebuffer&&) = delete;

        [[nodiscard]]
        VkFramebuffer handle() const noexcept;

    private:
        VkDevice device_{ VK_NULL_HANDLE };
        VkFramebuffer framebuffer_{ VK_NULL_HANDLE };
    };
}