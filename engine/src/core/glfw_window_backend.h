#pragma once

#include "window_backend.h"
#include "../rendering/surface_provider.h"
#include "keyboard_state.h"
#include "mouse_state.h"

struct GLFWwindow;

namespace arvek
{
    struct WindowConfig;

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

        [[nodiscard]]
        const KeyboardState& keyboardState() const noexcept override;

        [[nodiscard]]
        const MouseState& mouseState() const noexcept override;

    private:
        GLFWwindow* window_{ nullptr };
        bool resized_{ false };

        KeyboardState keyboardState_;
        MouseState mouseState_;

        static void framebufferSizeCallback(
            GLFWwindow* glfwWindow,
            int width,
            int height
        );

        static void keyCallback(
            GLFWwindow* glfwWindow,
            int glfwKey,
            int scancode,
            int action,
            int mods
        );

        static void mouseButtonCallback(
            GLFWwindow* glfwWindow,
            int glfwButton,
            int action,
            int mods
        );

        static void cursorPositionCallback(
            GLFWwindow* glfwWindow,
            double x,
            double y
        );

        static void scrollCallback(
            GLFWwindow* glfwWindow,
            double xOffset,
            double yOffset
        );

        static void windowFocusCallback(
            GLFWwindow* glfwWindow,
            int focused
        );
    };
}
