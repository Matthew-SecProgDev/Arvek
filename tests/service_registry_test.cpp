#include "service_registry.h"
#include <arvek/core/service_provider.h>
#include <arvek/core/service_registrar.h>

#include <memory>
#include <stdexcept>
#include <vector>
#include <cstdlib>
#include <type_traits>
#include <utility>

namespace
{
    struct EngineService
    {
        int value{ 42 };
    };

    struct GameService
    {
        int value{ 100 };
    };

    struct OtherService
    {
    };

    struct TrackedService
    {
        TrackedService(std::vector<int>& events, int id)
            : events_(events), id_(id)
        {
        }

        ~TrackedService()
        {
            events_.push_back(id_);
        }

        std::vector<int>& events_;
        int id_;
    };

    struct OtherTrackedService
    {
        explicit OtherTrackedService(std::vector<int>& events)
            : events_(events)
        {
        }

        ~OtherTrackedService()
        {
            events_.push_back(2);
        }

        std::vector<int>& events_;
    };

    template<typename T, typename = void>
    struct HasEngineRegistration : std::false_type {};

    template<typename T>
    struct HasEngineRegistration<T, std::void_t<
        decltype(std::declval<T&>().addEngineService(
            std::declval<std::unique_ptr<EngineService>>()
        ))
        >> : std::true_type {};
}

int main()
{
    using namespace arvek;

    // Game registration capability must not expose
    // engine-service registration.
    static_assert(
        !HasEngineRegistration<ServiceRegistrar>::value,
        "Game registrar must not register engine services"
        );

    ServiceRegistry registry;

    auto provider = registry.provider();
    auto registrar = registry.gameRegistrar();

    if (registry.size() != 0)
    {
        return EXIT_FAILURE;
    }

    // Engine service registration.
    registry.addEngineService(
        std::make_unique<EngineService>()
    );

    // Game service registration.
    registrar.addGameService(
        std::make_unique<GameService>()
    );

    // Provider retrieves both categories.
    if (provider.require<EngineService>().value != 42)
    {
        return EXIT_FAILURE;
    }

    if (provider.require<GameService>().value != 100)
    {
        return EXIT_FAILURE;
    }

    if (registry.size() != 2)
    {
        return EXIT_FAILURE;
    }

    // Missing service.
    if (provider.find<OtherService>() != nullptr)
    {
        return EXIT_FAILURE;
    }

    bool missingRejected = false;

    try
    {
        (void)provider.require<OtherService>();
    }
    catch (const std::logic_error&)
    {
        missingRejected = true;
    }

    if (!missingRejected)
    {
        return EXIT_FAILURE;
    }

    // Duplicate registration.
    bool duplicateRejected = false;

    try
    {
        registrar.addGameService(
            std::make_unique<EngineService>()
        );
    }
    catch (const std::logic_error&)
    {
        duplicateRejected = true;
    }

    if (!duplicateRejected || registry.size() != 2)
    {
        return EXIT_FAILURE;
    }

    registry.clear();

    if (registry.size() != 0)
    {
        return EXIT_FAILURE;
    }

    // Verify game-before-engine destruction.
    std::vector<int> destructionOrder;

    {
        ServiceRegistry lifetimeRegistry;

        lifetimeRegistry.addEngineService(
            std::make_unique<TrackedService>(
                destructionOrder, 1
            )
        );

        lifetimeRegistry.gameRegistrar().addGameService(
            std::make_unique<OtherTrackedService>(
                destructionOrder
            )
        );
    }

    if (destructionOrder != std::vector<int>{ 2, 1 })
    {
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}