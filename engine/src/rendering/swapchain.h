#pragma once

#include <vulkan/vulkan.h>

#include <vector>

namespace arvek
{
    class PhysicalDevice;
    class Device;
    class Surface;
    class Window;

    class Swapchain final
    {
    public:
        Swapchain(const PhysicalDevice& physicalDevice,
            const Device& device,
            const Surface& surface,
            const Window& window);

        ~Swapchain();

        Swapchain(const Swapchain&) = delete;
        Swapchain& operator=(const Swapchain&) = delete;
        Swapchain(Swapchain&&) = delete;
        Swapchain& operator=(Swapchain&&) = delete;

        [[nodiscard]]
        VkSwapchainKHR handle() const noexcept;

        [[nodiscard]]
        VkFormat imageFormat() const noexcept;

        [[nodiscard]]
        VkExtent2D extent() const noexcept;

        [[nodiscard]]
        const std::vector<VkImage>& images() const noexcept;

        [[nodiscard]]
        const std::vector<VkImageView>& imageViews() const noexcept;

    private:
        void destroy() noexcept;

        VkDevice device_{ VK_NULL_HANDLE };
        VkSwapchainKHR swapchain_{ VK_NULL_HANDLE };

        VkFormat imageFormat_{ VK_FORMAT_UNDEFINED };
        VkExtent2D extent_{};

        std::vector<VkImage> images_;
        std::vector<VkImageView> imageViews_;
    };
}