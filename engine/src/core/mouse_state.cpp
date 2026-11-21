#include "mouse_state.h"

namespace
{
    constexpr std::size_t toIndex(arvek::MouseButton button) noexcept
    {
        return static_cast<std::size_t>(button);
    }

    constexpr bool isValid(arvek::MouseButton button) noexcept
    {
        return toIndex(button) < arvek::MouseState::ButtonCount;
    }
}

namespace arvek
{
    void MouseState::beginFrame() noexcept
    {
        pressed_ = pendingPressed_;
        released_ = pendingReleased_;

        pendingPressed_.fill(false);
        pendingReleased_.fill(false);

        delta_ = pendingDelta_;
        scrollDelta_ = pendingScrollDelta_;

        pendingDelta_ = {};
        pendingScrollDelta_ = {};
    }

    void MouseState::handlePress(MouseButton button) noexcept
    {
        if (!isValid(button))
        {
            return;
        }

        const auto index = toIndex(button);

        if (!down_[index])
        {
            pendingPressed_[index] = true;
        }

        down_[index] = true;
    }

    void MouseState::handleRelease(MouseButton button) noexcept
    {
        if (!isValid(button))
        {
            return;
        }

        const auto index = toIndex(button);

        if (down_[index])
        {
            pendingReleased_[index] = true;
        }

        down_[index] = false;
    }

    void MouseState::handleCursorMove(double x, double y) noexcept
    {
        if (hasPosition_)
        {
            pendingDelta_.x += x - position_.x;
            pendingDelta_.y += y - position_.y;
        }

        position_ = { x, y };
        hasPosition_ = true;
    }

    void MouseState::handleScroll(double xOffset, double yOffset) noexcept
    {
        pendingScrollDelta_.x += xOffset;
        pendingScrollDelta_.y += yOffset;
    }

    bool MouseState::isDown(MouseButton button) const noexcept
    {
        return isValid(button) && down_[toIndex(button)];
    }

    bool MouseState::wasPressed(MouseButton button) const noexcept
    {
        return isValid(button) && pressed_[toIndex(button)];
    }

    bool MouseState::wasReleased(MouseButton button) const noexcept
    {
        return isValid(button) && released_[toIndex(button)];
    }

    MousePosition MouseState::cursorPosition() const noexcept
    {
        return position_;
    }

    MousePosition MouseState::cursorDelta() const noexcept
    {
        return delta_;
    }

    MousePosition MouseState::scrollDelta() const noexcept
    {
        return scrollDelta_;
    }

    bool MouseState::hasCursorPosition() const noexcept
    {
        return hasPosition_;
    }

    void MouseState::resetCursorBaseline() noexcept
    {
        hasPosition_ = false;
        pendingDelta_ = {};
        delta_ = {};
    }

    void MouseState::clear() noexcept
    {
        down_.fill(false);
        pressed_.fill(false);
        released_.fill(false);

        pendingPressed_.fill(false);
        pendingReleased_.fill(false);

        position_ = {};
        delta_ = {};
        pendingDelta_ = {};

        scrollDelta_ = {};
        pendingScrollDelta_ = {};

        hasPosition_ = false;
    }
}
