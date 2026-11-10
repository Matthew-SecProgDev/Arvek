#pragma once

#include <vulkan/vulkan.h>

namespace arvek
{
    class SurfaceProvider
    {
    public:
        virtual ~SurfaceProvider() = default;

        [[nodiscard]]
        virtual VkSurfaceKHR createSurface(VkInstance instance) const = 0;
    };
}