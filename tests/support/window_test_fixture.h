#pragma once

#include <memory>

namespace arvek
{
    class GlfwRuntime;
    class Window;

    class WindowTestFixture final
    {
    public:
        WindowTestFixture();
        ~WindowTestFixture();

        WindowTestFixture(const WindowTestFixture&) = delete;
        WindowTestFixture& operator=(const WindowTestFixture&) = delete;
        WindowTestFixture(WindowTestFixture&&) = delete;
        WindowTestFixture& operator=(WindowTestFixture&&) = delete;

        [[nodiscard]]
        Window& window() noexcept;

    private:
        std::unique_ptr<GlfwRuntime> runtime_;
        std::unique_ptr<Window> window_;
    };
}