#include "keyboard_state.h"

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
        KeyboardState keyboard;

        constexpr std::size_t key = 65;

        // Initial state.
        check(
            !keyboard.isDown(key),
            "Key must initially be up."
        );

        // Press.
        keyboard.beginFrame();
        keyboard.handleKeyPress(key);

        check(
            keyboard.isDown(key),
            "Key should be down."
        );

        check(
            keyboard.pressedThisFrame(key),
            "Press transition missing."
        );

        // Repeated press while held.
        keyboard.handleKeyPress(key);

        check(
            keyboard.pressedThisFrame(key),
            "Press event should remain recorded."
        );

        // Next frame.
        keyboard.beginFrame();

        check(
            keyboard.isDown(key),
            "Held key was lost."
        );

        check(
            !keyboard.pressedThisFrame(key),
            "Press transition was not cleared."
        );

        // Release.
        keyboard.handleKeyRelease(key);

        check(
            !keyboard.isDown(key),
            "Key should be up."
        );

        check(
            keyboard.releasedThisFrame(key),
            "Release transition missing."
        );

        // Press and release within one frame.
        keyboard.beginFrame();
        keyboard.handleKeyPress(key);
        keyboard.handleKeyRelease(key);

        check(
            keyboard.pressedThisFrame(key),
            "Short press was lost."
        );

        check(
            keyboard.releasedThisFrame(key),
            "Short release was lost."
        );

        check(
            !keyboard.isDown(key),
            "Key should no longer be held."
        );

        // Out-of-range key.
        keyboard.handleKeyPress(KeyboardState::KeyCount);

        check(
            !keyboard.isDown(KeyboardState::KeyCount),
            "Invalid key was accepted."
        );

        // Clear everything.
        keyboard.clear();

        check(
            !keyboard.isDown(key) &&
            !keyboard.pressedThisFrame(key) &&
            !keyboard.releasedThisFrame(key),
            "Clear did not reset keyboard state."
        );

        std::cout << "KeyboardState tests passed.\n";
        return EXIT_SUCCESS;
    }
    catch (const std::exception& exception)
    {
        std::cerr << exception.what() << '\n';
        return EXIT_FAILURE;
    }
}
