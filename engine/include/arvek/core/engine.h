#pragma once

#include <type_traits>
#include <memory>
#include <stdexcept>

namespace arvek
{
	class ServiceRegistry;
	class GlfwRuntime;
	class Window;
	class VulkanRenderer;
	class Application;

	class Engine final
	{
	public:
		Engine();
		~Engine();

		Engine(const Engine&) = delete;
		Engine& operator=(const Engine&) = delete;
		Engine(Engine&&) = delete;
		Engine& operator=(Engine&&) = delete;

		void initialize();

		template<typename TApp>
		void run();

        void requestExit() noexcept;

		[[nodiscard]]
		Window& window();

        void shutdown();

	private:
		enum class State
		{
			Constructed,
			Initialized,
            Running,
			Shutdown
		};

		std::unique_ptr<ServiceRegistry> serviceRegistry_;
		std::unique_ptr<GlfwRuntime> glfwRuntime_;
		std::unique_ptr<Window> window_;
        std::unique_ptr<VulkanRenderer> renderer_;

		State state_{ State::Constructed };
        bool exitRequested_{ false };

		void runApplication(Application& app);
	};
}

template <typename TApp>
void arvek::Engine::run()
{
    static_assert(
        std::is_base_of_v<Application, TApp>,
        "TApp must derive from arvek::Application!"
        );

    if (state_ != State::Initialized)
    {
        throw std::logic_error(
            "Engine must be initialized and idle before running!"
        );
    }

    TApp app;
    runApplication(app);
}
