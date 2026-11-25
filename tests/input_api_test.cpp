#include <arvek/input/input.h>

#include <type_traits>
#include <utility>

namespace
{
    using arvek::Input;
    using arvek::MousePosition;

    static_assert(std::is_same_v<
        decltype(std::declval<const Input&>().cursorPosition()),
        MousePosition>
        );

    static_assert(std::is_same_v<
        decltype(std::declval<const Input&>().cursorDelta()),
        MousePosition>
        );

    static_assert(std::is_same_v<
        decltype(std::declval<const Input&>().scrollDelta()),
        MousePosition>
        );

    static_assert(std::is_same_v<
        decltype(std::declval<const Input&>().hasCursorPosition()),
        bool>
        );

    static_assert(!std::is_default_constructible_v<Input>);

    static_assert(!std::is_copy_constructible_v<Input>);
}

int main()
{
    return 0;
}
