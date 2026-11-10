#include "window_test_fixture.h"
#include "instance.h"
#include "surface.h"

#include <exception>
#include <iostream>
#include <cstdlib>

int main()
{
    try
    {
        arvek::WindowTestFixture fixture;

        arvek::Instance instance;

        arvek::Surface surface(fixture.window(), instance);

        if (surface.handle() == VK_NULL_HANDLE)
        {
            std::cerr << "Vulkan surface is null!\n";
            return EXIT_FAILURE;
        }

        std::cout << "Vulkan surface created successfully.\n";
        return EXIT_SUCCESS;
    }
    catch (const std::exception& e)
    {
        std::cerr << e.what() << '\n';
        return EXIT_FAILURE;
    }
}