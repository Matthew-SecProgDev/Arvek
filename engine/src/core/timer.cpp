#include "timer.h"

namespace arvek
{
    void Timer::start() noexcept
    {
        if (running_)
        {
            return;
        }

        startTime_ = Clock::now();
        running_ = true;
    }

    void Timer::stop() noexcept
    {
        if (!running_)
        {
            return;
        }

        accumulated_ += Clock::now() - startTime_;
        running_ = false;
    }

    void Timer::reset() noexcept
    {
        accumulated_ = Duration::zero();
        running_ = false;
    }

    void Timer::restart() noexcept
    {
        accumulated_ = Duration::zero();
        startTime_ = Clock::now();
        running_ = true;
    }

    double Timer::elapsedSeconds() const noexcept
    {
        Duration elapsed = accumulated_;

        if (running_)
        {
            elapsed += Clock::now() - startTime_;
        }

        return std::chrono::duration<double>(elapsed).count();
    }

    bool Timer::isRunning() const noexcept
    {
        return running_;
    }
}
