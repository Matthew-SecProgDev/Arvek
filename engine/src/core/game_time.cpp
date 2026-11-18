#include "game_time.h"

#include <cmath>
#include <limits>
#include <stdexcept>

namespace arvek
{
    void GameTime::reset() noexcept
    {
        deltaTime_ = 0.0;
        unscaledDeltaTime_ = 0.0;
        totalTime_ = 0.0;
        unscaledTotalTime_ = 0.0;
        frameCount_ = 0;
    }

    void GameTime::advance(double realDeltaSeconds)
    {
        if (!std::isfinite(realDeltaSeconds) || realDeltaSeconds < 0.0)
        {
            throw std::invalid_argument(
                "Frame delta must be finite and non-negative!"
            );
        }

        if (frameCount_ == std::numeric_limits<std::uint64_t>::max())
        {
            throw std::overflow_error(
                "GameTime frame counter overflow!"
            );
        }

        const double scaledDelta = realDeltaSeconds * timeScale_;

        const double nextTotal = totalTime_ + scaledDelta;

        const double nextUnscaledTotal = unscaledTotalTime_ + realDeltaSeconds;

        if (!std::isfinite(scaledDelta) ||
            !std::isfinite(nextTotal) ||
            !std::isfinite(nextUnscaledTotal))
        {
            throw std::overflow_error(
                "GameTime accumulated time overflow!"
            );
        }

        // Update state only after all validation succeeds.
        unscaledDeltaTime_ = realDeltaSeconds;
        deltaTime_ = scaledDelta;

        unscaledTotalTime_ = nextUnscaledTotal;
        totalTime_ = nextTotal;

        ++frameCount_;
    }

    double GameTime::deltaTime() const noexcept
    {
        return deltaTime_;
    }

    double GameTime::unscaledDeltaTime() const noexcept
    {
        return unscaledDeltaTime_;
    }

    double GameTime::totalTime() const noexcept
    {
        return totalTime_;
    }

    double GameTime::unscaledTotalTime() const noexcept
    {
        return unscaledTotalTime_;
    }

    double GameTime::fixedDeltaTime() const noexcept
    {
        return fixedDeltaTime_;
    }

    double GameTime::timeScale() const noexcept
    {
        return timeScale_;
    }

    std::uint64_t GameTime::frameCount() const noexcept
    {
        return frameCount_;
    }

    void GameTime::setTimeScale(double scale)
    {
        if (!std::isfinite(scale) || scale < 0.0)
        {
            throw std::invalid_argument(
                "Time scale must be finite and non-negative!"
            );
        }

        timeScale_ = scale;
    }

    void GameTime::setFixedDeltaTime(double seconds)
    {
        if (!std::isfinite(seconds) || seconds <= 0.0)
        {
            throw std::invalid_argument(
                "Fixed delta time must be finite and positive!"
            );
        }

        fixedDeltaTime_ = seconds;
    }
}
