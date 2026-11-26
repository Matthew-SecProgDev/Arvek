#include "scene_manager.h"
#include "service_registry.h"
#include <arvek/core/service_provider.h>

#include <cstdlib>
#include <memory>
#include <stdexcept>
#include <vector>

namespace
{
    void check(bool condition, const char* message)
    {
        if (!condition)
        {
            throw std::runtime_error(message);
        }
    }

    struct SceneEvents
    {
        std::vector<int> calls;
    };

    class TestScene final : public arvek::Scene
    {
    public:
        explicit TestScene(SceneEvents& events) : events_(events)
        {
        }

        ~TestScene() override
        {
            events_.calls.push_back(4);
        }

        void onStart(arvek::SceneContext&) override
        {
            events_.calls.push_back(1);
        }

        void onUpdate(const arvek::FrameTime&) override
        {
            events_.calls.push_back(2);
        }

        void onShutdown() override
        {
            events_.calls.push_back(3);
        }

    private:
        SceneEvents& events_;
    };

    class FailingScene final : public arvek::Scene
    {
    public:
        explicit FailingScene(bool& destroyed) : destroyed_(destroyed)
        {
        }

        ~FailingScene() override
        {
            destroyed_ = true;
        }

        void onStart(arvek::SceneContext&) override
        {
            throw std::runtime_error("Scene startup failed!");
        }

        void onUpdate(const arvek::FrameTime&) override
        {
        }

    private:
        bool& destroyed_;
    };
}

int main()
{
    using namespace arvek;

    ServiceRegistry registry;
    auto provider = registry.provider();

    SceneManager manager(provider);
    FrameTime time{};

    // No active scene initially.
    check(
        !manager.hasActiveScene(),
        "SceneManager should initially be empty."
    );

    // Updating or stopping an empty manager is safe.
    manager.update(time);
    manager.stop();

    // Null scenes must be rejected.
    bool nullRejected = false;

    try
    {
        manager.start(nullptr);
    }
    catch (const std::invalid_argument&)
    {
        nullRejected = true;
    }

    check(
        nullRejected,
        "Null scene was accepted."
    );

    // Test normal lifecycle.
    SceneEvents events;

    manager.start(std::make_unique<TestScene>(events));

    check(
        manager.hasActiveScene(),
        "Scene was not activated."
    );

    manager.update(time);
    manager.update(time);

    // Starting another scene must fail.
    bool duplicateRejected = false;

    try
    {
        manager.start(std::make_unique<TestScene>(events));
    }
    catch (const std::logic_error&)
    {
        duplicateRejected = true;
    }

    check(
        duplicateRejected,
        "Second scene was accepted."
    );

    // The rejected scene was destroyed, so clear
    // that event before checking the active lifecycle.
    events.calls.clear();

    manager.stop();

    check(
        !manager.hasActiveScene(),
        "Scene was not removed."
    );

    check(
        events.calls == std::vector<int>({ 3, 4 }),
        "Scene shutdown/destruction order is incorrect."
    );

    // A failed startup must leave the manager empty.
    bool destroyed = false;
    bool startupRejected = false;

    try
    {
        manager.start(std::make_unique<FailingScene>(destroyed));
    }
    catch (const std::runtime_error&)
    {
        startupRejected = true;
    }

    check(
        startupRejected,
        "Scene startup exception was not propagated."
    );

    check(destroyed,
        "Failed scene was not destroyed.");

    check(
        !manager.hasActiveScene(),
        "Failed scene remained active."
    );

    // Verify the manager can start a new scene afterward.
    SceneEvents secondEvents;

    manager.start(std::make_unique<TestScene>(secondEvents));

    manager.update(time);
    manager.stop();

    check(
        secondEvents.calls == std::vector<int>({ 1, 2, 3, 4 }),
        "Scene lifecycle order is incorrect."
    );

    return EXIT_SUCCESS;
}
