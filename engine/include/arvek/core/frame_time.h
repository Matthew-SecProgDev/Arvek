#pragma once

#include <cstdint>

namespace arvek
{
    struct FrameTime
    {
        // All time values are measured in seconds.

        double deltaTime{ 0.0 };
        double unscaledDeltaTime{ 0.0 };

        double totalTime{ 0.0 };
        double unscaledTotalTime{ 0.0 };

        std::uint64_t frameIndex{ 0 };
    };
}
