#pragma once

#include <cstdint>

namespace arvek
{
    class GameTime final
    {
    public:
        GameTime() = default;

        // Resets elapsed timing state but preserves configuration.
        void reset() noexcept;

        // Advances timing by a real elapsed duration in seconds.
        void advance(double realDeltaSeconds);

        [[nodiscard]]
        double deltaTime() const noexcept;

        [[nodiscard]]
        double unscaledDeltaTime() const noexcept;

        [[nodiscard]]
        double totalTime() const noexcept;

        [[nodiscard]]
        double unscaledTotalTime() const noexcept;

        [[nodiscard]]
        double fixedDeltaTime() const noexcept;

        [[nodiscard]]
        double timeScale() const noexcept;

        [[nodiscard]]
        std::uint64_t frameCount() const noexcept;

        // Requires a finite, non-negative value.
        void setTimeScale(double scale);

        // Requires a finite, strictly positive value.
        void setFixedDeltaTime(double seconds);

    private:
        double deltaTime_{ 0.0 };
        double unscaledDeltaTime_{ 0.0 };

        double totalTime_{ 0.0 };
        double unscaledTotalTime_{ 0.0 };

        double fixedDeltaTime_{ 1.0 / 60.0 };
        double timeScale_{ 1.0 };

        std::uint64_t frameCount_{ 0 };
    };
}
