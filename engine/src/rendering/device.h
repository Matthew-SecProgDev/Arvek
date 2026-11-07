#pragma once

#include <vulkan/vulkan.h>

namespace arvek
{
    class PhysicalDevice;

    class Device final
    {
    public:
        explicit Device(const PhysicalDevice& physicalDevice);
        ~Device();

        Device(const Device&) = delete;
        Device& operator=(const Device&) = delete;
        Device(Device&&) = delete;
        Device& operator=(Device&&) = delete;

        [[nodiscard]]
        VkDevice handle() const noexcept;

        [[nodiscard]]
        VkQueue graphicsQueue() const noexcept;

        [[nodiscard]]
        VkQueue presentationQueue() const noexcept;

    private:
        VkDevice device_{ VK_NULL_HANDLE };
        VkQueue graphicsQueue_{ VK_NULL_HANDLE };
        VkQueue presentationQueue_{ VK_NULL_HANDLE };
    };
}