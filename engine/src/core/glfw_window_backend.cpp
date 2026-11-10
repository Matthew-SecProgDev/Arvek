#include "glfw_window_backend.h"

#include "window_config.h"

#include <GLFW/glfw3.h>

#include <cstdint>
#include <stdexcept>
#include <string>

namespace arvek
{
    GlfwWindowBackend::GlfwWindowBackend(const WindowConfig& config)
    {
        if (config.width <= 0 || config.height <= 0)
        {
            throw std::invalid_argument(
                "Window dimensions must be positive!"
            );
        }

        if (config.title.empty())
        {
            throw std::invalid_argument(
                "Window title must not be empty!"
            );
        }

        glfwDefaultWindowHints();

        // We will use Vulkan, not OpenGL.
        glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

        glfwWindowHint(
            GLFW_RESIZABLE,
            config.resizable ? GLFW_TRUE : GLFW_FALSE
        );

        window_ = glfwCreateWindow(
            config.width,
            config.height,
            config.title.c_str(),
            nullptr,
            nullptr
        );

        if (!window_)
        {
            throw std::runtime_error(
                "Failed to create GLFW window!"
            );
        }

        glfwSetWindowUserPointer(window_, this);

        glfwSetFramebufferSizeCallback(
            window_,
            framebufferSizeCallback
        );

        glfwSetKeyCallback(
            window_,
            keyCallback
        );

        if (config.centered)
        {
            if (GLFWmonitor* monitor = glfwGetPrimaryMonitor())
            {
                int x, y, width, height;

                glfwGetMonitorWorkarea(
                    monitor,
                    &x,
                    &y,
                    &width,
                    &height
                );

                int windowWidth, windowHeight;

                glfwGetWindowSize(
                    window_,
                    &windowWidth,
                    &windowHeight
                );

                glfwSetWindowPos(
                    window_,
                    x + ((width - windowWidth) / 2),
                    y + ((height - windowHeight) / 2)
                );
            }
        }
    }

    GlfwWindowBackend::~GlfwWindowBackend()
    {
        if (window_)
        {
            glfwDestroyWindow(window_);
        }
    }

    bool GlfwWindowBackend::shouldClose() const
    {
        return glfwWindowShouldClose(window_) == GLFW_TRUE;
    }

    void GlfwWindowBackend::pollEvents()
    {
        glfwPollEvents();
    }

    void GlfwWindowBackend::waitEvents()
    {
        glfwWaitEvents();
    }

    VkSurfaceKHR GlfwWindowBackend::createSurface(VkInstance instance) const
    {
        if (!window_)
        {
            throw std::logic_error(
                "Cannot create Vulkan surface: GLFW window is null!"
            );
        }

        if (instance == VK_NULL_HANDLE)
        {
            throw std::invalid_argument(
                "Cannot create Vulkan surface: instance is null!"
            );
        }

        VkSurfaceKHR surface = VK_NULL_HANDLE;

        const VkResult result = glfwCreateWindowSurface(
            instance,
            window_,
            nullptr,
            &surface
        );

        if (result != VK_SUCCESS)
        {
            throw std::runtime_error(
                "Failed to create Vulkan window surface. VkResult: " +
                std::to_string(static_cast<int>(result))
            );
        }

        return surface;
    }

    WindowExtent GlfwWindowBackend::framebufferExtent() const
    {
        int width = 0;
        int height = 0;

        glfwGetFramebufferSize(
            window_,
            &width,
            &height
        );

        return WindowExtent{
            static_cast<std::uint32_t>(width),
            static_cast<std::uint32_t>(height)
        };
    }

    bool GlfwWindowBackend::wasResized() const
    {
        return resized_;
    }

    void GlfwWindowBackend::acknowledgeResize()
    {
        resized_ = false;
    }

    void GlfwWindowBackend::framebufferSizeCallback(
        GLFWwindow* window,
        int,
        int)
    {
        auto* backend = static_cast<GlfwWindowBackend*>(glfwGetWindowUserPointer(window));

        if (backend)
        {
            backend->resized_ = true;
        }
    }

    void GlfwWindowBackend::keyCallback(
        GLFWwindow* window,
        int key,
        int,
        int action,
        int)
    {
        if (key == GLFW_KEY_ESCAPE &&
            action == GLFW_PRESS)
        {
            glfwSetWindowShouldClose(window, GLFW_TRUE);
        }
    }
}