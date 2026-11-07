#pragma once

#include <arvek/core/window_extent.h>

#include <memory>

namespace arvek
{
    class Engine;
    class WindowBackend;
    class Surface;

    class Window final
    {
    public:
        ~Window();

        Window(const Window&) = delete;
        Window& operator=(const Window&) = delete;
        Window(Window&&) = delete;
        Window& operator=(Window&&) = delete;

        [[nodiscard]]
        bool shouldClose() const;

        void pollEvents();
        void waitEvents();

        [[nodiscard]]
        WindowExtent framebufferExtent() const;

        [[nodiscard]]
        bool wasResized() const;

        void acknowledgeResize();

    private:
        friend class Engine;
        friend class Surface;

        explicit Window(std::unique_ptr<WindowBackend> backend);

        std::unique_ptr<WindowBackend> backend_;
    };
}