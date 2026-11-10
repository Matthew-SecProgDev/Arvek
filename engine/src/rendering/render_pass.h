#pragma once

#include <vulkan/vulkan.h>

namespace arvek
{
    class Device;

    class RenderPass final
    {
    public:
        RenderPass(const Device& device, VkFormat colorFormat);
        ~RenderPass();

        RenderPass(const RenderPass&) = delete;
        RenderPass& operator=(const RenderPass&) = delete;
        RenderPass(RenderPass&&) = delete;
        RenderPass& operator=(RenderPass&&) = delete;

        [[nodiscard]]
        VkRenderPass handle() const noexcept;

    private:
        VkDevice device_{ VK_NULL_HANDLE };
        VkRenderPass renderPass_{ VK_NULL_HANDLE };
    };
}