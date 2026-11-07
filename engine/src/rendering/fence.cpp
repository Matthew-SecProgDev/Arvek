#include "fence.h"

#include "device.h"

#include <stdexcept>
#include <string>

namespace arvek
{
    Fence::Fence(const Device& device) : device_(device.handle())
    {
        if (device_ == VK_NULL_HANDLE)
        {
            throw std::invalid_argument(
                "Fence requires a valid Vulkan device!"
            );
        }

        VkFenceCreateInfo createInfo{};
        createInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
        createInfo.flags = 0; // Initially unsignaled.

        const VkResult result = vkCreateFence(
            device_,
            &createInfo,
            nullptr,
            &fence_
        );

        if (result != VK_SUCCESS)
        {
            throw std::runtime_error(
                "Failed to create Vulkan fence. VkResult: " +
                std::to_string(static_cast<int>(result))
            );
        }
    }

    Fence::~Fence()
    {
        if (fence_ != VK_NULL_HANDLE)
        {
            vkDestroyFence(device_, fence_, nullptr);
        }
    }

    VkFence Fence::handle() const noexcept
    {
        return fence_;
    }

    void Fence::wait(std::uint64_t timeout) const
    {
        const VkResult result = vkWaitForFences(
            device_,
            1,
            &fence_,
            VK_TRUE,
            timeout
        );

        if (result == VK_TIMEOUT)
        {
            throw std::runtime_error(
                "Timed out waiting for Vulkan fence!"
            );
        }

        if (result != VK_SUCCESS)
        {
            throw std::runtime_error(
                "Failed to wait for Vulkan fence. VkResult: " +
                std::to_string(static_cast<int>(result))
            );
        }
    }

    void Fence::reset()
    {
        const VkResult result = vkResetFences(
            device_,
            1,
            &fence_
        );

        if (result != VK_SUCCESS)
        {
            throw std::runtime_error(
                "Failed to reset Vulkan fence. VkResult: " +
                std::to_string(static_cast<int>(result))
            );
        }
    }
}