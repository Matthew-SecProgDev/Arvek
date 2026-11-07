#pragma once

#include <vulkan/vulkan.h>

#include <cstdint>

namespace arvek
{
    class Device;

    class Fence final
    {
    public:
        explicit Fence(const Device& device);
        ~Fence();

        Fence(const Fence&) = delete;
        Fence& operator=(const Fence&) = delete;
        Fence(Fence&&) = delete;
        Fence& operator=(Fence&&) = delete;

        [[nodiscard]]
        VkFence handle() const noexcept;

        void wait(std::uint64_t timeout = UINT64_MAX) const;
        void reset();

    private:
        VkDevice device_{ VK_NULL_HANDLE };
        VkFence fence_{ VK_NULL_HANDLE };
    };
}