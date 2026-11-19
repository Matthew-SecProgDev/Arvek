#include "keyboard_state.h"

#include <cstddef>
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
        constexpr std::size_t anotherKey = 87;

        // Test 1: Initial state.
        check(
            !keyboard.isDown(key),
            "Key must initially be up."
        );

        check(
            !keyboard.pressedThisFrame(key),
            "Key must not initially be pressed."
        );

        check(
            !keyboard.releasedThisFrame(key),
            "Key must not initially be released."
        );

        // Test 2: Press events remain pending until beginFrame() publishes them.
        keyboard.handleKeyPress(key);

        check(
            keyboard.isDown(key),
            "Key should be down immediately after press."
        );

        check(
            !keyboard.pressedThisFrame(key),
            "Unpublished press became visible."
        );

        check(
            !keyboard.releasedThisFrame(key),
            "Unexpected release transition."
        );

        // Publish pending input.
        keyboard.beginFrame();

        check(
            keyboard.isDown(key),
            "Held state was lost after publication."
        );

        check(
            keyboard.pressedThisFrame(key),
            "Pending press was not published."
        );

        check(
            !keyboard.releasedThisFrame(key),
            "Unexpected release after publication."
        );

        // Test 3: The next frame clears published  transitions but preserves held keys.
        keyboard.beginFrame();

        check(
            keyboard.isDown(key),
            "Held key was lost between frames."
        );

        check(
            !keyboard.pressedThisFrame(key),
            "Press transition was not cleared."
        );

        check(
            !keyboard.releasedThisFrame(key),
            "Unexpected release transition."
        );

        // Test 4: Repeated presses while held must not generate another press transition.
        keyboard.handleKeyPress(key);
        keyboard.beginFrame();

        check(
            keyboard.isDown(key),
            "Repeated press changed held state."
        );

        check(
            !keyboard.pressedThisFrame(key),
            "Repeated press generated a new transition."
        );

        // Test 5: Release events are published on the next input frame.
        keyboard.handleKeyRelease(key);

        check(
            !keyboard.isDown(key),
            "Key should be up immediately after release."
        );

        check(
            !keyboard.releasedThisFrame(key),
            "Unpublished release became visible."
        );

        keyboard.beginFrame();

        check(
            !keyboard.isDown(key),
            "Released key remained down."
        );

        check(
            keyboard.releasedThisFrame(key),
            "Release transition was not published."
        );

        check(
            !keyboard.pressedThisFrame(key),
            "Unexpected press transition."
        );

        // Test 6: Release transition disappears after the following frame.
        keyboard.beginFrame();

        check(
            !keyboard.releasedThisFrame(key),
            "Release transition survived another frame."
        );

        check(
            !keyboard.isDown(key),
            "Released key became held."
        );

        // Test 7: Releasing an already released key
        // must not generate another release transition.
        keyboard.handleKeyRelease(key);
        keyboard.beginFrame();

        check(
            !keyboard.releasedThisFrame(key),
            "Repeated release generated a transition."
        );

        // Test 8: Press and release within the
        // same frame must preserve both transitions.
        keyboard.clear();

        keyboard.handleKeyPress(key);
        keyboard.handleKeyRelease(key);

        check(
            !keyboard.isDown(key),
            "Key should be up after short press."
        );

        check(
            !keyboard.pressedThisFrame(key),
            "Short press was published prematurely."
        );

        check(
            !keyboard.releasedThisFrame(key),
            "Short release was published prematurely."
        );

        keyboard.beginFrame();

        check(
            keyboard.pressedThisFrame(key),
            "Short press transition was lost."
        );

        check(
            keyboard.releasedThisFrame(key),
            "Short release transition was lost."
        );

        check(
            !keyboard.isDown(key),
            "Key remained down after short press."
        );

        // Test 9: Multiple keys must maintain independent states.
        keyboard.clear();

        keyboard.handleKeyPress(key);
        keyboard.handleKeyPress(anotherKey);

        keyboard.beginFrame();

        check(
            keyboard.isDown(key),
            "First key should be down."
        );

        check(
            keyboard.isDown(anotherKey),
            "Second key should be down."
        );

        check(
            keyboard.pressedThisFrame(key),
            "First key press was lost."
        );

        check(
            keyboard.pressedThisFrame(anotherKey),
            "Second key press was lost."
        );

        keyboard.handleKeyRelease(key);
        keyboard.beginFrame();

        check(
            !keyboard.isDown(key),
            "First key should be released."
        );

        check(keyboard.releasedThisFrame(key),
            "First key release was lost.");

        check(
            keyboard.isDown(anotherKey),
            "Second key should remain down."
        );

        check(
            !keyboard.releasedThisFrame(anotherKey),
            "Second key was unexpectedly released."
        );

        // Test 10: Pending events must survive until beginFrame() is called.
        keyboard.clear();

        keyboard.handleKeyPress(key);

        check(
            !keyboard.pressedThisFrame(key),
            "Pending press was published too early."
        );

        keyboard.beginFrame();

        check(
            keyboard.pressedThisFrame(key),
            "Pending press was lost."
        );

        // Test 11: clear() removes both pending and published transitions.
        keyboard.clear();

        keyboard.handleKeyPress(key);

        // Clear pending events before publication.
        keyboard.clear();
        keyboard.beginFrame();

        check(
            !keyboard.isDown(key),
            "Clear did not reset held state."
        );

        check(
            !keyboard.pressedThisFrame(key),
            "Clear did not discard pending press."
        );

        check(
            !keyboard.releasedThisFrame(key),
            "Clear did not discard pending release."
        );

        // Also test clearing published transitions.
        keyboard.handleKeyPress(key);
        keyboard.beginFrame();

        check(
            keyboard.pressedThisFrame(key),
            "Expected a published press."
        );

        keyboard.clear();

        check(
            !keyboard.isDown(key),
            "Clear did not reset held key."
        );

        check(
            !keyboard.pressedThisFrame(key),
            "Clear did not reset published press."
        );

        check(
            !keyboard.releasedThisFrame(key),
            "Clear did not reset published release."
        );

        // Test 12: Out-of-range key indices must be ignored safely.
        constexpr std::size_t invalidKey = KeyboardState::KeyCount;

        keyboard.handleKeyPress(invalidKey);
        keyboard.handleKeyRelease(invalidKey);

        keyboard.beginFrame();

        check(
            !keyboard.isDown(invalidKey),
            "Invalid key was reported as down."
        );

        check(
            !keyboard.pressedThisFrame(invalidKey),
            "Invalid key generated a press."
        );

        check(
            !keyboard.releasedThisFrame(invalidKey),
            "Invalid key generated a release."
        );

        // Test 13: Last valid key index must be supported.
        constexpr std::size_t lastValidKey = KeyboardState::KeyCount - 1;

        keyboard.handleKeyPress(lastValidKey);
        keyboard.beginFrame();

        check(
            keyboard.isDown(lastValidKey),
            "Last valid key was not held."
        );

        check(
            keyboard.pressedThisFrame(lastValidKey),
            "Last valid key press was lost."
        );

        keyboard.handleKeyRelease(lastValidKey);
        keyboard.beginFrame();

        check(
            !keyboard.isDown(lastValidKey),
            "Last valid key remained down."
        );

        check(
            keyboard.releasedThisFrame(lastValidKey),
            "Last valid key release was lost."
        );

        // Test 14: A new instance must have independent, empty keyboard state.
        KeyboardState anotherKeyboard;

        check(
            !anotherKeyboard.isDown(key),
            "New keyboard inherited held state."
        );

        check(
            !anotherKeyboard.pressedThisFrame(key),
            "New keyboard inherited a press."
        );

        check(
            !anotherKeyboard.releasedThisFrame(key),
            "New keyboard inherited a release."
        );

        std::cout << "All KeyboardState tests passed.\n";
        return EXIT_SUCCESS;
    }
    catch (const std::exception& exception)
    {
        std::cerr << "KeyboardState test failed: " << exception.what() << '\n';
        return EXIT_FAILURE;
    }
}
