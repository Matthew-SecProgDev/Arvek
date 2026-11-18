#pragma once

#include <chrono>

namespace arvek
{
    class Timer final
    {
    public:
        using Clock = std::chrono::steady_clock;
        using TimePoint = Clock::time_point;
        using Duration = Clock::duration;

        Timer() = default;

        void start() noexcept;

        void stop() noexcept;

        void reset() noexcept;

        void restart() noexcept;

        [[nodiscard]]
        double elapsedSeconds() const noexcept;

        [[nodiscard]]
        bool isRunning() const noexcept;

    private:
        TimePoint startTime_{};
        Duration accumulated_{ Duration::zero() };
        bool running_{ false };
    };
}
