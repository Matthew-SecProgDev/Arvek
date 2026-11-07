#pragma once

#include "window_backend.h"
#include "window_config.h"
#include "../rendering/surface_provider.h"

struct GLFWwindow;

namespace arvek
{
    class GlfwWindowBackend final : public WindowBackend, public SurfaceProvider
    {
    public:
        explicit GlfwWindowBackend(const WindowConfig& config);
        ~GlfwWindowBackend() override;

        GlfwWindowBackend(const GlfwWindowBackend&) = delete;
        GlfwWindowBackend& operator=(const GlfwWindowBackend&) = delete;
        GlfwWindowBackend(GlfwWindowBackend&&) = delete;
        GlfwWindowBackend& operator=(GlfwWindowBackend&&) = delete;

        [[nodiscard]]
        bool shouldClose() const override;

        void pollEvents() override;
        void waitEvents() override;

        [[nodiscard]]
        VkSurfaceKHR createSurface(VkInstance instance) const override;

        [[nodiscard]]
        WindowExtent framebufferExtent() const override;

        [[nodiscard]]
        bool wasResized() const override;

        void acknowledgeResize() override;

    private:
        GLFWwindow* window_{ nullptr };
        bool resized_{ false };

        static void framebufferSizeCallback(
            GLFWwindow* window,
            int width,
            int height
        );

        static void keyCallback(
            GLFWwindow* window,
            int key,
            int scancode,
            int action,
            int mods
        );
    };
}