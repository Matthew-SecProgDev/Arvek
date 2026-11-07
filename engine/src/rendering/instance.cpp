#include "instance.h"

#include <GLFW/glfw3.h>

#include <cstdint>
#include <stdexcept>

namespace arvek
{
    Instance::Instance()
    {
        if (glfwVulkanSupported() != GLFW_TRUE)
        {
            throw std::runtime_error(
                "GLFW could not find a Vulkan loader or driver"
            );
        }

        std::uint32_t extensionCount = 0;

        const char** requiredExtensions = glfwGetRequiredInstanceExtensions(&extensionCount);

        if (!requiredExtensions || extensionCount == 0)
        {
            throw std::runtime_error(
                "Failed to get required Vulkan extensions"
            );
        }

        VkApplicationInfo applicationInfo{};
        applicationInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
        applicationInfo.pApplicationName = "Arvek";
        applicationInfo.applicationVersion = VK_MAKE_VERSION(0, 1, 0);
        applicationInfo.pEngineName = "Arvek";
        applicationInfo.engineVersion = VK_MAKE_VERSION(0, 1, 0);
        applicationInfo.apiVersion = VK_API_VERSION_1_0;

        VkInstanceCreateInfo createInfo{};
        createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
        createInfo.pApplicationInfo = &applicationInfo;
        createInfo.enabledExtensionCount = extensionCount;
        createInfo.ppEnabledExtensionNames = requiredExtensions;

        if (vkCreateInstance(&createInfo, nullptr, &instance_) != VK_SUCCESS)
        {
            throw std::runtime_error(
                "Failed to create Vulkan instance!"
            );
        }
    }

    Instance::~Instance()
    {
        if (instance_ != VK_NULL_HANDLE)
        {
            vkDestroyInstance(instance_, nullptr);
        }
    }

    VkInstance Instance::handle() const noexcept
    {
        return instance_;
    }
}