#include <arvek/input/key.h>
#include <arvek/input/mouse_button.h>

#include <cstdint>
#include <cstdlib>
#include <iostream>
#include <type_traits>

int main()
{
    using namespace arvek;

    static_assert(std::is_enum_v<Key>);
    static_assert(std::is_enum_v<MouseButton>);

    static_assert(std::is_same_v<std::underlying_type_t<Key>, std::uint16_t>);

    static_assert(std::is_same_v<std::underlying_type_t<MouseButton>, std::uint8_t>);

    static_assert(Key::S != Key::W);
    static_assert(Key::Escape != Key::Space);

    static_assert(MouseButton::Left != MouseButton::Right);

    static_assert(static_cast<std::uint8_t>(MouseButton::Left) == 0);

    static_assert(static_cast<std::uint8_t>(MouseButton::Right) == 1);

    static_assert(static_cast<std::uint8_t>(MouseButton::Middle) == 2);

    std::cout << "Input code tests passed.\n";

    return EXIT_SUCCESS;
}
