#include "semaphore.h"

#include "device.h"

#include <stdexcept>
#include <string>

namespace arvek
{
    Semaphore::Semaphore(const Device& device) : device_(device.handle())
    {
        if (device_ == VK_NULL_HANDLE)
        {
            throw std::invalid_argument(
                "Semaphore requires a valid Vulkan device!"
            );
        }

        VkSemaphoreCreateInfo createInfo{};
        createInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

        const VkResult result = vkCreateSemaphore(
            device_,
            &createInfo,
            nullptr,
            &semaphore_
        );

        if (result != VK_SUCCESS)
        {
            throw std::runtime_error(
                "Failed to create Vulkan semaphore. VkResult: " +
                std::to_string(static_cast<int>(result))
            );
        }
    }

    Semaphore::~Semaphore()
    {
        if (semaphore_ != VK_NULL_HANDLE)
        {
            vkDestroySemaphore(device_, semaphore_, nullptr);
        }
    }

    VkSemaphore Semaphore::handle() const noexcept
    {
        return semaphore_;
    }
}