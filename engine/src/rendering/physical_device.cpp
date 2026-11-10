#include "physical_device.h"

#include "instance.h"
#include "surface.h"

#include <algorithm>
#include <cstring>
#include <optional>
#include <stdexcept>
#include <vector>

namespace arvek
{
    namespace
    {
        struct QueueFamilies
        {
            std::optional<std::uint32_t> graphics;
            std::optional<std::uint32_t> presentation;
        };

        QueueFamilies findQueueFamilies(VkPhysicalDevice device, VkSurfaceKHR surface)
        {
            std::uint32_t count = 0;

            vkGetPhysicalDeviceQueueFamilyProperties(
                device,
                &count,
                nullptr
            );
            if (count == 0)
            {
                throw std::runtime_error(
                    "No Vulkan queue families found!"
                );
            }

            std::vector<VkQueueFamilyProperties> families(count);

            vkGetPhysicalDeviceQueueFamilyProperties(
                device,
                &count,
                families.data()
            );

            QueueFamilies result;

            for (std::uint32_t i = 0; i < count; ++i)
            {
                if (families[i].queueCount == 0)
                {
                    continue;
                }

                if (!result.graphics &&
                    (families[i].queueFlags & VK_QUEUE_GRAPHICS_BIT) != 0)
                {
                    result.graphics = i;
                }

                VkBool32 presentationSupported = VK_FALSE;

                if (vkGetPhysicalDeviceSurfaceSupportKHR(
                    device,
                    i,
                    surface,
                    &presentationSupported) != VK_SUCCESS)
                {
                    throw std::runtime_error(
                        "Failed to query Vulkan presentation support!"
                    );
                }

                if (!result.presentation && presentationSupported)
                {
                    result.presentation = i;
                }

                // Prefer one family supporting both operations.
                if ((families[i].queueFlags & VK_QUEUE_GRAPHICS_BIT) != 0 && 
                    presentationSupported)
                {
                    result.graphics = i;
                    result.presentation = i;
                    break;
                }
            }

            return result;
        }

        bool supportsSwapchainExtension(VkPhysicalDevice device)
        {
            std::uint32_t count = 0;

            if (vkEnumerateDeviceExtensionProperties(
                device,
                nullptr,
                &count,
                nullptr) != VK_SUCCESS || count == 0)
            {
                throw std::runtime_error(
                    "Failed to enumerate Vulkan device extensions!"
                );
            }

            std::vector<VkExtensionProperties> extensions(count);

            if (vkEnumerateDeviceExtensionProperties(
                device,
                nullptr,
                &count,
                extensions.data()) != VK_SUCCESS)
            {
                throw std::runtime_error(
                    "Failed to retrieve Vulkan device extensions!"
                );
            }

            return std::any_of(
                extensions.begin(),
                extensions.end(),
                [](const VkExtensionProperties& extension)
                {
                    return std::strcmp(
                        extension.extensionName,
                        VK_KHR_SWAPCHAIN_EXTENSION_NAME) == 0;
                }
            );
        }

        bool supportsSwapchain(VkPhysicalDevice device, VkSurfaceKHR surface)
        {
            std::uint32_t formatCount = 0;
            std::uint32_t presentModeCount = 0;

            if (vkGetPhysicalDeviceSurfaceFormatsKHR(
                device,
                surface,
                &formatCount,
                nullptr) != VK_SUCCESS)
            {
                throw std::runtime_error(
                    "Failed to query Vulkan surface formats!"
                );
            }

            if (vkGetPhysicalDeviceSurfacePresentModesKHR(
                device,
                surface,
                &presentModeCount,
                nullptr) != VK_SUCCESS)
            {
                throw std::runtime_error(
                    "Failed to query Vulkan presentation modes!"
                );
            }

            return formatCount > 0 && presentModeCount > 0;
        }
    }

    PhysicalDevice::PhysicalDevice(const Instance& instance, const Surface& surface)
    {
        if (instance.handle() == VK_NULL_HANDLE ||
            surface.handle() == VK_NULL_HANDLE)
        {
            throw std::invalid_argument(
                "Physical device selection requires a valid instance and surface!"
            );
        }

        std::uint32_t deviceCount = 0;

        if (vkEnumeratePhysicalDevices(
            instance.handle(),
            &deviceCount,
            nullptr) != VK_SUCCESS || deviceCount == 0)
        {
            throw std::runtime_error(
                "No Vulkan physical devices are available!"
            );
        }

        std::vector<VkPhysicalDevice> devices(deviceCount);

        if (vkEnumeratePhysicalDevices(
            instance.handle(),
            &deviceCount,
            devices.data()) != VK_SUCCESS)
        {
            throw std::runtime_error(
                "Failed to enumerate Vulkan physical devices!"
            );
        }

        int bestScore = -1;
        for (VkPhysicalDevice candidate : devices)
        {
            const auto queues = findQueueFamilies(candidate, surface.handle());

            if (!queues.graphics || !queues.presentation)
            {
                continue;
            }

            if (!supportsSwapchainExtension(candidate))
            {
                continue;
            }

            if (!supportsSwapchain(candidate, surface.handle()))
            {
                continue;
            }

            VkPhysicalDeviceProperties props{};
            vkGetPhysicalDeviceProperties(candidate, &props);

            const int score =
                props.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU
                ? 100
                : 0;

            if (score > bestScore)
            {
                bestScore = score;

                physicalDevice_ = candidate;
                graphicsFamilyIndex_ = *queues.graphics;
                presentationFamilyIndex_ = *queues.presentation;
                properties_ = props;
            }
        }

        if (physicalDevice_ == VK_NULL_HANDLE)
        {
            throw std::runtime_error(
                "No suitable Vulkan GPU supports graphics and presentation!"
            );
        }
    }

    VkPhysicalDevice PhysicalDevice::handle() const noexcept
    {
        return physicalDevice_;
    }

    std::uint32_t PhysicalDevice::graphicsFamilyIndex() const noexcept
    {
        return graphicsFamilyIndex_;
    }

    std::uint32_t PhysicalDevice::presentationFamilyIndex() const noexcept
    {
        return presentationFamilyIndex_;
    }

    const VkPhysicalDeviceProperties& PhysicalDevice::properties() const noexcept
    {
        return properties_;
    }
}