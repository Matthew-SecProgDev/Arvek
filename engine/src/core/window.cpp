#include <arvek/core/window.h>

#include "window_backend.h"

#include <stdexcept>
#include <utility>

namespace arvek
{
    Window::Window(std::unique_ptr<WindowBackend> backend)
        : backend_(std::move(backend))
    {
        if (!backend_)
        {
            throw std::invalid_argument(
                "Window backend cannot be null"
            );
        }
    }

    Window::~Window() = default;

    bool Window::shouldClose() const
    {
        return backend_->shouldClose();
    }

    void Window::pollEvents()
    {
        backend_->pollEvents();
    }

    void Window::waitEvents()
    {
        backend_->waitEvents();
    }

    WindowExtent Window::framebufferExtent() const
    {
        return backend_->framebufferExtent();
    }

    bool Window::wasResized() const
    {
        return backend_->wasResized();
    }

    void Window::acknowledgeResize()
    {
        backend_->acknowledgeResize();
    }
}