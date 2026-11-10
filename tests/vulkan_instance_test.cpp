#include <arvek/core/engine.h>
#include "instance.h"

#include <exception>
#include <iostream>
#include <cstdlib>

int main()
{
    try
    {
        arvek::Engine engine;
        engine.initialize(false);

        {
            arvek::Instance instance;

            if (instance.handle() == VK_NULL_HANDLE)
            {
                std::cerr << "Vulkan instance is null\n";
                return EXIT_FAILURE;
            }

            std::cout << "Vulkan instance created successfully\n";
        }

        // Vulkan instance is destroyed before Engine,
        // so the GLFW runtime remains initialized.

        engine.shutdown();
    }
    catch (const std::exception& e)
    {
        std::cerr << e.what() << '\n';
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}