#include "command_pool.h"

#include "device.h"
#include "physical_device.h"

#include <stdexcept>
#include <string>

namespace arvek
{
    CommandPool::CommandPool(const Device& device,
        const PhysicalDevice& physicalDevice,
        std::uint32_t commandBufferCount) : device_(device.handle())
    {
        if (device_ == VK_NULL_HANDLE ||
            physicalDevice.handle() == VK_NULL_HANDLE)
        {
            throw std::invalid_argument(
                "CommandPool requires valid Vulkan devices!"
            );
        }

        if (commandBufferCount == 0)
        {
            throw std::invalid_argument(
                "Command buffer count must be positive!"
            );
        }

        VkCommandPoolCreateInfo poolInfo{};
        poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
        poolInfo.queueFamilyIndex = physicalDevice.graphicsFamilyIndex();
        poolInfo.flags = 0; //0 means reset all command buffers together using 'vkResetCommandPool()'

        VkResult result = vkCreateCommandPool(
            device_,
            &poolInfo,
            nullptr,
            &pool_
        );

        if (result != VK_SUCCESS)
        {
            throw std::runtime_error(
                "Failed to create command pool. VkResult: " +
                std::to_string(static_cast<int>(result))
            );
        }

        try
        {
            commandBuffers_.resize(commandBufferCount);

            VkCommandBufferAllocateInfo allocationInfo{};
            allocationInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
            allocationInfo.commandPool = pool_;
            allocationInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
            allocationInfo.commandBufferCount = commandBufferCount;

            result = vkAllocateCommandBuffers(
                device_,
                &allocationInfo,
                commandBuffers_.data()
            );

            if (result != VK_SUCCESS)
            {
                throw std::runtime_error(
                    "Failed to allocate command buffers. VkResult: " +
                    std::to_string(static_cast<int>(result))
                );
            }
        }
        catch (...)
        {
            vkDestroyCommandPool(device_, pool_, nullptr);
            throw;
        }
    }

    CommandPool::~CommandPool()
    {
        if (pool_ != VK_NULL_HANDLE)
        {
            vkDestroyCommandPool(device_, pool_, nullptr);
        }
    }

    VkCommandPool CommandPool::handle() const noexcept
    {
        return pool_;
    }

    const std::vector<VkCommandBuffer>& CommandPool::commandBuffers() const noexcept
    {
        return commandBuffers_;
    }

    void CommandPool::reset()
    {
        const VkResult result = vkResetCommandPool(device_, pool_, 0);

        if (result != VK_SUCCESS)
        {
            throw std::runtime_error(
                "Failed to reset command pool. VkResult: " +
                std::to_string(static_cast<int>(result))
            );
        }
    }
}