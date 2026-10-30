#include <yforge/core/service_registry.h>

#include <cassert>
#include <memory>
#include <stdexcept>
#include <vector>

namespace
{
    struct FirstService
    {
        int value{ 42 };
    };

    struct SecondService
    {
        int value{ 100 };
    };

    struct TrackedService
    {
        TrackedService(
            std::vector<int>& events,
            int identifier)
            : events_(events),
            identifier_(identifier)
        {
        }

        ~TrackedService()
        {
            events_.push_back(identifier_);
        }

        std::vector<int>& events_;
        int identifier_;
    };

    struct AnotherTrackedService
    {
        explicit AnotherTrackedService(
            std::vector<int>& events)
            : events_(events)
        {
        }

        ~AnotherTrackedService()
        {
            events_.push_back(2);
        }

        std::vector<int>& events_;
    };
}

int main()
{
    using namespace yforge;

    ServiceRegistry registry;

    assert(registry.size() == 0);
    assert(registry.find<FirstService>() == nullptr);

    registry.add(std::make_unique<FirstService>());

    assert(registry.size() == 1);
    assert(registry.require<FirstService>().value == 42);

    bool duplicateRejected = false;

    try
    {
        registry.add(std::make_unique<FirstService>());
    }
    catch (const std::logic_error&)
    {
        duplicateRejected = true;
    }

    if (!duplicateRejected)
    {
        return 1;
    }

    registry.add(std::make_unique<SecondService>());

    if (registry.require<SecondService>().value != 100)
    {
        return 2;
    }

    registry.clear();

    if (registry.size() != 0 ||
        registry.find<FirstService>() != nullptr)
    {
        return 3;
    }

    std::vector<int> destructionOrder;

    {
        ServiceRegistry lifetimeRegistry;

        lifetimeRegistry.add(
            std::make_unique<TrackedService>(
                destructionOrder, 1)
        );

        lifetimeRegistry.add(
            std::make_unique<AnotherTrackedService>(
                destructionOrder)
        );
    }

    if (destructionOrder != std::vector<int>{ 2, 1 })
    {
        return 4;
    }

    return 0;
}