#pragma once

#include <arvek/core/application.h>
#include <arvek/core/engine.h>

#include <cstdlib>
#include <exception>
#include <iostream>
#include <type_traits>

namespace arvek
{
    template<typename TApp>
    int runApplication()
    {
        static_assert(
            std::is_base_of_v<Application, TApp>,
            "TApp must derive from arvek::Application!"
            );

        static_assert(
            std::is_default_constructible_v<TApp>,
            "TApp must be default-constructible!"
            );

        try
        {
            Engine engine;

            engine.initialize();
            engine.run<TApp>();
            engine.shutdown();

            return EXIT_SUCCESS;
        }
        catch (const std::exception& exception)
        {
            std::cerr << "Arvek runtime error: " << exception.what() << '\n';
        }
        catch (...)
        {
            std::cerr << "Arvek runtime: unknown exception.\n";
        }

        return EXIT_FAILURE;
    }
}
