#pragma once

#include <arvek/input/key.h>
#include <arvek/input/mouse_button.h>

#include <optional>

namespace arvek
{
    [[nodiscard]]
    std::optional<Key> translateGlfwKey(int key) noexcept;

    [[nodiscard]]
    std::optional<MouseButton> translateGlfwMouseButton(int button) noexcept;
}
