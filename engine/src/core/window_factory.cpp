#include "window_factory.h"

#include "glfw_window_backend.h"
#include <arvek/core/window.h>

#include <memory>
#include <utility>

namespace arvek
{
    std::unique_ptr<Window> WindowFactory::create(const WindowConfig& config)
    {
        auto backend = std::make_unique<GlfwWindowBackend>(config);

        return std::unique_ptr<Window>(new Window(std::move(backend)));
    }
}