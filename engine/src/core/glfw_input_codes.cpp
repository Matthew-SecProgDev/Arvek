#include "glfw_input_codes.h"

#include <GLFW/glfw3.h>

namespace arvek
{
    std::optional<Key> translateGlfwKey(int key) noexcept
    {
        switch (key)
        {
        case GLFW_KEY_W:
            return Key::W;
        case GLFW_KEY_S:
            return Key::S;
        case GLFW_KEY_A:
            return Key::A;
        case GLFW_KEY_D:
            return Key::D;
        case GLFW_KEY_Q:
            return Key::Q;
        case GLFW_KEY_E:
            return Key::E;
        case GLFW_KEY_R:
            return Key::R;
        case GLFW_KEY_F:
            return Key::F;
        case GLFW_KEY_SPACE:
            return Key::Space;
        case GLFW_KEY_ESCAPE:
            return Key::Escape;
        case GLFW_KEY_ENTER:
            return Key::Enter;
        case GLFW_KEY_UP:
            return Key::Up;
        case GLFW_KEY_DOWN:
            return Key::Down;
        case GLFW_KEY_LEFT:
            return Key::Left;
        case GLFW_KEY_RIGHT:
            return Key::Right;
        case GLFW_KEY_LEFT_SHIFT:
            return Key::LeftShift;
        case GLFW_KEY_RIGHT_SHIFT:
            return Key::RightShift;
        case GLFW_KEY_LEFT_CONTROL:
            return Key::LeftControl;
        case GLFW_KEY_RIGHT_CONTROL:
            return Key::RightControl;
        default:
            return std::nullopt;
        }
    }

    std::optional<MouseButton> translateGlfwMouseButton(int button) noexcept
    {
        switch (button)
        {
        case GLFW_MOUSE_BUTTON_LEFT:
            return MouseButton::Left;
        case GLFW_MOUSE_BUTTON_RIGHT:
            return MouseButton::Right;
        case GLFW_MOUSE_BUTTON_MIDDLE:
            return MouseButton::Middle;
        default:
            return std::nullopt;
        }
    }
}
