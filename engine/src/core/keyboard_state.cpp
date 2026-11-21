#include "keyboard_state.h"

namespace
{
    constexpr std::size_t toIndex(arvek::Key key) noexcept
    {
        return static_cast<std::size_t>(key);
    }

    constexpr bool isValid(arvek::Key key) noexcept
    {
        return toIndex(key) < arvek::KeyboardState::KeyCount;
    }
}

namespace arvek
{
    void KeyboardState::beginFrame() noexcept
    {
        pressed_ = pendingPressed_;
        released_ = pendingReleased_;

        pendingPressed_.fill(false);
        pendingReleased_.fill(false);
    }

    void KeyboardState::handlePress(Key key) noexcept
    {
        if (!isValid(key))
        {
            return;
        }

        const auto index = toIndex(key);

        if (!down_[index])
        {
            pendingPressed_[index] = true;
        }

        down_[index] = true;
    }

    void KeyboardState::handleRelease(Key key) noexcept
    {
        if (!isValid(key))
        {
            return;
        }

        const auto index = toIndex(key);

        if (down_[index])
        {
            pendingReleased_[index] = true;
        }

        down_[index] = false;
    }

    bool KeyboardState::isDown(Key key) const noexcept
    {
        return isValid(key) && down_[toIndex(key)];
    }

    bool KeyboardState::wasPressed(Key key) const noexcept
    {
        return isValid(key) && pressed_[toIndex(key)];
    }

    bool KeyboardState::wasReleased(Key key) const noexcept
    {
        return isValid(key) && released_[toIndex(key)];
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
