#include "pipeline_layout.h"

#include "device.h"

#include <stdexcept>
#include <string>

namespace arvek
{
    PipelineLayout::PipelineLayout(const Device& device) : device_(device.handle())
    {
        if (device_ == VK_NULL_HANDLE)
        {
            throw std::invalid_argument(
                "PipelineLayout requires a valid device!"
            );
        }

        VkPipelineLayoutCreateInfo createInfo{};
        createInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
        createInfo.setLayoutCount = 0;
        createInfo.pSetLayouts = nullptr;
        createInfo.pushConstantRangeCount = 0;
        createInfo.pPushConstantRanges = nullptr;

        const VkResult result = vkCreatePipelineLayout(
            device_,
            &createInfo,
            nullptr,
            &layout_
        );

        if (result != VK_SUCCESS)
        {
            throw std::runtime_error(
                "Failed to create pipeline layout. VkResult: " +
                std::to_string(static_cast<int>(result))
            );
        }
    }

    PipelineLayout::~PipelineLayout()
    {
        if (layout_ != VK_NULL_HANDLE)
        {
            vkDestroyPipelineLayout(
                device_,
                layout_,
                nullptr
            );
        }
    }

    VkPipelineLayout PipelineLayout::handle() const noexcept
    {
        return layout_;
    }
}