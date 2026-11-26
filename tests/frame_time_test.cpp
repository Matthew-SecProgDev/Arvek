#include <arvek/core/frame_time.h>

#include <cstdlib>
#include <cstdint>
#include <type_traits>

int main()
{
    using namespace arvek;

    static_assert(std::is_copy_constructible_v<FrameTime>);
    static_assert(std::is_copy_assignable_v<FrameTime>);

    FrameTime time{};

    // Verify default initialization.
    if (time.deltaTime != 0.0 ||
        time.unscaledDeltaTime != 0.0 ||
        time.totalTime != 0.0 ||
        time.unscaledTotalTime != 0.0 ||
        time.frameIndex != 0)
    {
        return EXIT_FAILURE;
    }

    // Verify that timing information can be stored.
    time.deltaTime = 0.008;
    time.unscaledDeltaTime = 0.016;
    time.totalTime = 1.5;
    time.unscaledTotalTime = 3.0;
    time.frameIndex = 120;

    const FrameTime snapshot = time;

    if (snapshot.deltaTime != 0.008 ||
        snapshot.unscaledDeltaTime != 0.016 ||
        snapshot.totalTime != 1.5 ||
        snapshot.unscaledTotalTime != 3.0 ||
        snapshot.frameIndex != 120)
    {
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
