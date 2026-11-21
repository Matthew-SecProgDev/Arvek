#pragma once

#include <arvek/input/key.h>

#include <array>
#include <cstddef>

namespace arvek
{
    class KeyboardState final
    {
    public:
        static constexpr std::size_t KeyCount = static_cast<std::size_t>(Key::Count);

        void beginFrame() noexcept;

        // Process incoming keyboard events
        void handlePress(Key key) noexcept;
        void handleRelease(Key key) noexcept;

        // Query keyboard state
        [[nodiscard]]
        bool isDown(Key key) const noexcept;

        [[nodiscard]]
        bool wasPressed(Key key) const noexcept;

        [[nodiscard]]
        bool wasReleased(Key key) const noexcept;

        // Clear held keys and transition flags
        void clear() noexcept;

    private:
        // Current physical key state.
        std::array<bool, KeyCount> down_{};

        // Transitions visible to the current frame.
        std::array<bool, KeyCount> pressed_{};
        std::array<bool, KeyCount> released_{};

        // Transitions awaiting publication.
        std::array<bool, KeyCount> pendingPressed_{};
        std::array<bool, KeyCount> pendingReleased_{};
    };
}
