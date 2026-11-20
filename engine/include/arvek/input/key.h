#pragma once

#include <cstdint>

namespace arvek
{
    enum class Key : std::uint16_t
    {
        Unknown = 0,

        W,
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
