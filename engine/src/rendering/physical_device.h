#pragma once

#include <vulkan/vulkan.h>

#include <cstdint>

namespace arvek
{
    class Instance;
    class Surface;

    class PhysicalDevice final
    {
    public:
        PhysicalDevice(const Instance& instance, const Surface& surface);

        [[nodiscard]]
        VkPhysicalDevice handle() const noexcept;

        [[nodiscard]]
        std::uint32_t graphicsFamilyIndex() const noexcept;

        [[nodiscard]]
        std::uint32_t presentationFamilyIndex() const noexcept;

        [[nodiscard]]
        const VkPhysicalDeviceProperties& properties() const noexcept;

    private:
        VkPhysicalDevice physicalDevice_{ VK_NULL_HANDLE };

        std::uint32_t graphicsFamilyIndex_{ UINT32_MAX };
        std::uint32_t presentationFamilyIndex_{ UINT32_MAX };

        VkPhysicalDeviceProperties properties_{};
    };
}