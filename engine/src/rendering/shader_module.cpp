#include "shader_module.h"

#include "device.h"

#include <stdexcept>
#include <string>

namespace arvek
{
    ShaderModule::ShaderModule(const Device& device,
        const std::vector<std::uint32_t>& spirv)
        : device_(device.handle())
    {
        if (device_ == VK_NULL_HANDLE)
        {
            throw std::invalid_argument(
                "ShaderModule requires a valid Vulkan device!"
            );
        }

        if (spirv.empty())
        {
            throw std::invalid_argument("SPIR-V code cannot be empty!");
        }

        VkShaderModuleCreateInfo createInfo{};
        createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
        createInfo.codeSize = spirv.size() * sizeof(std::uint32_t);
        createInfo.pCode = spirv.data();

        const VkResult result = vkCreateShaderModule(
            device_,
            &createInfo,
            nullptr,
            &shaderModule_
        );

        if (result != VK_SUCCESS)
        {
            throw std::runtime_error(
                "Failed to create Vulkan shader module. VkResult: " +
                std::to_string(static_cast<int>(result))
            );
        }
    }

    ShaderModule::~ShaderModule()
    {
        if (shaderModule_ != VK_NULL_HANDLE)
        {
            vkDestroyShaderModule(
                device_,
                shaderModule_,
                nullptr
            );
        }
    }

    VkShaderModule ShaderModule::handle() const noexcept
    {
        return shaderModule_;
    }
}