#include <arvek/core/engine.h>
#include <arvek/core/logger.h>
#include <arvek/core/window.h>
#include "vulkan_renderer.h"

#include <cstdint>
#include <exception>
#include <fstream>
#include <stdexcept>
#include <string>
#include <vector>
#include <cstdlib>

#ifndef ARVEK_TRIANGLE_VERTEX_SPV
#error "ARVEK_TRIANGLE_VERTEX_SPV must be defined by CMake"
#endif

#ifndef ARVEK_TRIANGLE_FRAGMENT_SPV
#error "ARVEK_TRIANGLE_FRAGMENT_SPV must be defined by CMake"
#endif

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

int main()
{
    try
    {
        const auto vertexCode = readSpirv(ARVEK_TRIANGLE_VERTEX_SPV);
        const auto fragmentCode = readSpirv(ARVEK_TRIANGLE_FRAGMENT_SPV);

        arvek::Engine engine;
        engine.initialize();

        {
            arvek::VulkanRenderer renderer(
                engine.window(),
                vertexCode,
                fragmentCode
            );

            ARV_LOG_INFO("VulkanRenderer initialized successfully.");

            auto& window = engine.window();

            while (!window.shouldClose())
            {
                window.pollEvents();

                if (window.wasResized())
                {
                    ARV_LOG_INFO(
                        "Window resized; renderer recreation will be added next."
                    );
                    break;
                }

                if (!renderer.drawFrame())
                {
                    ARV_LOG_INFO("Swapchain needs recreation.");
                    break;
                }
            }
        }

        // VulkanRenderer is destroyed before Engine.
        engine.shutdown();

        ARV_LOG_INFO("Renderer manual test completed.");
    }
    catch (const std::exception& e)
    {
        ARV_LOG_ERROR(
            std::string("Renderer manual test failed: ") +
            e.what()
        );

        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}