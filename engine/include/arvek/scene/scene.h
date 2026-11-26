#pragma once

#include <arvek/core/frame_time.h>

namespace arvek
{
    class SceneContext;

    class Scene
    {
    public:
        virtual ~Scene() = default;

        Scene(const Scene&) = delete;
        Scene& operator=(const Scene&) = delete;

        virtual void onStart(SceneContext& context) = 0;

        virtual void onUpdate(const FrameTime& time) = 0;

        virtual void onShutdown()
        {
        }

    protected:
        Scene() = default;
    };
}
