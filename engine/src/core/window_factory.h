#pragma once

#include <memory>

namespace arvek
{
    class Window;
    struct WindowConfig;

    class WindowFactory final
    {
    public:
        [[nodiscard]]
        static std::unique_ptr<Window> create(const WindowConfig& config);
    };
}