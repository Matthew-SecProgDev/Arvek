#include "arvek/core/service_registry.h"

arvek::ServiceRegistry::~ServiceRegistry()
{
	clear();
}

void arvek::ServiceRegistry::clear() noexcept
{
    services_.clear();

    while (!ownedServices_.empty())
    {
        ownedServices_.pop_back();
    }
}

std::size_t arvek::ServiceRegistry::size() const noexcept
{
    return ownedServices_.size();
}