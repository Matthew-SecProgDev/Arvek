#include "glfw_runtime.h"

#include <exception>
#include <iostream>

int main()
{
    try
    {
        arvek::GlfwRuntime runtime;
    }
    catch (const std::exception& e)
    {
        std::cerr
            << "GLFW initialization failed: "
            << e.what()
            << '\n';

        return 1;
    }

    return 0;
}