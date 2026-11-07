#pragma once

#include <vulkan/vulkan.h>

#include <cstdint>
#include <vector>

namespace arvek
{
    class Device;
    class PhysicalDevice;

    class CommandPool final
    {
    public:
        CommandPool(const Device& device,
            const PhysicalDevice& physicalDevice,
            std::uint32_t commandBufferCount = 2);

        ~CommandPool();

        CommandPool(const CommandPool&) = delete;
        CommandPool& operator=(const CommandPool&) = delete;
        CommandPool(CommandPool&&) = delete;
        CommandPool& operator=(CommandPool&&) = delete;

        [[nodiscard]]
        VkCommandPool handle() const noexcept;

        [[nodiscard]]
        const std::vector<VkCommandBuffer>& commandBuffers() const noexcept;

        void reset();

    private:
        VkDevice device_{ VK_NULL_HANDLE };
        VkCommandPool pool_{ VK_NULL_HANDLE };
        std::vector<VkCommandBuffer> commandBuffers_;
    };
}