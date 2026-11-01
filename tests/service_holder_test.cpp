#include <arvek/core/service_holder.h>

#include <memory>
#include <vector>
#include <cstdlib>

namespace
{
    struct AudioService
    {
        explicit AudioService(std::vector<int>& events)
            : events_(events)
        {
        }

        ~AudioService()
        {
            events_.push_back(1);
        }

        std::vector<int>& events_;
    };

    struct PhysicsService
    {
        explicit PhysicsService(std::vector<int>& events)
            : events_(events)
        {
        }

        ~PhysicsService()
        {
            events_.push_back(2);
        }

        std::vector<int>& events_;
    };
}

int main()
{
    using namespace arvek;

    std::vector<int> destructionOrder;

    {
        std::vector<std::unique_ptr<IServiceHolder>> services;

        services.push_back(
            std::make_unique<ServiceHolder<AudioService>>(
                std::make_unique<AudioService>(destructionOrder)
            )
        );

        services.push_back(
            std::make_unique<ServiceHolder<PhysicsService>>(
                std::make_unique<PhysicsService>(destructionOrder)
            )
        );

        if (services.size() != 2)
        {
            return EXIT_FAILURE;
        }

        if (services[0]->get() == nullptr ||
            services[1]->get() == nullptr)
        {
            return EXIT_FAILURE;
        }

        // Explicitly destroy in reverse registration order.
        while (!services.empty())
        {
            services.pop_back();
        }
    }

    if (destructionOrder != std::vector<int>{ 2, 1 })
    {
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}