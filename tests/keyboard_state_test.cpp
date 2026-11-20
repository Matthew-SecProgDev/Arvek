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
            !keyboard.wasPressed(key),
            "Key must not initially be pressed."
        );

        check(
            !keyboard.wasReleased(key),
            "Key must not initially be released."
        );

        // Test 2: Press events remain pending until beginFrame() publishes them.
        keyboard.handlePress(key);

        check(
            keyboard.isDown(key),
            "Key should be down immediately after press."
        );

        check(
            !keyboard.wasPressed(key),
            "Unpublished press became visible."
        );

        check(
            !keyboard.wasReleased(key),
            "Unexpected release transition."
        );

        // Publish pending input.
        keyboard.beginFrame();

        check(
            keyboard.isDown(key),
            "Held state was lost after publication."
        );

        check(
            keyboard.wasPressed(key),
            "Pending press was not published."
        );

        check(
            !keyboard.wasReleased(key),
            "Unexpected release after publication."
        );

        // Test 3: The next frame clears published  transitions but preserves held keys.
        keyboard.beginFrame();

        check(
            keyboard.isDown(key),
            "Held key was lost between frames."
        );

        check(
            !keyboard.wasPressed(key),
            "Press transition was not cleared."
        );

        check(
            !keyboard.wasReleased(key),
            "Unexpected release transition."
        );

        // Test 4: Repeated presses while held must not generate another press transition.
        keyboard.handlePress(key);
        keyboard.beginFrame();

        check(
            keyboard.isDown(key),
            "Repeated press changed held state."
        );

        check(
            !keyboard.wasPressed(key),
            "Repeated press generated a new transition."
        );

        // Test 5: Release events are published on the next input frame.
        keyboard.handleRelease(key);

        check(
            !keyboard.isDown(key),
            "Key should be up immediately after release."
        );

        check(
            !keyboard.wasReleased(key),
            "Unpublished release became visible."
        );

        keyboard.beginFrame();

        check(
            !keyboard.isDown(key),
            "Released key remained down."
        );

        check(
            keyboard.wasReleased(key),
            "Release transition was not published."
        );

        check(
            !keyboard.wasPressed(key),
            "Unexpected press transition."
        );

        // Test 6: Release transition disappears after the following frame.
        keyboard.beginFrame();

        check(
            !keyboard.wasReleased(key),
            "Release transition survived another frame."
        );

        check(
            !keyboard.isDown(key),
            "Released key became held."
        );

        // Test 7: Releasing an already released key
        // must not generate another release transition.
        keyboard.handleRelease(key);
        keyboard.beginFrame();

        check(
            !keyboard.wasReleased(key),
            "Repeated release generated a transition."
        );

        // Test 8: Press and release within the
        // same frame must preserve both transitions.
        keyboard.clear();

        keyboard.handlePress(key);
        keyboard.handleRelease(key);

        check(
            !keyboard.isDown(key),
            "Key should be up after short press."
        );

        check(
            !keyboard.wasPressed(key),
            "Short press was published prematurely."
        );

        check(
            !keyboard.wasReleased(key),
            "Short release was published prematurely."
        );

        keyboard.beginFrame();

        check(
            keyboard.wasPressed(key),
            "Short press transition was lost."
        );

        check(
            keyboard.wasReleased(key),
            "Short release transition was lost."
        );

        check(
            !keyboard.isDown(key),
            "Key remained down after short press."
        );

        // Test 9: Multiple keys must maintain independent states.
        keyboard.clear();

        keyboard.handlePress(key);
        keyboard.handlePress(anotherKey);

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
            keyboard.wasPressed(key),
            "First key press was lost."
        );

        check(
            keyboard.wasPressed(anotherKey),
            "Second key press was lost."
        );

        keyboard.handleRelease(key);
        keyboard.beginFrame();

        check(
            !keyboard.isDown(key),
            "First key should be released."
        );

        check(keyboard.wasReleased(key),
            "First key release was lost.");

        check(
            keyboard.isDown(anotherKey),
            "Second key should remain down."
        );

        check(
            !keyboard.wasReleased(anotherKey),
            "Second key was unexpectedly released."
        );

        // Test 10: Pending events must survive until beginFrame() is called.
        keyboard.clear();

        keyboard.handlePress(key);

        check(
            !keyboard.wasPressed(key),
            "Pending press was published too early."
        );

        keyboard.beginFrame();

        check(
            keyboard.wasPressed(key),
            "Pending press was lost."
        );

        // Test 11: clear() removes both pending and published transitions.
        keyboard.clear();

        keyboard.handlePress(key);

        // Clear pending events before publication.
        keyboard.clear();
        keyboard.beginFrame();

        check(
            !keyboard.isDown(key),
            "Clear did not reset held state."
        );

        check(
            !keyboard.wasPressed(key),
            "Clear did not discard pending press."
        );

        check(
            !keyboard.wasReleased(key),
            "Clear did not discard pending release."
        );

        // Also test clearing published transitions.
        keyboard.handlePress(key);
        keyboard.beginFrame();

        check(
            keyboard.wasPressed(key),
            "Expected a published press."
        );

        keyboard.clear();

        check(
            !keyboard.isDown(key),
            "Clear did not reset held key."
        );

        check(
            !keyboard.wasPressed(key),
            "Clear did not reset published press."
        );

        check(
            !keyboard.wasReleased(key),
            "Clear did not reset published release."
        );

        // Test 12: Out-of-range key indices must be ignored safely.
        constexpr std::size_t invalidKey = KeyboardState::KeyCount;

        keyboard.handlePress(invalidKey);
        keyboard.handleRelease(invalidKey);

        keyboard.beginFrame();

        check(
            !keyboard.isDown(invalidKey),
            "Invalid key was reported as down."
        );

        check(
            !keyboard.wasPressed(invalidKey),
            "Invalid key generated a press."
        );

        check(
            !keyboard.wasReleased(invalidKey),
            "Invalid key generated a release."
        );

        // Test 13: Last valid key index must be supported.
        constexpr std::size_t lastValidKey = KeyboardState::KeyCount - 1;

        keyboard.handlePress(lastValidKey);
        keyboard.beginFrame();

        check(
            keyboard.isDown(lastValidKey),
            "Last valid key was not held."
        );

        check(
            keyboard.wasPressed(lastValidKey),
            "Last valid key press was lost."
        );

        keyboard.handleRelease(lastValidKey);
        keyboard.beginFrame();

        check(
            !keyboard.isDown(lastValidKey),
            "Last valid key remained down."
        );

        check(
            keyboard.wasReleased(lastValidKey),
            "Last valid key release was lost."
        );

        // Test 14: A new instance must have independent, empty keyboard state.
        KeyboardState anotherKeyboard;

        check(
            !anotherKeyboard.isDown(key),
            "New keyboard inherited held state."
        );

        check(
            !anotherKeyboard.wasPressed(key),
            "New keyboard inherited a press."
        );

        check(
            !anotherKeyboard.wasReleased(key),
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
