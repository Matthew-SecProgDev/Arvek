#include "vulkan_renderer.h"

#include "device.h"
#include "instance.h"
#include "physical_device.h"
#include "surface.h"

#include <memory>

namespace arvek
{
    VulkanRenderer::VulkanRenderer(Window& window) : window_(window)
    {
        instance_ = std::make_unique<Instance>();

        surface_ = std::make_unique<Surface>(window_, *instance_);

        physicalDevice_ = std::make_unique<PhysicalDevice>(*instance_, *surface_);

        device_ = std::make_unique<Device>(*physicalDevice_);
    }

    VulkanRenderer::~VulkanRenderer() = default;
}