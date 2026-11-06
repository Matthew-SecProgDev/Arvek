#pragma once

#include <string>

namespace arvek
{
    struct WindowConfig
    {
        int width{ 800 };
        int height{ 600 };

        std::string title{ "Arvek" };

        bool resizable{ true };
        bool centered{ true };
    };
}