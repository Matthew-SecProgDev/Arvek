#include "glfw_runtime.h"
#include "glfw_window_backend.h"
#include "window_config.h"

#include <exception>
#include <iostream>
#include <cstdlib>

int main()
{
    try
    {
        arvek::GlfwRuntime runtime;

        {
            arvek::WindowConfig config;
            config.width = 1024;
            config.height = 768;
            config.title = "Arvek - GLFW Test";

            arvek::GlfwWindowBackend window(config);

            std::cout << "Window created successfully!\n";

            while (!window.shouldClose())
            {
                window.waitEvents();
            }

            std::cout << "Window destroyed successfully!\n";
        }

        // GlfwWindow is destroyed before GlfwRuntime.
    }
    catch (const std::exception& e)
    {
        std::cerr << e.what() << '\n';
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}