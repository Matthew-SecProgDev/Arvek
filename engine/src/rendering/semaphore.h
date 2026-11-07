#pragma once

#include <vulkan/vulkan.h>

namespace arvek
{
    class Device;

    class Semaphore final
    {
    public:
        explicit Semaphore(const Device& device);
        ~Semaphore();

        Semaphore(const Semaphore&) = delete;
        Semaphore& operator=(const Semaphore&) = delete;
        Semaphore(Semaphore&&) = delete;
        Semaphore& operator=(Semaphore&&) = delete;

        [[nodiscard]]
        VkSemaphore handle() const noexcept;

    private:
        VkDevice device_{ VK_NULL_HANDLE };
        VkSemaphore semaphore_{ VK_NULL_HANDLE };
    };
}