#pragma once

namespace arvek
{
    class SceneManager;
    class ServiceProvider;

    class SceneContext final
    {
    public:
        SceneContext(const SceneContext&) = delete;
        SceneContext& operator=(const SceneContext&) = delete;

        [[nodiscard]]
        ServiceProvider& serviceProvider() const noexcept
        {
            return provider_;
        }

    private:
        friend class SceneManager;

        explicit SceneContext(ServiceProvider& provider) noexcept
            : provider_(provider)
        {
        }

        ServiceProvider& provider_;
    };
}
