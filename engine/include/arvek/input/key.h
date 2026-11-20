#pragma once

#include <cstdint>

namespace arvek
{
    enum class Key : std::uint16_t
    {
        W = 0,
        S,
        A,        
        D,
        Q,
        E,
        R,
        F,

        Space,
        Escape,
        Enter,

        Up,
        Down,
        Left,
        Right,
        
        LeftShift,
        RightShift,
        LeftControl,
        RightControl,

        Count
    };
}
