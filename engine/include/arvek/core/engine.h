#pragma once

#include <iostream>
#include <memory>

namespace arvek
{
	class ServiceRegistry;

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

		void shutdown();

	private:
		enum class State
		{
			Constructed,
			Initialized,
			Shutdown
		};

		std::unique_ptr<ServiceRegistry> serviceRegistry_;

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