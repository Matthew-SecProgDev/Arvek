#include <arvek/core/engine.h>

#include <stdexcept>
#include <cstdlib>
#include <iostream>

namespace
{
    class MyGame
    {
    };
}

int main()
{
    using namespace arvek;

    Engine engine;

    // Test 1: First initialization must succeed.
    bool singleInitializationSuccessful = true;

    try
    {
        engine.initialize();
    }
    catch (const std::logic_error&)
    {
        singleInitializationSuccessful = false;
    }

    if (!singleInitializationSuccessful)
    {
        std::cerr << "FAILED: Initialize must be succeeded!\n";
        return EXIT_FAILURE;
    }

    // Test 2: Double initialization must fail.
    bool doubleInitializationRejected = false;

    try
    {
        engine.initialize();
    }
    catch (const std::logic_error&)
    {
        doubleInitializationRejected = true;
    }

    if (!doubleInitializationRejected)
    {
        std::cerr << "FAILED: Double initialization must be failed!\n";
        return EXIT_FAILURE;
    }

    // Test 3: Run must succeed.
    bool PerformingRunSuccessful = true;

    try
    {
        engine.run<MyGame>();
    }
    catch (const std::logic_error&)
    {
        PerformingRunSuccessful = false;
    }

    if (!PerformingRunSuccessful)
    {
        std::cerr << "FAILED: Run must be succeeded!\n";
        return EXIT_FAILURE;
    }

    // Test 4: Shutdown
    engine.shutdown();

    // Test 5: Repeated shutdown is safe.
    engine.shutdown();

    // Test 6: Run before initialization must fail.
    bool PerformingRunRejected = false;

    try
    {
        engine.run<MyGame>();
    }
    catch (const std::logic_error&)
    {
        PerformingRunRejected = true;
    }

    if (!PerformingRunRejected)
    {
        std::cerr << "FAILED: Performing Run before initialization must be failed!\n";
        return EXIT_FAILURE;
    }

    // Test 7: Reinitialization after shutdown fails.
    bool reinitializationRejected = false;

    try
    {
        engine.initialize();
    }
    catch (const std::logic_error&)
    {
        reinitializationRejected = true;
    }

    if (!reinitializationRejected)
    {
        std::cerr << "FAILED: Reinitialization after shutdown must be failed!\n";
        return EXIT_FAILURE;
    }

    // Test 8: Shutdown before initialization is safe.
    Engine unusedEngine;
    unusedEngine.shutdown();
    unusedEngine.shutdown();

    return EXIT_SUCCESS;
}