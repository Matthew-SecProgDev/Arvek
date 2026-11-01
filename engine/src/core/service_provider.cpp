#include "arvek/core/service_provider.h"
#include "service_registry.h"

namespace arvek
{
	ServiceProvider::ServiceProvider(ServiceRegistry& registry) noexcept
		: registry_(&registry)
	{
	}

	void* ServiceProvider::find(std::type_index type) const noexcept
	{
		return registry_->find(type);
	}
}