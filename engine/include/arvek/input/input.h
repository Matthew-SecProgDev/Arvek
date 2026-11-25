#pragma once

#include <arvek/input/key.h>
#include <arvek/input/mouse_button.h>
#include <arvek/input/mouse_position.h>

namespace arvek
{
    class Engine;
    class Window;

    class Input final
    {
    public:
        Input(const Input&) = delete;
        Input& operator=(const Input&) = delete;

        [[nodiscard]]
        bool isKeyDown(Key key) const noexcept;

        [[nodiscard]]
        bool wasKeyPressed(Key key) const noexcept;

        [[nodiscard]]
        bool wasKeyReleased(Key key) const noexcept;

        [[nodiscard]]
        bool isMouseButtonDown(MouseButton button) const noexcept;

        [[nodiscard]]
        bool wasMouseButtonPressed(MouseButton button) const noexcept;

        [[nodiscard]]
        bool wasMouseButtonReleased(MouseButton button) const noexcept;

        [[nodiscard]]
        MousePosition cursorPosition() const noexcept;

        // Movement accumulated during the current input frame
        [[nodiscard]]
        MousePosition cursorDelta() const noexcept;

        // Scroll accumulated during the current input frame
        [[nodiscard]]
        MousePosition scrollDelta() const noexcept;

        // Whether a cursor position has been received
        [[nodiscard]]
        bool hasCursorPosition() const noexcept;

    private:
        friend class Engine;

        explicit Input(const Window& window) noexcept;

        const Window& window_;
    };
}
