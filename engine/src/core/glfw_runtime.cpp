#include "glfw_runtime.h"

#include <GLFW/glfw3.h>

#include <stdexcept>

namespace arvek
{
    GlfwRuntime::GlfwRuntime()
    {
        if (glfwInit() != GLFW_TRUE)
        {
            throw std::runtime_error(
                "Failed to initialize GLFW!"
            );
        }
    }

    GlfwRuntime::~GlfwRuntime()
    {
        glfwTerminate();
    }
}