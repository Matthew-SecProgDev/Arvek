#include <arvek/core/engine.h>
#include <arvek/core/logger.h>
#include "device.h"
#include "graphics_pipeline.h"
#include "instance.h"
#include "physical_device.h"
#include "pipeline_layout.h"
#include "render_pass.h"
#include "shader_module.h"
#include "surface.h"
#include "swapchain.h"

#include <cstdint>
#include <exception>
#include <fstream>
#include <stdexcept>
#include <string>
#include <vector>
#include <cstdlib>

namespace
{
    std::vector<std::uint32_t> readSpirv(const std::string& path)
    {
        std::ifstream file(path, std::ios::binary | std::ios::ate);

        if (!file)
        {
            throw std::runtime_error("Cannot open SPIR-V file: " + path);
        }

        const std::streamoff size = file.tellg();

        if (size <= 0 || size % sizeof(std::uint32_t) != 0)
        {
            throw std::runtime_error("Invalid SPIR-V file size: " + path);
        }

        std::vector<std::uint32_t> code(static_cast<std::size_t>(size) / sizeof(std::uint32_t));

        file.seekg(0, std::ios::beg);

        if (!file.read(reinterpret_cast<char*>(code.data()), static_cast<std::streamsize>(size)))
        {
            throw std::runtime_error("Failed to read SPIR-V file: " + path);
        }

        return code;
    }
}

int main(int argc, char* argv[])
{
    try
    {
        if (argc != 3)
        {
            throw std::invalid_argument("Expected vertex and fragment SPIR-V paths!");
        }

        const auto vertexCode = readSpirv(argv[1]);
        const auto fragmentCode = readSpirv(argv[2]);

        arvek::Engine engine;
        engine.initialize(false);

        {
            arvek::Instance instance(true);

            arvek::Surface surface(engine.window(), instance);

            arvek::PhysicalDevice physicalDevice(instance, surface);

            arvek::Device device(physicalDevice);

            arvek::Swapchain swapchain(
                physicalDevice,
                device,
                surface,
                engine.window()
            );

            arvek::ShaderModule vertexShader(device, vertexCode);

            arvek::ShaderModule fragmentShader(device, fragmentCode);

            arvek::RenderPass renderPass(device, swapchain.imageFormat());

            arvek::PipelineLayout pipelineLayout(device);

            arvek::GraphicsPipeline pipeline(
                device,
                renderPass,
                pipelineLayout,
                vertexShader,
                fragmentShader
            );

            if (pipeline.handle() == VK_NULL_HANDLE)
            {
                throw std::runtime_error("Graphics pipeline handle is null!");
            }

            ARV_LOG_INFO("Vulkan graphics pipeline created successfully.");
        }

        engine.shutdown();
    }
    catch (const std::exception& e)
    {
        ARV_LOG_ERROR(
            std::string("Graphics pipeline test failed: ") +
            e.what()
        );

        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}