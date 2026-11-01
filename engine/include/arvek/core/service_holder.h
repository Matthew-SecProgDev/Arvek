#pragma once

#include <memory>
#include <utility>

namespace arvek
{
    class IServiceHolder
    {
    public:
        virtual ~IServiceHolder() = default;

        virtual void* get() noexcept = 0;
    };

    template<typename T>
    class ServiceHolder final : public IServiceHolder
    {
    public:
        explicit ServiceHolder(std::unique_ptr<T> service)
            : service_(std::move(service))
        {
        }

        void* get() noexcept override
        {
            return service_.get();
        }

    private:
        std::unique_ptr<T> service_;
    };
}