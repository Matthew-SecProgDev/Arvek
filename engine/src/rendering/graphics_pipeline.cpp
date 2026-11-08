#include "graphics_pipeline.h"

#include "device.h"
#include "pipeline_layout.h"
#include "render_pass.h"
#include "shader_module.h"

#include <array>
#include <stdexcept>
#include <string>

namespace arvek
{
    GraphicsPipeline::GraphicsPipeline(const Device& device,
        const RenderPass& renderPass,
        const PipelineLayout& layout,
        const ShaderModule& vertexShader,
        const ShaderModule& fragmentShader)
        : device_(device.handle())
    {
        if (device_ == VK_NULL_HANDLE ||
            renderPass.handle() == VK_NULL_HANDLE ||
            layout.handle() == VK_NULL_HANDLE ||
            vertexShader.handle() == VK_NULL_HANDLE ||
            fragmentShader.handle() == VK_NULL_HANDLE)
        {
            throw std::invalid_argument(
                "GraphicsPipeline requires valid Vulkan handles!"
            );
        }

        VkPipelineShaderStageCreateInfo vertexStage{};
        vertexStage.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
        vertexStage.stage = VK_SHADER_STAGE_VERTEX_BIT;
        vertexStage.module = vertexShader.handle();
        vertexStage.pName = "main";

        VkPipelineShaderStageCreateInfo fragmentStage{};
        fragmentStage.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
        fragmentStage.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
        fragmentStage.module = fragmentShader.handle();
        fragmentStage.pName = "main";

        const std::array<VkPipelineShaderStageCreateInfo, 2> stages{
            vertexStage,
            fragmentStage
        };

        // No vertex buffers: the vertex shader uses gl_VertexIndex.
        VkPipelineVertexInputStateCreateInfo vertexInput{};
        vertexInput.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;

        VkPipelineInputAssemblyStateCreateInfo inputAssembly{};
        inputAssembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
        inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
        inputAssembly.primitiveRestartEnable = VK_FALSE;

        // Values will be set during command recording.
        VkPipelineViewportStateCreateInfo viewportState{};
        viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
        viewportState.viewportCount = 1;
        viewportState.scissorCount = 1;

        VkPipelineRasterizationStateCreateInfo rasterization{};
        rasterization.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
        rasterization.depthClampEnable = VK_FALSE;
        rasterization.rasterizerDiscardEnable = VK_FALSE;
        rasterization.polygonMode = VK_POLYGON_MODE_FILL;
        rasterization.cullMode = VK_CULL_MODE_NONE;
        rasterization.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
        rasterization.depthBiasEnable = VK_FALSE;
        rasterization.lineWidth = 1.0f;

        VkPipelineMultisampleStateCreateInfo multisampling{};
        multisampling.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
        multisampling.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
        multisampling.sampleShadingEnable = VK_FALSE;

        VkPipelineColorBlendAttachmentState colorAttachment{};
        colorAttachment.blendEnable = VK_FALSE;
        colorAttachment.colorWriteMask =
            VK_COLOR_COMPONENT_R_BIT |
            VK_COLOR_COMPONENT_G_BIT |
            VK_COLOR_COMPONENT_B_BIT |
            VK_COLOR_COMPONENT_A_BIT;

        VkPipelineColorBlendStateCreateInfo colorBlending{};
        colorBlending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
        colorBlending.logicOpEnable = VK_FALSE;
        colorBlending.attachmentCount = 1;
        colorBlending.pAttachments = &colorAttachment;

        const std::array<VkDynamicState, 2> dynamicStates{
            VK_DYNAMIC_STATE_VIEWPORT,
            VK_DYNAMIC_STATE_SCISSOR
        };

        VkPipelineDynamicStateCreateInfo dynamicState{};
        dynamicState.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
        dynamicState.dynamicStateCount = static_cast<std::uint32_t>(dynamicStates.size());
        dynamicState.pDynamicStates = dynamicStates.data();

        VkGraphicsPipelineCreateInfo createInfo{};
        createInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
        createInfo.stageCount = static_cast<std::uint32_t>(stages.size());
        createInfo.pStages = stages.data();
        createInfo.pVertexInputState = &vertexInput;
        createInfo.pInputAssemblyState = &inputAssembly;
        createInfo.pViewportState = &viewportState;
        createInfo.pRasterizationState = &rasterization;
        createInfo.pMultisampleState = &multisampling;
        createInfo.pColorBlendState = &colorBlending;
        createInfo.pDynamicState = &dynamicState;
        createInfo.pDepthStencilState = nullptr; // No depth attachment in our render pass.
        createInfo.layout = layout.handle();
        createInfo.renderPass = renderPass.handle();
        createInfo.subpass = 0;
        createInfo.basePipelineHandle = VK_NULL_HANDLE;
        createInfo.basePipelineIndex = -1;

        const VkResult result = vkCreateGraphicsPipelines(
            device_,
            VK_NULL_HANDLE,
            1,
            &createInfo,
            nullptr,
            &pipeline_
        );

        if (result != VK_SUCCESS)
        {
            if (pipeline_ != VK_NULL_HANDLE)
            {
                vkDestroyPipeline(device_, pipeline_, nullptr);
            }

            throw std::runtime_error(
                "Failed to create graphics pipeline. VkResult: " +
                std::to_string(static_cast<int>(result))
            );
        }
    }

    GraphicsPipeline::~GraphicsPipeline()
    {
        if (pipeline_ != VK_NULL_HANDLE)
        {
            vkDestroyPipeline(
                device_,
                pipeline_,
                nullptr
            );
        }
    }

    VkPipeline GraphicsPipeline::handle() const noexcept
    {
        return pipeline_;
    }
}