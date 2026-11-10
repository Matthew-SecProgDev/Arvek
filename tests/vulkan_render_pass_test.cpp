#include <arvek/core/engine.h>
#include <arvek/core/logger.h>
#include "device.h"
#include "framebuffer.h"
#include "instance.h"
#include "physical_device.h"
#include "render_pass.h"
#include "surface.h"
#include "swapchain.h"

#include <exception>
#include <memory>
#include <stdexcept>
#include <string>
#include <vector>
#include <cstdlib>

int main()
{
    try
    {
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

            arvek::RenderPass renderPass(
                device,
                swapchain.imageFormat()
            );

            std::vector<std::unique_ptr<arvek::Framebuffer>> framebuffers;

            framebuffers.reserve(swapchain.imageViews().size());

            for (VkImageView imageView : swapchain.imageViews())
            {
                framebuffers.push_back(
                    std::make_unique<arvek::Framebuffer>(
                        device,
                        renderPass,
                        imageView,
                        swapchain.extent())
                );
            }

            if (framebuffers.empty() ||
                framebuffers.size() != swapchain.images().size())
            {
                throw std::runtime_error("Unexpected framebuffer count!");
            }

            for (const auto& framebuffer : framebuffers)
            {
                if (framebuffer->handle() == VK_NULL_HANDLE)
                {
                    throw std::runtime_error("Vulkan framebuffer handle is null!");
                }
            }

            ARV_LOG_INFO(
                "Vulkan render pass and framebuffers created successfully."
            );
        }

        engine.shutdown();
    }
    catch (const std::exception& e)
    {
        ARV_LOG_ERROR(
            std::string("Render pass test failed: ") +
            e.what()
        );

        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}