#pragma once

namespace arvek
{
    class ApplicationStartupContext;

    class Application
    {
    public:
        virtual ~Application() = default;

        Application(const Application&) = delete;
        Application& operator=(const Application&) = delete;

        virtual void onStart(ApplicationStartupContext& context) = 0;

        virtual void onShutdown()
        {
        }

    protected:
        Application() = default;
    };
}