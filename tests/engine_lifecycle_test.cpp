#include <arvek/core/engine.h>
#include <arvek/core/window.h>
#include <arvek/core/application.h>
#include <arvek/core/application_startup_context.h>
#include <arvek/core/service_registrar.h>
#include <arvek/core/service_provider.h>

#include <stdexcept>
#include <cstdlib>
#include <iostream>
#include <memory>

namespace
{
    void check(bool condition, const char* message)
    {
        if (!condition)
        {
            throw std::runtime_error(message);
        }
    }

    struct TestGameService
    {
        static int destructionCount;

        ~TestGameService()
        {
            ++destructionCount;
        }
    };

    int TestGameService::destructionCount{ 0 };

    struct TestEnvironment
    {
        static arvek::Engine* engine;
        static int startCount;
        static int shutdownCount;
        static bool serviceAvailableDuringShutdown;

        static void reset(arvek::Engine& value)
        {
            engine = &value;
            startCount = 0;
            shutdownCount = 0;
            serviceAvailableDuringShutdown = false;
        }
    };

    arvek::Engine* TestEnvironment::engine{ nullptr };
    int TestEnvironment::startCount{ 0 };
    int TestEnvironment::shutdownCount{ 0 };
    bool TestEnvironment::serviceAvailableDuringShutdown{ false };

    class ExitOnStartGame final : public arvek::Application
    {
    public:
        void onStart(arvek::ApplicationStartupContext& context) override
        {
            ++TestEnvironment::startCount;

            // Test-only access to Engine.
            // Production Application does not receive Engine.
            TestEnvironment::engine->requestExit();
        }

        void onShutdown() override
        {
            ++TestEnvironment::shutdownCount;
        }
    };

    class RegisterServiceGame final : public arvek::Application
    {
    public:
        void onStart(arvek::ApplicationStartupContext& context) override
        {
            context.serviceRegistrar().addGameService(
                std::make_unique<TestGameService>()
            );

            ++TestEnvironment::startCount;
            TestEnvironment::engine->requestExit();
        }

        void onShutdown() override
        {
            ++TestEnvironment::shutdownCount;
        }
    };

    class ThrowOnStartGame final : public arvek::Application
    {
    public:
        void onStart(arvek::ApplicationStartupContext& context) override
        {
            ++TestEnvironment::startCount;

            throw std::runtime_error("Expected startup failure!");
        }

        void onShutdown() override
        {
            ++TestEnvironment::shutdownCount;
        }
    };

    class ThrowOnShutdownGame final : public arvek::Application
    {
    public:
        void onStart(arvek::ApplicationStartupContext& context) override
        {
            ++TestEnvironment::startCount;
            TestEnvironment::engine->requestExit();
        }

        void onShutdown() override
        {
            ++TestEnvironment::shutdownCount;

            throw std::runtime_error("Expected shutdown failure!");
        }
    };

    class RecursiveRunGame final : public arvek::Application
    {
    public:
        static bool recursiveRunRejected;

        void onStart(arvek::ApplicationStartupContext& context) override
        {
            ++TestEnvironment::startCount;

            try
            {
                TestEnvironment::engine->run<ExitOnStartGame>();
            }
            catch (const std::logic_error&)
            {
                recursiveRunRejected = true;
            }

            TestEnvironment::engine->requestExit();
        }

        void onShutdown() override
        {
            ++TestEnvironment::shutdownCount;
        }
    };

    bool RecursiveRunGame::recursiveRunRejected{ false };

    class ShutdownDuringRunGame final : public arvek::Application
    {
    public:
        static bool shutdownRejected;

        void onStart(arvek::ApplicationStartupContext& context) override
        {
            ++TestEnvironment::startCount;

            try
            {
                TestEnvironment::engine->shutdown();
            }
            catch (const std::logic_error&)
            {
                shutdownRejected = true;
            }

            TestEnvironment::engine->requestExit();
        }

        void onShutdown() override
        {
            ++TestEnvironment::shutdownCount;
        }
    };

    bool ShutdownDuringRunGame::shutdownRejected{ false };
}

