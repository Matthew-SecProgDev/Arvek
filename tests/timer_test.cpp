#include "timer.h"

#include <chrono>
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
    using namespace std::chrono;

    try
    {
        Timer timer;

        // 1. Initial state
        check(
            !timer.isRunning(),
            "Timer should initially be stopped."
        );

        check(
            timer.elapsedSeconds() == 0.0,
            "Initial elapsed time must be zero."
        );

        // 2. Start timer
        timer.start();

        check(
            timer.isRunning(),
            "Timer failed to start."
        );

        // Repeated start must be safe.
        timer.start();

        // 3. Stop timer
        timer.stop();

        check(
            !timer.isRunning(),
            "Timer failed to stop."
        );

        const double stoppedTime = timer.elapsedSeconds();

        check(
            stoppedTime >= 0.0,
            "Elapsed time cannot be negative."
        );

        // Repeated stop must preserve elapsed time.
        timer.stop();

        check(
            timer.elapsedSeconds() == stoppedTime,
            "Repeated stop changed elapsed time."
        );

        // 4. Resume timer
        timer.start();

        check(
            timer.isRunning(),
            "Timer failed to resume."
        );

        check(
            timer.elapsedSeconds() >= stoppedTime,
            "Elapsed time decreased after resume."
        );

        timer.stop();

        // 5. Reset timer
        timer.reset();

        check(
            !timer.isRunning(),
            "Reset should stop the timer."
        );

        check(
            timer.elapsedSeconds() == 0.0,
            "Reset should clear elapsed time."
        );

        // 6. Restart timer
        timer.restart();

        check(
            timer.isRunning(),
            "Restart should start the timer."
        );

        check(
            timer.elapsedSeconds() >= 0.0,
            "Restart produced negative elapsed time."
        );

        timer.stop();

        std::cout << "All Timer tests passed.\n";

        return EXIT_SUCCESS;
    }
    catch (const std::exception& exception)
    {
        std::cerr << "Timer test failed: " << exception.what() << '\n';
        return EXIT_FAILURE;
    }
}
