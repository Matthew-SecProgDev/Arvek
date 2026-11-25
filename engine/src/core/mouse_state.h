#pragma once

#include <arvek/input/mouse_button.h>
#include <arvek/input/mouse_position.h>

#include <array>
#include <cstddef>

namespace arvek
{
    class MouseState final
    {
    public:
        // This is an internal capacity, not a public API.
        static constexpr std::size_t ButtonCount = static_cast<std::size_t>(MouseButton::Count);

        // Publish pending mouse events for this frame.
        void beginFrame() noexcept;

        void handlePress(MouseButton button) noexcept;

        void handleRelease(MouseButton button) noexcept;

        void handleCursorMove(double x, double y) noexcept;

        void handleScroll(double xOffset, double yOffset) noexcept;

        [[nodiscard]]
        bool isDown(MouseButton button) const noexcept;

        [[nodiscard]]
        bool wasPressed(MouseButton button) const noexcept;

        [[nodiscard]]
        bool wasReleased(MouseButton button) const noexcept;

        [[nodiscard]]
        MousePosition cursorPosition() const noexcept;

        [[nodiscard]]
        MousePosition cursorDelta() const noexcept;

        [[nodiscard]]
        MousePosition scrollDelta() const noexcept;

        [[nodiscard]]
        bool hasCursorPosition() const noexcept;

        // Discard the previous cursor baseline.
        // Useful after focus changes or cursor-mode changes.
        void resetCursorBaseline() noexcept;

        void clear() noexcept;

    private:
        std::array<bool, ButtonCount> down_{};
        std::array<bool, ButtonCount> pressed_{};
        std::array<bool, ButtonCount> released_{};

        std::array<bool, ButtonCount> pendingPressed_{};
        std::array<bool, ButtonCount> pendingReleased_{};

        MousePosition position_{};
        MousePosition delta_{};
        MousePosition pendingDelta_{};

        MousePosition scrollDelta_{};
        MousePosition pendingScrollDelta_{};

        bool hasPosition_{ false };
    };
}
