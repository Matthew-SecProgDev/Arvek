#include <arvek/core/service_registrar.h>

#include "service_registry.h"

namespace arvek
{
	ServiceRegistrar::ServiceRegistrar(ServiceRegistry& registry) noexcept
		: registry_(&registry)
	{
	}

	void ServiceRegistrar::addGameService(
		std::type_index type,
		std::unique_ptr<IServiceHolder> holder)
	{
		registry_->addErased(
			type,
			std::move(holder),
			ServiceRegistry::ServiceCategory::Game
		);
	}
}