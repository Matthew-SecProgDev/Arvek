#include "mouse_state.h"

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

    void MouseState::handlePress(std::size_t button) noexcept
    {
        if (button >= ButtonCount)
        {
            return;
        }

        if (!down_[button])
        {
            pendingPressed_[button] = true;
        }

        down_[button] = true;
    }

    void MouseState::handleRelease(std::size_t button) noexcept
    {
        if (button >= ButtonCount)
        {
            return;
        }

        if (down_[button])
        {
            pendingReleased_[button] = true;
        }

        down_[button] = false;
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

    bool MouseState::isDown(std::size_t button) const noexcept
    {
        return button < ButtonCount && down_[button];
    }

    bool MouseState::wasPressed(std::size_t button) const noexcept
    {
        return button < ButtonCount && pressed_[button];
    }

    bool MouseState::wasReleased(std::size_t button) const noexcept
    {
        return button < ButtonCount && released_[button];
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
