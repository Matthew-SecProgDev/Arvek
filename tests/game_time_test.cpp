#include "game_time.h"

#include <cmath>
#include <cstdlib>
#include <iostream>
#include <limits>
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

    template<typename Exception, typename F>
    bool throwsException(F&& operation)
    {
        try
        {
            operation();
        }
        catch (const Exception&)
        {
            return true;
        }

        return false;
    }
}

int main()
{
    using namespace arvek;

    try
    {
        GameTime time;

        // Test 1: Default state.
        check(
            time.frameCount() == 0,
            "Initial frame count must be zero."
        );

        check(
            approximatelyEqual(time.deltaTime(), 0.0),
            "Initial delta must be zero."
        );

        check(
            approximatelyEqual(time.totalTime(), 0.0),
            "Initial total time must be zero."
        );

        check(
            approximatelyEqual(time.timeScale(), 1.0),
            "Default time scale must be one."
        );

        check(
            approximatelyEqual(time.fixedDeltaTime(), 1.0 / 60.0),
            "Default fixed delta is incorrect."
        );

        // Test 2: Advance by 16 milliseconds.
        time.advance(0.016);

        check(
            approximatelyEqual(time.deltaTime(), 0.016),
            "First delta is incorrect."
        );

        check(
            approximatelyEqual(time.totalTime(), 0.016),
            "First total time is incorrect."
        );

        check(
            time.frameCount() == 1,
            "Frame count must be one."
        );

        // Test 3: Accumulate another 20 milliseconds.
        time.advance(0.020);

        check(
            approximatelyEqual(time.deltaTime(), 0.020),
            "Second delta is incorrect."
        );

        check(
            approximatelyEqual(time.totalTime(), 0.036),
            "Accumulated time is incorrect."
        );

        check(
            time.frameCount() == 2,
            "Frame count must be two."
        );

        // Test 4: Half-speed simulation.
        time.setTimeScale(0.5);
        time.advance(0.020);

        check(
            approximatelyEqual(time.deltaTime(), 0.010),
            "Scaled delta is incorrect."
        );

        check(
            approximatelyEqual(time.unscaledDeltaTime(), 0.020),
            "Unscaled delta is incorrect."
        );

        check(
            approximatelyEqual(time.totalTime(), 0.046),
            "Scaled total is incorrect."
        );

        check(
            approximatelyEqual(time.unscaledTotalTime(), 0.056),
            "Unscaled total is incorrect."
        );

        // Test 5: Pause game time.
        time.setTimeScale(0.0);
        time.advance(0.030);

        check(
            approximatelyEqual(time.deltaTime(), 0.0),
            "Paused delta must be zero."
        );

        check(
            approximatelyEqual(time.totalTime(), 0.046),
            "Paused game time must not advance."
        );

        check(
            approximatelyEqual(time.unscaledTotalTime(), 0.086),
            "Unscaled time must continue advancing."
        );

        // Test 6: Configure fixed timestep.
        time.setFixedDeltaTime(1.0 / 120.0);

        check(
            approximatelyEqual(time.fixedDeltaTime(), 1.0 / 120.0),
            "Fixed timestep is incorrect."
        );

        // Test 7: Invalid values must be rejected.
        check(
            throwsException<std::invalid_argument>(
            [&]
            {
                time.advance(-0.01);
            }),
            "Negative delta was accepted."
        );

        check(
            throwsException<std::invalid_argument>(
            [&]
            {
                time.setTimeScale(-1.0);
            }),
            "Negative time scale was accepted."
        );

        check(
            throwsException<std::invalid_argument>(
            [&]
            {
                time.setFixedDeltaTime(0.0);
            }),
            "Zero fixed timestep was accepted."
        );

        check(
            throwsException<std::invalid_argument>(
            [&] {
                time.advance(
                    std::numeric_limits<double>::quiet_NaN());
            }),
            "NaN delta was accepted."
        );

        // Test 8: Reset preserves configuration.
        time.reset();

        check(
            time.frameCount() == 0,
            "Reset did not clear frame count."
        );

        check(
            approximatelyEqual(time.totalTime(), 0.0),
            "Reset did not clear total time."
        );

        check(
            approximatelyEqual(time.timeScale(), 0.0),
            "Reset unexpectedly changed time scale."
        );

        check(
            approximatelyEqual(time.fixedDeltaTime(), 1.0 / 120.0),
            "Reset unexpectedly changed fixed timestep."
        );

        std::cout << "All GameTime tests passed.\n";
        return EXIT_SUCCESS;
    }
    catch (const std::exception& exception)
    {
        std::cerr << "GameTime test failed: " << exception.what() << '\n';
        return EXIT_FAILURE;
    }
}
