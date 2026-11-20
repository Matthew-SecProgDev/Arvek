#include "keyboard_state.h"

namespace arvek
{
    void KeyboardState::beginFrame() noexcept
    {
        pressed_ = pendingPressed_;
        released_ = pendingReleased_;

        pendingPressed_.fill(false);
        pendingReleased_.fill(false);
    }

    void KeyboardState::handlePress(std::size_t key) noexcept
    {
        if (key >= KeyCount)
        {
            return;
        }

        if (!down_[key])
        {
            pendingPressed_[key] = true;
        }

        down_[key] = true;
    }

    void KeyboardState::handleRelease(std::size_t key) noexcept
    {
        if (key >= KeyCount)
        {
            return;
        }

        if (down_[key])
        {
            pendingReleased_[key] = true;
        }

        down_[key] = false;
    }

    bool KeyboardState::isDown(std::size_t key) const noexcept
    {
        return key < KeyCount && down_[key];
    }

    bool KeyboardState::wasPressed(std::size_t key) const noexcept
    {
        return key < KeyCount && pressed_[key];
    }

    bool KeyboardState::wasReleased(std::size_t key) const noexcept
    {
        return key < KeyCount && released_[key];
    }

    void KeyboardState::clear() noexcept
    {
        down_.fill(false);

        pressed_.fill(false);
        released_.fill(false);

        pendingPressed_.fill(false);
        pendingReleased_.fill(false);
    }
}
