#include <arvek/core/engine.h>
#include <arvek/core/logger.h>
#include "vulkan_renderer.h"

#include <cstdlib>
#include <cstdint>
#include <exception>
#include <fstream>
#include <stdexcept>
#include <string>
#include <vector>

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

        if (!file.read(
            reinterpret_cast<char*>(code.data()),
            static_cast<std::streamsize>(size)))
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
            throw std::invalid_argument(
                "Expected vertex and fragment SPIR-V paths!"
            );
        }

        const auto vertexCode = readSpirv(argv[1]);
        const auto fragmentCode = readSpirv(argv[2]);

        arvek::Engine engine;
        engine.initialize();

        {
            // Renderer is created after the window.
            arvek::VulkanRenderer renderer(
                engine.window(),
                vertexCode,
                fragmentCode
            );

            ARV_LOG_INFO("VulkanRenderer initialized successfully.");
        }

        // Renderer and its Vulkan resources are destroyed
        // before the engine destroys its window.
        engine.shutdown();

        ARV_LOG_INFO("VulkanRenderer destroyed successfully.");
    }
    catch (const std::exception& e)
    {
        ARV_LOG_ERROR(
            std::string("VulkanRenderer test failed: ") +
            e.what()
        );

        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}