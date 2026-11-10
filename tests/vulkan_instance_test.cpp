#include "instance.h"
#include "window_test_fixture.h"

#include <exception>
#include <iostream>
#include <cstdlib>

int main()
{
    try
    {
        arvek::WindowTestFixture fixture;

        {
            arvek::Instance instance;

            if (instance.handle() == VK_NULL_HANDLE)
            {
                std::cerr << "Vulkan instance is null\n";
                return EXIT_FAILURE;
            }

            std::cout << "Vulkan instance created successfully\n";
        }
    }
    catch (const std::exception& e)
    {
        std::cerr << e.what() << '\n';
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}