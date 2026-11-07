#include "swapchain.h"

#include "device.h"
#include "physical_device.h"
#include "surface.h"
#include <arvek/core/window.h>

#include <algorithm>
#include <array>
#include <cstdint>
#include <stdexcept>
#include <string>
#include <vector>

namespace arvek
{
    namespace
    {
        void check(VkResult result, const char* operation)
        {
            if (result != VK_SUCCESS)
            {
                throw std::runtime_error(
                    std::string(operation) +
                    " failed. VkResult: " +
                    std::to_string(static_cast<int>(result))
                );
            }
        }

        VkSurfaceFormatKHR chooseSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& formats)
        {
            for (const auto& format : formats)
            {
                if (format.format == VK_FORMAT_B8G8R8A8_SRGB &&
                    format.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR)
                {
                    return format;
                }
            }

            // VK_FORMAT_UNDEFINED means the application
            // may select its own supported format.
            if (formats.size() == 1 &&
                formats[0].format == VK_FORMAT_UNDEFINED)
            {
                return {
                    VK_FORMAT_B8G8R8A8_SRGB,
                    formats[0].colorSpace
                };
            }

            return formats.front();
        }

        VkPresentModeKHR choosePresentMode(const std::vector<VkPresentModeKHR>& modes)
        {
            const auto it = std::find(
                modes.begin(),
                modes.end(),
                VK_PRESENT_MODE_FIFO_KHR
            );

            if (it == modes.end())
            {
                throw std::runtime_error(
                    "FIFO presentation mode was not reported!"
                );
            }

            return VK_PRESENT_MODE_FIFO_KHR;
        }

        VkExtent2D chooseExtent(
            const VkSurfaceCapabilitiesKHR& capabilities,
            const Window& window)
        {
            if (capabilities.currentExtent.width != UINT32_MAX)
            {
                return capabilities.currentExtent;
            }

            const auto framebuffer = window.framebufferExtent();

            if (framebuffer.width == 0 ||
                framebuffer.height == 0)
            {
                throw std::runtime_error(
                    "Cannot create a swapchain for a zero-sized framebuffer!"
                );
            }

            return {
                std::clamp(
                    framebuffer.width,
                    capabilities.minImageExtent.width,
                    capabilities.maxImageExtent.width
                ),
                std::clamp(
                    framebuffer.height,
                    capabilities.minImageExtent.height,
                    capabilities.maxImageExtent.height
                )
            };
        }

