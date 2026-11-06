#pragma once

#include <arvek/core/window_extent.h>

namespace arvek
{
    class WindowBackend
    {
    public:
        virtual ~WindowBackend() = default;

        [[nodiscard]]
        virtual bool shouldClose() const = 0;

        virtual void pollEvents() = 0;

        virtual void waitEvents() = 0;

        [[nodiscard]]
        virtual WindowExtent framebufferExtent() const = 0;

        [[nodiscard]]
        virtual bool wasResized() const = 0;

        virtual void acknowledgeResize() = 0;
    };
}