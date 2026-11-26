#include <arvek/scene/scene.h>
#include <arvek/scene/scene_context.h>

#include <type_traits>
#include <cstdlib>

namespace
{
    class TestScene final : public arvek::Scene
    {
    public:
        double lastDeltaTime_{ 0.0 };

        void onStart(arvek::SceneContext&) override
        {
        }

        void onUpdate(const arvek::FrameTime& time) override
        {
            lastDeltaTime_ = time.deltaTime;
        }
    };

    static_assert(std::is_abstract_v<arvek::Scene>);
    static_assert(std::has_virtual_destructor_v<arvek::Scene>);

    static_assert(!std::is_copy_constructible_v<arvek::Scene>);
    static_assert(!std::is_copy_assignable_v<arvek::Scene>);

    static_assert(!std::is_default_constructible_v<arvek::SceneContext>);
    static_assert(!std::is_copy_constructible_v<arvek::SceneContext>);

    static_assert(!std::is_abstract_v<TestScene>);
}

int main()
{
    TestScene scene;

    arvek::FrameTime time{};
    time.deltaTime = 0.016;

    scene.onUpdate(time);

    if (scene.lastDeltaTime_ != 0.016)
    {
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
