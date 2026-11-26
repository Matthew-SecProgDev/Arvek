#include "scene_manager.h"

#include <arvek/core/service_provider.h>

#include <stdexcept>
#include <utility>

namespace arvek
{
    SceneManager::SceneManager(ServiceProvider& provider) noexcept
        : context_(provider)
    {
    }

    SceneManager::~SceneManager() noexcept
    {
        // Normal engine shutdown should call stop() explicitly
        // so exceptions can be handled.
        try
        {
            stop();
        }
        catch (...)
        {
            // Destructors must not propagate exceptions.
        }
    }

    void SceneManager::start(std::unique_ptr<Scene> scene)
    {
        if (!scene)
        {
            throw std::invalid_argument("Cannot start a null scene!");
        }

        if (activeScene_)
        {
            throw std::logic_error("A scene is already active!");
        }

        scene->onStart(context_);

        activeScene_ = std::move(scene);
    }

    void SceneManager::update(const FrameTime& time)
    {
        if (!activeScene_)
        {
            return;
        }

        activeScene_->onUpdate(time);
    }

    void SceneManager::stop()
    {
        if (!activeScene_)
        {
            return;
        }

        // Remove ownership from the manager before invoking user-defined shutdown code.
        auto scene = std::move(activeScene_);

        scene->onShutdown();
    }

    bool SceneManager::hasActiveScene() const noexcept
    {
        return activeScene_ != nullptr;
    }
}
