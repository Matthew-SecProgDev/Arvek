#pragma once

#include <memory>
#include <vector>

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

    class VulkanRenderer final
    {
    public:
        explicit VulkanRenderer(Window& window);
        ~VulkanRenderer();

        VulkanRenderer(const VulkanRenderer&) = delete;
        VulkanRenderer& operator=(const VulkanRenderer&) = delete;
        VulkanRenderer(VulkanRenderer&&) = delete;
        VulkanRenderer& operator=(VulkanRenderer&&) = delete;

    private:
        Window& window_;

        std::unique_ptr<Instance> instance_;
        std::unique_ptr<Surface> surface_;
        std::unique_ptr<PhysicalDevice> physicalDevice_;
        std::unique_ptr<Device> device_;

        // Swapchain-dependent resources.
        std::unique_ptr<Swapchain> swapchain_;
        std::unique_ptr<RenderPass> renderPass_;

        std::vector<std::unique_ptr<Framebuffer>> framebuffers_;
    };
}