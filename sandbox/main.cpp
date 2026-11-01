#include <arvek/core/engine.h>
#include <arvek/core/logger.h>
#include <arvek/core/assert.h>
//#include <arvek/core/service_registrar.h>
//#include <arvek/core/service_provider.h>
//#include <arvek/core/service_registry.h>

#include <cstdlib>

namespace
{
    class Application
    {
    };

    struct WindowService
    {
        WindowService()
        {
            ARV_LOG_INFO("WindowService created");
        }

        ~WindowService()
        {
            ARV_LOG_INFO("WindowService destroyed");
        }

        void show()
        {
            ARV_LOG_INFO("WindowService::show() called");
        }
    };

    struct GameSettings
    {
        int width{ 1280 };
        int height{ 720 };

        GameSettings()
        {
            ARV_LOG_INFO("GameSettings created");
        }

        ~GameSettings()
        {
            ARV_LOG_INFO("GameSettings destroyed");
        }
    };
}

int main()
{
    ARV_LOG_INFO("Arvek sandbox started");

    arvek::Engine engine;

    engine.initialize();    

    int value = 42;
    ARV_ASSERT(value == 42, "Value must equal 42");
    //ARV_ASSERT(value == 100, "Value must equal 100");


    /*
    arvek::ServiceRegistry registry;

    // Engine developers register engine services.
    registry.addEngineService(
        std::make_unique<WindowService>()
    );

    // Game developers receive a restricted registrar.
    auto registrar = registry.gameRegistrar();

    registrar.addGameService(
        std::make_unique<GameSettings>()
    );

    // Both sides can retrieve either category.
    auto provider = registry.provider();

    auto& window = provider.require<WindowService>();
    auto& settings = provider.require<GameSettings>();

    window.show();

    ARV_LOG_INFO(
        "Game resolution: {}x{}",
        settings.width,
        settings.height
    );

    ARV_LOG_INFO(
        "Registered services: {}",
        registry.size()
    );*/


    ARV_LOG_WARN("Vulkan renderer is not integrated yet");
    engine.run<Application>();


    /*
    // Shut down game services before engine services.
    ARV_LOG_INFO("Clearing services...");
    registry.clear();
    */


    engine.shutdown();
    ARV_LOG_INFO("Arvek sandbox finished");

    return EXIT_SUCCESS;
}