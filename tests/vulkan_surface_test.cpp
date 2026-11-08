#include <arvek/core/engine.h>
#include "instance.h"
#include "surface.h"

#include <exception>
#include <iostream>
#include <cstdlib>

int main()
{
    try
    {
        arvek::Engine engine;
        engine.initialize();

        {
            arvek::Instance instance;

            arvek::Surface surface(engine.window(), instance);

            if (surface.handle() == VK_NULL_HANDLE)
            {
                std::cerr << "Vulkan surface is null!\n";
                return EXIT_FAILURE;
            }

            std::cout << "Vulkan surface created successfully.\n";
        }

        // Surface is destroyed before Instance,
        // and both are destroyed before Engine's window.

        engine.shutdown();
    }
    catch (const std::exception& e)
    {
        std::cerr << e.what() << '\n';
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}