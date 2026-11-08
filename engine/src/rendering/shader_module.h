#pragma once

#include <vulkan/vulkan.h>

#include <cstdint>
#include <vector>

namespace arvek
{
    class Device;

    class ShaderModule final
    {
    public:
        ShaderModule(const Device& device,
            const std::vector<std::uint32_t>& spirv);

        ~ShaderModule();

        ShaderModule(const ShaderModule&) = delete;
        ShaderModule& operator=(const ShaderModule&) = delete;
        ShaderModule(ShaderModule&&) = delete;
        ShaderModule& operator=(ShaderModule&&) = delete;

        [[nodiscard]]
        VkShaderModule handle() const noexcept;

    private:
        VkDevice device_{ VK_NULL_HANDLE };
        VkShaderModule shaderModule_{ VK_NULL_HANDLE };
    };
}