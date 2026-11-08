#pragma once

#include <vulkan/vulkan.h>

namespace arvek
{
    class Device;

    class PipelineLayout final
    {
    public:
        explicit PipelineLayout(const Device& device);
        ~PipelineLayout();

        PipelineLayout(const PipelineLayout&) = delete;
        PipelineLayout& operator=(const PipelineLayout&) = delete;
        PipelineLayout(PipelineLayout&&) = delete;
        PipelineLayout& operator=(PipelineLayout&&) = delete;

        [[nodiscard]]
        VkPipelineLayout handle() const noexcept;

    private:
        VkDevice device_{ VK_NULL_HANDLE };
        VkPipelineLayout layout_{ VK_NULL_HANDLE };
    };
}