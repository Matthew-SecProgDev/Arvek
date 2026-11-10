#include "window_test_fixture.h"

#include "glfw_runtime.h"
#include "window_config.h"
#include "window_factory.h"
#include <arvek/core/window.h>

#include <memory>
#include <utility>

namespace arvek
{
    WindowTestFixture::WindowTestFixture()
    {
        auto runtime = std::make_unique<GlfwRuntime>();

        WindowConfig config;
        config.width = 800;
        config.height = 600;
        config.title = "Arvek Vulkan Test";

        auto window = WindowFactory::create(config);

        runtime_ = std::move(runtime);
        window_ = std::move(window);
    }

    WindowTestFixture::~WindowTestFixture() = default;

    Window& WindowTestFixture::window() noexcept
    {
        return *window_;
    }
}