#include "yforge/core/service_registry.h"

yforge::ServiceRegistry::~ServiceRegistry()
{
	clear();
}

void yforge::ServiceRegistry::clear() noexcept
{
    services_.clear();

    while (!ownedServices_.empty())
    {
        ownedServices_.pop_back();
    }
}

std::size_t yforge::ServiceRegistry::size() const noexcept
{
    return ownedServices_.size();
}