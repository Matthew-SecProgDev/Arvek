#include "instance.h"

#include <GLFW/glfw3.h>

#include <cstdint>
#include <stdexcept>
#include <algorithm>
#include <cstdio>
#include <cstring>
#include <string>
#include <vector>

namespace arvek
{
    namespace
    {
        constexpr const char* validationLayer = "VK_LAYER_KHRONOS_validation";

        bool validationLayerAvailable()
        {
            std::uint32_t count = 0;

            if (vkEnumerateInstanceLayerProperties(
                &count,
                nullptr) != VK_SUCCESS || count == 0)
            {
                throw std::runtime_error(
                    "Failed to enumerate Vulkan layers!"
                );
            }

            std::vector<VkLayerProperties> layers(count);

            if (vkEnumerateInstanceLayerProperties(
                &count,
                layers.data()) != VK_SUCCESS)
            {
                throw std::runtime_error(
                    "Failed to retrieve Vulkan layers!"
                );
            }

            return std::any_of(
                layers.begin(),
                layers.end(),
                [](const VkLayerProperties& layer)
                {
                    return std::strcmp(layer.layerName, validationLayer) == 0;
                }
            );
        }

        bool debugUtilsAvailable()
        {
            std::uint32_t count = 0;

            if (vkEnumerateInstanceExtensionProperties(
                nullptr,
                &count,
                nullptr) != VK_SUCCESS || count == 0)
            {
                throw std::runtime_error(
                    "Failed to enumerate instance extensions!"
                );
            }

            std::vector<VkExtensionProperties> extensions(count);

            if (vkEnumerateInstanceExtensionProperties(
                nullptr,
                &count,
                extensions.data()) != VK_SUCCESS)
            {
                throw std::runtime_error(
                    "Failed to retrieve instance extensions!"
                );
            }

            return std::any_of(
                extensions.begin(),
                extensions.end(),
                [](const VkExtensionProperties& extension)
                {
                    return std::strcmp(
                        extension.extensionName,
                        VK_EXT_DEBUG_UTILS_EXTENSION_NAME) == 0;
                }
            );
        }

        VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(
            VkDebugUtilsMessageSeverityFlagBitsEXT severity,
            VkDebugUtilsMessageTypeFlagsEXT,
            const VkDebugUtilsMessengerCallbackDataEXT* data,
            void*)
        {
            const char* level = "INFO";

            if (severity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT)
            {
                level = "ERROR";
            }
            else if (severity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT)
            {
                level = "WARNING";
            }

            std::fprintf(
                stderr,
                "[Vulkan %s] %s\n",
                level,
                data && data->pMessage ? data->pMessage : "(no message)"
            );

            return VK_FALSE;
        }

        VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo()
        {
            VkDebugUtilsMessengerCreateInfoEXT info{};
            info.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
            info.messageSeverity =
                VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
                VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
            info.messageType =
                VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT |
                VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |
                VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
            info.pfnUserCallback = debugCallback;

            return info;
        }
    }

    Instance::Instance(bool enableValidation)
    {
        if (glfwVulkanSupported() != GLFW_TRUE)
        {
            throw std::runtime_error(
                "Vulkan loader or driver is not available!"
            );
        }

        std::uint32_t glfwExtensionCount = 0;

        const char** glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

        if (!glfwExtensions || glfwExtensionCount == 0)
        {
            throw std::runtime_error(
                "GLFW returned no required Vulkan extensions!"
            );
        }

        std::vector<const char*> extensions(
            glfwExtensions,
            glfwExtensions + glfwExtensionCount
        );

        if (enableValidation)
        {
            if (!validationLayerAvailable())
            {
                throw std::runtime_error(
                    "VK_LAYER_KHRONOS_validation is unavailable!"
                );
            }

            if (!debugUtilsAvailable())
            {
                throw std::runtime_error(
                    "VK_EXT_debug_utils is unavailable!"
                );
            }

            extensions.push_back(
                VK_EXT_DEBUG_UTILS_EXTENSION_NAME
            );
        }

        VkApplicationInfo applicationInfo{};
        applicationInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
        applicationInfo.pApplicationName = "Arvek";
        applicationInfo.applicationVersion = VK_MAKE_VERSION(0, 1, 0);
        applicationInfo.pEngineName = "Arvek";
        applicationInfo.engineVersion = VK_MAKE_VERSION(0, 1, 0);
        applicationInfo.apiVersion = VK_API_VERSION_1_0;

        auto messengerInfo = debugCreateInfo();

        VkInstanceCreateInfo createInfo{};
        createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
        createInfo.pApplicationInfo = &applicationInfo;
        createInfo.enabledExtensionCount = static_cast<std::uint32_t>(extensions.size());
        createInfo.ppEnabledExtensionNames = extensions.data();

        if (enableValidation)
        {
            createInfo.enabledLayerCount = 1;
            createInfo.ppEnabledLayerNames = &validationLayer;

            // Capture messages emitted during vkCreateInstance.
            createInfo.pNext = &messengerInfo;
        }

        const VkResult result = vkCreateInstance(
            &createInfo,
            nullptr,
            &instance_
        );

        if (result != VK_SUCCESS)
        {
            throw std::runtime_error(
                "Failed to create Vulkan instance. VkResult: " +
                std::to_string(static_cast<int>(result))
            );
        }

        if (enableValidation)
        {
            auto createMessenger = reinterpret_cast<PFN_vkCreateDebugUtilsMessengerEXT>(
                vkGetInstanceProcAddr(
                    instance_,
                    "vkCreateDebugUtilsMessengerEXT")
                );

            if (!createMessenger)
            {
                vkDestroyInstance(instance_, nullptr);

                throw std::runtime_error(
                    "Cannot load vkCreateDebugUtilsMessengerEXT!"
                );
            }

            const VkResult messengerResult =
                createMessenger(
                    instance_,
                    &messengerInfo,
                    nullptr,
                    &debugMessenger_
                );

            if (messengerResult != VK_SUCCESS)
            {
                vkDestroyInstance(instance_, nullptr);

                throw std::runtime_error(
                    "Failed to create Vulkan debug messenger!"
                );
            }
        }
    }

    Instance::~Instance()
    {
        if (debugMessenger_ != VK_NULL_HANDLE)
        {
            auto destroyMessenger = reinterpret_cast<PFN_vkDestroyDebugUtilsMessengerEXT>(
                vkGetInstanceProcAddr(
                    instance_,
                    "vkDestroyDebugUtilsMessengerEXT")
                );

            if (destroyMessenger)
            {
                destroyMessenger(
                    instance_,
                    debugMessenger_,
                    nullptr
                );
            }
        }

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