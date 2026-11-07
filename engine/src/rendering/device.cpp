#include "device.h"

#include "physical_device.h"

#include <array>
#include <cstdint>
#include <stdexcept>
#include <string>
#include <vector>

namespace arvek
{
    Device::Device(const PhysicalDevice& physicalDevice)
    {
        if (physicalDevice.handle() == VK_NULL_HANDLE)
        {
            throw std::invalid_argument(
                "Cannot create a logical device from a null physical device!"
            );
        }

        const std::uint32_t graphicsFamily = physicalDevice.graphicsFamilyIndex();

        const std::uint32_t presentationFamily = physicalDevice.presentationFamilyIndex();

        if (graphicsFamily == UINT32_MAX ||
            presentationFamily == UINT32_MAX)
        {
            throw std::invalid_argument(
                "Invalid Vulkan queue family indices!"
            );
        }

        const float queuePriority = 1.0f;

        // Request a single queue from each distinct family.
        std::array<std::uint32_t, 2> families{
            graphicsFamily,
            presentationFamily
        };

        std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
        queueCreateInfos.reserve(2);

        for (std::size_t i = 0; i < families.size(); ++i)
        {
            if (i == 1 && families[i] == families[0])
            {
                // Since we only have two families 'graphics' and 'presentation',
                // if both indices are equal, only one VkDeviceQueueCreateInfo is added.
                continue;
            }

            VkDeviceQueueCreateInfo queueInfo{};
            queueInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
            queueInfo.queueFamilyIndex = families[i];
            queueInfo.queueCount = 1;
            queueInfo.pQueuePriorities = &queuePriority;

            queueCreateInfos.push_back(queueInfo);
        }

        // Enabled features can be extended as renderer needs grow.
        VkPhysicalDeviceFeatures enabledFeatures{};

        constexpr std::array<const char*, 1> extensions{
            VK_KHR_SWAPCHAIN_EXTENSION_NAME
        };

        VkDeviceCreateInfo createInfo{};
        createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
        createInfo.queueCreateInfoCount = static_cast<std::uint32_t>(queueCreateInfos.size());
        createInfo.pQueueCreateInfos = queueCreateInfos.data();
        createInfo.pEnabledFeatures = &enabledFeatures;
        createInfo.enabledExtensionCount = static_cast<std::uint32_t>(extensions.size());
        createInfo.ppEnabledExtensionNames = extensions.data();
        createInfo.enabledLayerCount = 0;
        createInfo.ppEnabledLayerNames = nullptr;

        const VkResult result = vkCreateDevice(
            physicalDevice.handle(),
            &createInfo,
            nullptr,
            &device_
        );
        if (result != VK_SUCCESS)
        {
            throw std::runtime_error(
                "Failed to create Vulkan logical device. VkResult: " +
                std::to_string(static_cast<int>(result))
            );
        }

        vkGetDeviceQueue(
            device_,
            graphicsFamily,
            0,
            &graphicsQueue_
        );

        vkGetDeviceQueue(
            device_,
            presentationFamily,
            0,
            &presentationQueue_
        );
    }

    Device::~Device()
    {
        if (device_ != VK_NULL_HANDLE)
        {
            vkDestroyDevice(device_, nullptr);
        }
    }

    VkDevice Device::handle() const noexcept
    {
        return device_;
    }

    VkQueue Device::graphicsQueue() const noexcept
    {
        return graphicsQueue_;
    }

    VkQueue Device::presentationQueue() const noexcept
    {
        return presentationQueue_;
    }
}