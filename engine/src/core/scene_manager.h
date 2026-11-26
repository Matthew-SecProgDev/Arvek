#pragma once

#include <arvek/scene/scene.h>
#include <arvek/scene/scene_context.h>

#include <memory>

namespace arvek
{
    class ServiceProvider;

    class SceneManager final
    {
    public:
        explicit SceneManager(ServiceProvider& provider) noexcept;
        ~SceneManager() noexcept;

        SceneManager(const SceneManager&) = delete;
        SceneManager& operator=(const SceneManager&) = delete;

        void start(std::unique_ptr<Scene> scene);
        void update(const FrameTime& time);
        void stop();

        [[nodiscard]]
        bool hasActiveScene() const noexcept;

    private:
        SceneContext context_;
        std::unique_ptr<Scene> activeScene_;
    };
}
