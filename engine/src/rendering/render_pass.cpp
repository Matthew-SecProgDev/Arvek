#include "render_pass.h"

#include "device.h"

#include <stdexcept>
#include <string>

namespace arvek
{
    RenderPass::RenderPass(const Device& device,
        VkFormat colorFormat) : device_(device.handle())
    {
        if (device_ == VK_NULL_HANDLE)
        {
            throw std::invalid_argument(
                "RenderPass requires a valid Vulkan device!"
            );
        }

        if (colorFormat == VK_FORMAT_UNDEFINED)
        {
            throw std::invalid_argument(
                "RenderPass requires a valid color format!"
            );
        }

        VkAttachmentDescription colorAttachment{};
        colorAttachment.format = colorFormat;
        colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
        colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
        colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
        colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
        colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
        colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        colorAttachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

        VkAttachmentReference colorReference{};
        colorReference.attachment = 0;
        colorReference.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

        VkSubpassDescription subpass{};
        subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
        subpass.colorAttachmentCount = 1;
        subpass.pColorAttachments = &colorReference;

        VkSubpassDependency dependency{};
        dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
        dependency.dstSubpass = 0;
        dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
        dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
        dependency.srcAccessMask = 0;
        dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

        VkRenderPassCreateInfo createInfo{};
        createInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
        createInfo.attachmentCount = 1;
        createInfo.pAttachments = &colorAttachment;
        createInfo.subpassCount = 1;
        createInfo.pSubpasses = &subpass;
        createInfo.dependencyCount = 1;
        createInfo.pDependencies = &dependency;

        const VkResult result = vkCreateRenderPass(
            device_,
            &createInfo,
            nullptr,
            &renderPass_
        );

        if (result != VK_SUCCESS)
        {
            throw std::runtime_error(
                "Failed to create Vulkan render pass. VkResult: " +
                std::to_string(static_cast<int>(result))
            );
        }
    }

    RenderPass::~RenderPass()
    {
        if (renderPass_ != VK_NULL_HANDLE)
        {
            vkDestroyRenderPass(
                device_,
                renderPass_,
                nullptr
            );
        }
    }

    VkRenderPass RenderPass::handle() const noexcept
    {
        return renderPass_;
    }
}