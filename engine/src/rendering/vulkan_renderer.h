#pragma once

#include <memory>
#include <vector>
#include <cstdint>

namespace arvek
{
    class Window;
    class Instance;
    class Surface;
    class PhysicalDevice;
    class Device;
    class Swapchain;
    class RenderPass;
    class Framebuffer;
    class ShaderModule;
    class PipelineLayout;
    class GraphicsPipeline;
    class CommandPool;
    class Fence;
    class Semaphore;

    class VulkanRenderer final
    {
    public:
        explicit VulkanRenderer(Window& window);
        VulkanRenderer(
            Window& window,
            const std::vector<std::uint32_t>& vertexSpirv,
            const std::vector<std::uint32_t>& fragmentSpirv);
        ~VulkanRenderer();

        VulkanRenderer(const VulkanRenderer&) = delete;
        VulkanRenderer& operator=(const VulkanRenderer&) = delete;
        VulkanRenderer(VulkanRenderer&&) = delete;
        VulkanRenderer& operator=(VulkanRenderer&&) = delete;

        // Returns true which means the renderer is still operational, 
        // including when a frame was skipped for recreation, otherwise
        // rendering should stop because the window is closing.
        [[nodiscard]]
        bool drawFrame();

    private:
        Window& window_;

        std::unique_ptr<Instance> instance_;
        std::unique_ptr<Surface> surface_;
        std::unique_ptr<PhysicalDevice> physicalDevice_;
        std::unique_ptr<Device> device_;

        // Swapchain-dependent resources
        std::unique_ptr<Swapchain> swapchain_;
        std::unique_ptr<RenderPass> renderPass_;

        std::vector<std::unique_ptr<Framebuffer>> framebuffers_;

        // Shader and graphics pipeline resources
        std::unique_ptr<ShaderModule> vertexShader_;
        std::unique_ptr<ShaderModule> fragmentShader_;
        std::unique_ptr<PipelineLayout> pipelineLayout_;
        std::unique_ptr<GraphicsPipeline> graphicsPipeline_;

        std::unique_ptr<CommandPool> commandPool_;
        std::unique_ptr<Fence> inFlightFence_;
        std::unique_ptr<Semaphore> imageAvailable_;

        std::vector<std::unique_ptr<Semaphore>> renderFinished_;

        bool submissionPending_{ false };

        void recordCommands(std::uint32_t imageIndex);

        void createSwapchainResources();
        void destroySwapchainResources();

        [[nodiscard]]
        bool recreateSwapchain();

        void createTriangleResources(
            const std::vector<std::uint32_t>& vertexSpirv,
            const std::vector<std::uint32_t>& fragmentSpirv);
    };
}