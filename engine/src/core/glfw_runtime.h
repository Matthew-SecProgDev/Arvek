#pragma once

namespace arvek
{
    class GlfwRuntime final
    {
    public:
        GlfwRuntime();
        ~GlfwRuntime();

        GlfwRuntime(const GlfwRuntime&) = delete;
        GlfwRuntime& operator=(const GlfwRuntime&) = delete;
        GlfwRuntime(GlfwRuntime&&) = delete;
        GlfwRuntime& operator=(GlfwRuntime&&) = delete;
    };
}