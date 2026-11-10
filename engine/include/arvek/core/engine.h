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
		void shutdown();

		template<typename TApp>
		void run();

		[[nodiscard]]
		Window& window();

	private:
		enum class State
		{
			Constructed,
			Initialized,
			Shutdown
		};

		std::unique_ptr<ServiceRegistry> serviceRegistry_;
		std::unique_ptr<GlfwRuntime> glfwRuntime_;
		std::unique_ptr<Window> window_;

		State state_{ State::Constructed };

		std::unique_ptr<VulkanRenderer> renderer_;

		void runApplication(Application& app);
	};
}

template <typename TApp>
void arvek::Engine::run()
{
	static_assert(
		std::is_base_of_v<Application, TApp>,
		"TApp must inherit from arvek::Application!"
		);

	static_assert(
		std::is_default_constructible_v<TApp>,
		"TApp must be default-constructible!"
		);

	if (state_ != State::Initialized)
	{
		throw std::logic_error(
			"Engine must be initialized before running!"
		);
	}

	TApp app;
	runApplication(app);
}