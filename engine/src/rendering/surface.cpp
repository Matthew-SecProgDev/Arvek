#include "surface.h"

#include "instance.h"
#include "surface_provider.h"
#include <arvek/core/window.h>
#include "../core/window_backend.h"

#include <stdexcept>

namespace arvek
{
    Surface::Surface(const Window& window, const Instance& instance)
        : instance_(instance.handle())
    {
        if (instance_ == VK_NULL_HANDLE)
        {
            throw std::invalid_argument(
                "Cannot create surface with a null Vulkan instance"
            );
        }

        const auto* provider = dynamic_cast<SurfaceProvider*>(window.backend_.get());

        if (!provider)
        {
            throw std::logic_error(
                "Window backend does not support Vulkan surfaces!"
            );
        }

        surface_ = provider->createSurface(instance_);

        if (surface_ == VK_NULL_HANDLE)
        {
            throw std::runtime_error(
                "Surface provider returned a null Vulkan surface!"
            );
        }
    }

    Surface::~Surface()
    {
        if (surface_ != VK_NULL_HANDLE)
        {
            vkDestroySurfaceKHR(
                instance_,
                surface_,
                nullptr
            );
        }
    }

    VkSurfaceKHR Surface::handle() const noexcept
    {
        return surface_;
    }
}