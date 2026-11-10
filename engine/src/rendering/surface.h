#pragma once

#include <vulkan/vulkan.h>

namespace arvek
{
    class Instance;
    class Window;

    class Surface final
    {
    public:
        Surface(const Window& window, const Instance& instance);
        ~Surface();

        Surface(const Surface&) = delete;
        Surface& operator=(const Surface&) = delete;
        Surface(Surface&&) = delete;
        Surface& operator=(Surface&&) = delete;

        [[nodiscard]]
        VkSurfaceKHR handle() const noexcept;

    private:
        VkInstance instance_{ VK_NULL_HANDLE };
        VkSurfaceKHR surface_{ VK_NULL_HANDLE };
    };
}