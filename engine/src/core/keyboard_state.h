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
        void handleKeyPress(std::size_t key) noexcept;
        void handleKeyRelease(std::size_t key) noexcept;

        // Query keyboard state
        [[nodiscard]]
        bool isDown(std::size_t key) const noexcept;

        [[nodiscard]]
        bool pressedThisFrame(std::size_t key) const noexcept;

        [[nodiscard]]
        bool releasedThisFrame(std::size_t key) const noexcept;

        // Clear held keys and transition flags
        void clear() noexcept;

    private:
        std::array<bool, KeyCount> down_{};
        std::array<bool, KeyCount> pressed_{};
        std::array<bool, KeyCount> released_{};
    };
}