int main()
{
    using namespace arvek;

    try
    {
        // 1. Running before initialization must fail.
        Engine uninitializedEngine;

        bool runBeforeInitializationRejected{ false };

        try
        {
            uninitializedEngine.run<ExitOnStartGame>();
        }
        catch (const std::logic_error&)
        {
            runBeforeInitializationRejected = true;
        }

        check(
            runBeforeInitializationRejected,
            "Run before initialization was not rejected!"
        );

        // Shutdown before initialization is safe.
        uninitializedEngine.shutdown();
        uninitializedEngine.shutdown();

        // 2. Initialize the main engine.
        Engine engine;
        engine.initialize();

        auto& window = engine.window();
        const auto extent = window.framebufferExtent();

        check(
            extent.width > 0 && extent.height > 0,
            "Initialized window has an invalid extent!"
        );

        // 3. Double initialization must fail.
        bool doubleInitializationRejected{ false };

        try
        {
            engine.initialize();
        }
        catch (const std::logic_error&)
        {
            doubleInitializationRejected = true;
        }

        check(
            doubleInitializationRejected,
            "Double initialization was not rejected!"
        );

        // 4. Normal application startup and shutdown.
        TestEnvironment::reset(engine);
        engine.run<ExitOnStartGame>();

        check(
            TestEnvironment::startCount == 1,
            "onStart() was not called exactly once!"
        );

        check(
            TestEnvironment::shutdownCount == 1,
            "onShutdown() was not called exactly once!"
        );

        // 5. A second sequential run must succeed.
        TestEnvironment::reset(engine);
        engine.run<ExitOnStartGame>();

        check(
            TestEnvironment::startCount == 1 &&
            TestEnvironment::shutdownCount == 1,
            "Second application run failed!"
        );

        // 6. Game services must be released after run().
        TestEnvironment::reset(engine);
        TestGameService::destructionCount = 0;

        engine.run<RegisterServiceGame>();

        check(
            TestGameService::destructionCount == 1,
            "Game service was not destroyed after run!"
        );

        // Verify a second registration of the same type works.
        engine.run<RegisterServiceGame>();

        check(
            TestGameService::destructionCount == 2,
            "Game service survived between application runs!"
        );

        // 7. A startup exception must propagate.
        TestEnvironment::reset(engine);

        bool startupExceptionCaught{ false };

        try
        {
            engine.run<ThrowOnStartGame>();
        }
        catch (const std::runtime_error&)
        {
            startupExceptionCaught = true;
        }

        check(
            startupExceptionCaught,
            "Startup exception was not propagated!"
        );

        check(
            TestEnvironment::shutdownCount == 0,
            "onShutdown() called after failed startup!"
        );

        // Verify engine state was restored.
        TestEnvironment::reset(engine);
        engine.run<ExitOnStartGame>();

        // 8. A shutdown exception must propagate.
        TestEnvironment::reset(engine);

        bool shutdownExceptionCaught{ false };

        try
        {
            engine.run<ThrowOnShutdownGame>();
        }
        catch (const std::runtime_error&)
        {
            shutdownExceptionCaught = true;
        }

        check(
            shutdownExceptionCaught,
            "Shutdown exception was not propagated!"
        );

        // Verify engine state was restored again.
        TestEnvironment::reset(engine);
        engine.run<ExitOnStartGame>();

        // 9. Recursive execution must be rejected.
        TestEnvironment::reset(engine);
        RecursiveRunGame::recursiveRunRejected = false;

        engine.run<RecursiveRunGame>();

        check(
            RecursiveRunGame::recursiveRunRejected,
            "Recursive Engine::run() was not rejected!"
        );

        // 10. Direct shutdown during execution is rejected.
        TestEnvironment::reset(engine);
        ShutdownDuringRunGame::shutdownRejected = false;

        engine.run<ShutdownDuringRunGame>();

        check(
            ShutdownDuringRunGame::shutdownRejected,
            "Shutdown during active execution was not rejected!"
        );

        // 11. Normal engine shutdown.
        engine.shutdown();

        // Repeated shutdown must be safe.
        engine.shutdown();

        // 12. Window access after shutdown fails.
        bool windowAccessRejected{ false };

        try
        {
            (void)engine.window();
        }
        catch (const std::logic_error&)
        {
            windowAccessRejected = true;
        }

        check(
            windowAccessRejected,
            "Window access after shutdown was not rejected!"
        );

        // 13. Running after shutdown fails.
        bool runAfterShutdownRejected{ false };

        try
        {
            engine.run<ExitOnStartGame>();
        }
        catch (const std::logic_error&)
        {
            runAfterShutdownRejected = true;
        }

        check(
            runAfterShutdownRejected,
            "Run after shutdown was not rejected!"
        );

        // 14. Reinitialization after shutdown fails.
        bool reinitializationRejected{ false };

        try
        {
            engine.initialize();
        }
        catch (const std::logic_error&)
        {
            reinitializationRejected = true;
        }

        check(
            reinitializationRejected,
            "Reinitialization after shutdown was not rejected!"
        );

        std::cout << "All Arvek engine lifecycle tests passed!\n";

        return EXIT_SUCCESS;
    }
    catch (const std::exception& exception)
    {
        std::cerr
            << "Engine lifecycle test failed: "
            << exception.what()
            << '\n';

        return EXIT_FAILURE;
    }
}
