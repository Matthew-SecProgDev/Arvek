#include <arvek/input/input.h>

#include <arvek/core/window.h>
#include "keyboard_state.h"
#include "mouse_state.h"

namespace arvek
{
    Input::Input(const Window& window) noexcept : window_(window)
    {
    }

    bool Input::isKeyDown(Key key) const noexcept
    {
        return window_.keyboardState().isDown(key);
    }

    bool Input::wasKeyPressed(Key key) const noexcept
    {
        return window_.keyboardState().wasPressed(key);
    }

    bool Input::wasKeyReleased(Key key) const noexcept
    {
        return window_.keyboardState().wasReleased(key);
    }

    bool Input::isMouseButtonDown(MouseButton button) const noexcept
    {
        return window_.mouseState().isDown(button);
    }

    bool Input::wasMouseButtonPressed(MouseButton button) const noexcept
    {
        return window_.mouseState().wasPressed(button);
    }

    bool Input::wasMouseButtonReleased(MouseButton button) const noexcept
    {
        return window_.mouseState().wasReleased(button);
    }
}
