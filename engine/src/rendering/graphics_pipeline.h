#pragma once

#include <vulkan/vulkan.h>

namespace arvek
{
    class Device;
    class PipelineLayout;
    class RenderPass;
    class ShaderModule;

    class GraphicsPipeline final
    {
    public:
        GraphicsPipeline(const Device& device,
            const RenderPass& renderPass,
            const PipelineLayout& layout,
            const ShaderModule& vertexShader,
            const ShaderModule& fragmentShader);

        ~GraphicsPipeline();

        GraphicsPipeline(const GraphicsPipeline&) = delete;
        GraphicsPipeline& operator=(const GraphicsPipeline&) = delete;
        GraphicsPipeline(GraphicsPipeline&&) = delete;
        GraphicsPipeline& operator=(GraphicsPipeline&&) = delete;

        [[nodiscard]]
        VkPipeline handle() const noexcept;

    private:
        VkDevice device_{ VK_NULL_HANDLE };
        VkPipeline pipeline_{ VK_NULL_HANDLE };
    };
}