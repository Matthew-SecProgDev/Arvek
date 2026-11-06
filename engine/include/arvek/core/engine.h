#pragma once

#include <iostream>
#include <memory>

namespace arvek
{
	class ServiceRegistry;
	class GlfwRuntime;
	class Window;

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

		[[nodiscard]]
		Window& window();

		void shutdown();

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
	};
}

template <typename TApp>
void arvek::Engine::run()
{
	if (state_ != State::Initialized)
	{
		throw std::logic_error(
			"Engine must be initialized before running!"
		);
	}

	std::cout << "Arvek Engine run!\n";
}