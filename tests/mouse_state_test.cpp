#include "mouse_state.h"

#include <cmath>
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

    bool approximatelyEqual(double a, double b)
    {
        return std::abs(a - b) < 1e-9;
    }
}

int main()
{
    using namespace arvek;

    try
    {
        MouseState mouse;

        constexpr MouseButton leftButton = MouseButton::Left;
        constexpr MouseButton rightButton = MouseButton::Right;

        // Test 1: Initial state.
        check(
            !mouse.isDown(leftButton),
            "Button initially down."
        );

        check(
            !mouse.wasPressed(leftButton),
            "Unexpected initial press."
        );

        check(
            !mouse.wasReleased(leftButton),
            "Unexpected initial release."
        );

        check(
            !mouse.hasCursorPosition(),
            "Cursor position unexpectedly initialized."
        );

        // Test 2: Button press is published.
        mouse.handlePress(leftButton);

        check(
            mouse.isDown(leftButton),
            "Pressed button should be down."
        );

        check(
            !mouse.wasPressed(leftButton),
            "Press published prematurely."
        );

        mouse.beginFrame();

        check(
            mouse.wasPressed(leftButton),
            "Mouse press was lost."
        );

        // Test 3: Held state persists.
        mouse.beginFrame();

        check(
            mouse.isDown(leftButton),
            "Held button was lost."
        );

        check(
            !mouse.wasPressed(leftButton),
            "Press transition remained visible."
        );

        // Test 4: Repeat press while held.
        mouse.handlePress(leftButton);
        mouse.beginFrame();

        check(
            !mouse.wasPressed(leftButton),
            "Repeated press created transition."
        );

        // Test 5: Button release.
        mouse.handleRelease(leftButton);
        mouse.beginFrame();

        check(
            !mouse.isDown(leftButton),
            "Released button remained down."
        );

        check(
            mouse.wasReleased(leftButton),
            "Release transition was lost."
        );

        // Test 6: Short press and release.
        mouse.clear();

        mouse.handlePress(leftButton);
        mouse.handleRelease(leftButton);
        mouse.beginFrame();

        check(
            mouse.wasPressed(leftButton),
            "Short press lost."
        );

        check(
            mouse.wasReleased(leftButton),
            "Short release lost."
        );

        check(
            !mouse.isDown(leftButton),
            "Button remained held."
        );

        // Test 7: Independent mouse buttons.
        mouse.clear();

        mouse.handlePress(leftButton);
        mouse.handlePress(rightButton);
        mouse.handleRelease(leftButton);

        mouse.beginFrame();

        check(
            !mouse.isDown(leftButton),
            "Left button should be up."
        );

        check(
            mouse.isDown(rightButton),
            "Right button should remain down."
        );

        check(
            mouse.wasPressed(rightButton),
            "Right button press missing."
        );

        // Test 8: First cursor position.
        mouse.clear();

        mouse.handleCursorMove(100.0, 200.0);
        mouse.beginFrame();

        check(
            mouse.hasCursorPosition(),
            "Cursor position not initialized."
        );

        check(
            approximatelyEqual(mouse.cursorPosition().x, 100.0),
            "Incorrect cursor X."
        );

        check(
            approximatelyEqual(mouse.cursorPosition().y, 200.0),
            "Incorrect cursor Y."
        );

        check(
            approximatelyEqual(mouse.cursorDelta().x, 0.0) &&
            approximatelyEqual(mouse.cursorDelta().y, 0.0),
            "Initial cursor event generated movement."
        );

        // Test 9: Accumulate cursor movement.
        mouse.handleCursorMove(110.0, 195.0);
        mouse.handleCursorMove(125.0, 205.0);

        mouse.beginFrame();

        check(
            approximatelyEqual(mouse.cursorDelta().x, 25.0),
            "Incorrect cursor X delta."
        );

        check(
            approximatelyEqual(mouse.cursorDelta().y, 5.0),
            "Incorrect cursor Y delta."
        );

        // Test 10: Movement resets next frame.
        mouse.beginFrame();

        check(
            approximatelyEqual(mouse.cursorDelta().x, 0.0) &&
            approximatelyEqual(mouse.cursorDelta().y, 0.0),
            "Mouse delta was not cleared."
        );

        // Position must be retained.
        check(
            approximatelyEqual(mouse.cursorPosition().x, 125.0),
            "Cursor position was lost."
        );

        // Test 11: Scroll accumulation.
        mouse.handleScroll(0.0, 1.0);
        mouse.handleScroll(0.5, -0.25);
        mouse.beginFrame();

        check(
            approximatelyEqual(mouse.scrollDelta().x, 0.5),
            "Incorrect horizontal scroll."
        );

        check(
            approximatelyEqual(mouse.scrollDelta().y, 0.75),
            "Incorrect vertical scroll."
        );

        mouse.beginFrame();

        check(
            approximatelyEqual(mouse.scrollDelta().y, 0.0),
            "Scroll delta was not cleared."
        );

        // Test 12: Reset cursor baseline.
        mouse.resetCursorBaseline();

        mouse.handleCursorMove(500.0, 500.0);
        mouse.beginFrame();

        check(
            approximatelyEqual(mouse.cursorDelta().x, 0.0) &&
            approximatelyEqual(mouse.cursorDelta().y, 0.0),
            "Reset baseline caused artificial movement."
        );

        // Test 13: Invalid mouse buttons.
        mouse.clear();
        constexpr auto invalidButton = MouseButton::Count;

        mouse.handlePress(invalidButton);
        mouse.handleRelease(invalidButton);
        mouse.beginFrame();

        check(
            !mouse.isDown(invalidButton),
            "Invalid button accepted."
        );

        check(
            !mouse.wasPressed(invalidButton),
            "Invalid button press reported."
        );

        check(
            !mouse.wasReleased(invalidButton),
            "Invalid button release was reported."
        );

        // Test another out-of-range value.
        const auto outOfRangeButton = static_cast<MouseButton>(
            static_cast<std::size_t>(MouseButton::Count) + 1
            );

        mouse.handlePress(outOfRangeButton);
        mouse.beginFrame();

        check(
            !mouse.isDown(outOfRangeButton),
            "Out-of-range button was accepted."
        );

        // Test 14: Last valid button must be supported.
        mouse.clear();

        mouse.handlePress(MouseButton::Middle);
        mouse.beginFrame();

        check(
            mouse.wasPressed(MouseButton::Middle),
            "Middle mouse button press was lost."
        );

        mouse.handleRelease(MouseButton::Middle);
        mouse.beginFrame();

        check(
            mouse.wasReleased(MouseButton::Middle),
            "Middle mouse button release was lost."
        );

        // Test 15: Clear pending events.
        mouse.clear();

        mouse.handlePress(leftButton);
        mouse.handleScroll(0.0, 2.0);
        mouse.clear();
        mouse.beginFrame();

        check(
            !mouse.isDown(leftButton) &&
            !mouse.wasPressed(leftButton),
            "Clear failed to reset mouse buttons.");

        check(
            approximatelyEqual(mouse.scrollDelta().y, 0.0),
            "Clear failed to discard scroll."
        );

        std::cout << "All MouseState tests passed.\n";
        return EXIT_SUCCESS;
    }
    catch (const std::exception& exception)
    {
        std::cerr << "MouseState test failed: " << exception.what() << '\n';
        return EXIT_FAILURE;
    }
}