        VkCompositeAlphaFlagBitsKHR chooseCompositeAlpha(
            const VkSurfaceCapabilitiesKHR& capabilities)
        {
            constexpr std::array<VkCompositeAlphaFlagBitsKHR, 4> candidates
            {
                VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR,
                VK_COMPOSITE_ALPHA_PRE_MULTIPLIED_BIT_KHR,
                VK_COMPOSITE_ALPHA_POST_MULTIPLIED_BIT_KHR,
                VK_COMPOSITE_ALPHA_INHERIT_BIT_KHR
            };

            for (const auto candidate : candidates)
            {
                if ((capabilities.supportedCompositeAlpha & candidate) != 0)
                {
                    return candidate;
                }
            }

            throw std::runtime_error(
                "No supported Vulkan composite alpha mode!"
            );
        }
    }

    Swapchain::Swapchain(const PhysicalDevice& physicalDevice,
        const Device& device,
        const Surface& surface,
        const Window& window) : device_(device.handle())
    {
        if (physicalDevice.handle() == VK_NULL_HANDLE ||
            device_ == VK_NULL_HANDLE ||
            surface.handle() == VK_NULL_HANDLE)
        {
            throw std::invalid_argument(
                "Swapchain requires valid Vulkan handles!"
            );
        }

        try
        {
            // Query surface capabilities
            VkSurfaceCapabilitiesKHR capabilities{};

            check(
                vkGetPhysicalDeviceSurfaceCapabilitiesKHR(
                    physicalDevice.handle(),
                    surface.handle(),
                    &capabilities
                ),
                "Query surface capabilities"
            );

            // Query surface formats
            std::uint32_t formatCount = 0;

            check(
                vkGetPhysicalDeviceSurfaceFormatsKHR(
                    physicalDevice.handle(),
                    surface.handle(),
                    &formatCount,
                    nullptr
                ),
                "Query surface formats"
            );

            if (formatCount == 0)
            {
                throw std::runtime_error(
                    "No Vulkan surface formats available!"
                );
            }

            std::vector<VkSurfaceFormatKHR> formats(formatCount);

            check(
                vkGetPhysicalDeviceSurfaceFormatsKHR(
                    physicalDevice.handle(),
                    surface.handle(),
                    &formatCount,
                    formats.data()
                ),
                "Retrieve surface formats"
            );

            // Query presentation modes
            std::uint32_t modeCount = 0;

            check(
                vkGetPhysicalDeviceSurfacePresentModesKHR(
                    physicalDevice.handle(),
                    surface.handle(),
                    &modeCount,
                    nullptr
                ),
                "Query presentation modes"
            );

            if (modeCount == 0)
            {
                throw std::runtime_error(
                    "No Vulkan presentation modes available!"
                );
            }

            std::vector<VkPresentModeKHR> modes(modeCount);

            check(
                vkGetPhysicalDeviceSurfacePresentModesKHR(
                    physicalDevice.handle(),
                    surface.handle(),
                    &modeCount,
                    modes.data()
                ),
                "Retrieve presentation modes"
            );

            const auto surfaceFormat = chooseSurfaceFormat(formats);
            const auto presentMode = choosePresentMode(modes);
            const auto imageExtent = chooseExtent(capabilities, window);

            std::uint32_t imageCount = capabilities.minImageCount + 1;

            if (capabilities.maxImageCount > 0)
            {
                imageCount = std::min(
                    imageCount,
                    capabilities.maxImageCount
                );
            }

            if ((capabilities.supportedUsageFlags & VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT) == 0)
            {
                throw std::runtime_error(
                    "Surface does not support color attachment images!"
                );
            }

            const std::array<std::uint32_t, 2> queueFamilies{
                physicalDevice.graphicsFamilyIndex(),
                physicalDevice.presentationFamilyIndex()
            };

            VkSwapchainCreateInfoKHR createInfo{};
            createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
            createInfo.surface = surface.handle();
            createInfo.minImageCount = imageCount;
            createInfo.imageFormat = surfaceFormat.format;
            createInfo.imageColorSpace = surfaceFormat.colorSpace;
            createInfo.imageExtent = imageExtent;
            createInfo.imageArrayLayers = 1;
            createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

            if (queueFamilies[0] != queueFamilies[1])
            {
                createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;

                createInfo.queueFamilyIndexCount = 2;
                createInfo.pQueueFamilyIndices = queueFamilies.data();
            }
            else
            {
                createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
                createInfo.queueFamilyIndexCount = 0u;
                createInfo.pQueueFamilyIndices = nullptr;
            }

            createInfo.preTransform = capabilities.currentTransform;
            createInfo.compositeAlpha = chooseCompositeAlpha(capabilities);
            createInfo.presentMode = presentMode;
            createInfo.clipped = VK_TRUE;
            createInfo.oldSwapchain = VK_NULL_HANDLE;

            check(
                vkCreateSwapchainKHR(
                    device_,
                    &createInfo,
                    nullptr,
                    &swapchain_
                ),
                "Create Vulkan swapchain"
            );

            std::uint32_t actualImageCount = 0;

            check(
                vkGetSwapchainImagesKHR(
                    device_,
                    swapchain_,
                    &actualImageCount,
                    nullptr
                ),
                "Query swapchain images"
            );

            images_.resize(actualImageCount);

            check(
                vkGetSwapchainImagesKHR(
                    device_,
                    swapchain_,
                    &actualImageCount,
                    images_.data()
                ),
                "Retrieve swapchain images"
            );

            if (images_.empty())
            {
                throw std::runtime_error(
                    "Swapchain returned no images!"
                );
            }

            imageViews_.reserve(images_.size());

            for (VkImage image : images_)
            {
                VkImageViewCreateInfo viewInfo{};
                viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
                viewInfo.image = image;
                viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
                viewInfo.format = surfaceFormat.format;
                viewInfo.components = {
                    VK_COMPONENT_SWIZZLE_IDENTITY,
                    VK_COMPONENT_SWIZZLE_IDENTITY,
                    VK_COMPONENT_SWIZZLE_IDENTITY,
                    VK_COMPONENT_SWIZZLE_IDENTITY
                };
                viewInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
                viewInfo.subresourceRange.baseMipLevel = 0;
                viewInfo.subresourceRange.levelCount = 1;
                viewInfo.subresourceRange.baseArrayLayer = 0;
                viewInfo.subresourceRange.layerCount = 1;

                VkImageView imageView = VK_NULL_HANDLE;

                check(
                    vkCreateImageView(
                        device_,
                        &viewInfo,
                        nullptr,
                        &imageView
                    ),
                    "Create swapchain image view"
                );

                imageViews_.push_back(imageView);
            }

            imageFormat_ = surfaceFormat.format;
            extent_ = imageExtent;
        }
        catch (...)
        {
            destroy();
            throw;
        }
    }

    Swapchain::~Swapchain()
    {
        destroy();
    }

    void Swapchain::destroy() noexcept
    {
        for (VkImageView imageView : imageViews_)
        {
            vkDestroyImageView(device_, imageView, nullptr);
        }

        imageViews_.clear();
        images_.clear();

        if (swapchain_ != VK_NULL_HANDLE)
        {
            vkDestroySwapchainKHR(
                device_,
                swapchain_,
                nullptr
            );

            swapchain_ = VK_NULL_HANDLE;
        }
    }

    VkSwapchainKHR Swapchain::handle() const noexcept
    {
        return swapchain_;
    }

    VkFormat Swapchain::imageFormat() const noexcept
    {
        return imageFormat_;
    }

    VkExtent2D Swapchain::extent() const noexcept
    {
        return extent_;
    }

    const std::vector<VkImage>& Swapchain::images() const noexcept
    {
        return images_;
    }

    const std::vector<VkImageView>& Swapchain::imageViews() const noexcept
    {
        return imageViews_;
    }
}