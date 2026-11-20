#pragma once

#include <cstdint>

namespace arvek
{
    enum class MouseButton : std::uint8_t
    {
        Left = 0,
        Right,
        Middle,

        Count
    };
}
