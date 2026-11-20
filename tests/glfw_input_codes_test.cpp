#include "glfw_input_codes.h"

#include <GLFW/glfw3.h>

#include <cstdlib>
#include <iostream>
#include <stdexcept>

namespace
{
    void check(bool condition, const char* message)
    {
        if (!condition)
        {
            throw std::runtime_error(message);
        }
    }
}

int main()
{
    using namespace arvek;

    try
    {
        // Test 1: Movement keys.
        check(
            translateGlfwKey(GLFW_KEY_W) == Key::W,
            "W translation failed."
        );

        check(
            translateGlfwKey(GLFW_KEY_A) == Key::A,
            "A translation failed."
        );

        check(
            translateGlfwKey(GLFW_KEY_S) == Key::S,
            "S translation failed."
        );

        check(
            translateGlfwKey(GLFW_KEY_D) == Key::D,
            "D translation failed."
        );

        check(
            translateGlfwKey(GLFW_KEY_UP) == Key::Up,
            "Up translation failed."
        );

        check(
            translateGlfwKey(GLFW_KEY_DOWN) == Key::Down,
            "Down translation failed."
        );

        check(
            translateGlfwKey(GLFW_KEY_LEFT) == Key::Left,
            "Left translation failed."
        );

        check(
            translateGlfwKey(GLFW_KEY_RIGHT) == Key::Right,
            "Right translation failed."
        );

        // Test 2: Action keys.
        check(
            translateGlfwKey(GLFW_KEY_SPACE) == Key::Space,
            "Space translation failed."
        );

        check(
            translateGlfwKey(GLFW_KEY_ESCAPE) == Key::Escape,
            "Escape translation failed."
        );

        check(
            translateGlfwKey(GLFW_KEY_ENTER) == Key::Enter,
            "Enter translation failed."
        );

        // Test 3: Modifier keys.
        check(
            translateGlfwKey(GLFW_KEY_LEFT_SHIFT) == Key::LeftShift,
            "LeftShift translation failed."
        );

        check(
            translateGlfwKey(GLFW_KEY_RIGHT_SHIFT) == Key::RightShift,
            "RightShift translation failed."
        );

        check(
            translateGlfwKey(GLFW_KEY_LEFT_CONTROL) == Key::LeftControl,
            "LeftControl translation failed."
        );

        check(
            translateGlfwKey(GLFW_KEY_RIGHT_CONTROL) == Key::RightControl,
            "RightControl translation failed."
        );

        // Test 4: Other keys.
        check(
            translateGlfwKey(GLFW_KEY_Q) == Key::Q,
            "Q translation failed."
        );

        check(
            translateGlfwKey(GLFW_KEY_E) == Key::E,
            "E translation failed."
        );

        check(
            translateGlfwKey(GLFW_KEY_R) == Key::R,
            "R translation failed."
        );

        check(
            translateGlfwKey(GLFW_KEY_F) == Key::F,
            "F translation failed."
        );

        // Test 5: Unsupported keyboard keys.
        check(
            !translateGlfwKey(GLFW_KEY_F1).has_value(),
            "Unsupported key was accepted."
        );

        check(
            !translateGlfwKey(-1).has_value(),
            "Invalid key was accepted."
        );

        // Test 6: Mouse buttons.
        check(
            translateGlfwMouseButton(GLFW_MOUSE_BUTTON_LEFT) == MouseButton::Left,
            "Left mouse button translation failed."
        );

        check(
            translateGlfwMouseButton(GLFW_MOUSE_BUTTON_RIGHT) == MouseButton::Right,
            "Right mouse button translation failed."
        );

        check(
            translateGlfwMouseButton(GLFW_MOUSE_BUTTON_MIDDLE) == MouseButton::Middle,
            "Middle mouse button translation failed."
        );

        // Test 7: Unsupported mouse buttons.
        check(
            !translateGlfwMouseButton(GLFW_MOUSE_BUTTON_4).has_value(),
            "Unsupported mouse button was accepted."
        );

        check(
            !translateGlfwMouseButton(-1).has_value(),
            "Invalid mouse button was accepted."
        );

        std::cout << "All GLFW input translation tests passed.\n";
        return EXIT_SUCCESS;
    }
    catch (const std::exception& exception)
    {
        std::cerr
            << "GLFW input translation test failed: "
            << exception.what()
            << '\n';

        return EXIT_FAILURE;
    }
}
