#pragma once

#include <array>
#include <cstddef>

namespace arvek
{
    class KeyboardState final
    {
    public:
        static constexpr std::size_t KeyCount = 512;

        void beginFrame() noexcept;

        // Process incoming keyboard events
        void handlePress(std::size_t key) noexcept;
        void handleRelease(std::size_t key) noexcept;

        // Query keyboard state
        [[nodiscard]]
        bool isDown(std::size_t key) const noexcept;

        [[nodiscard]]
        bool wasPressed(std::size_t key) const noexcept;

        [[nodiscard]]
        bool wasReleased(std::size_t key) const noexcept;

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
