#pragma once

#include <iostream>

namespace YForge
{
	class Engine final
	{
	public:
		Engine() = default;
		~Engine() = default;

		Engine(const Engine&) = delete;
		Engine& operator=(const Engine&) = delete;
		Engine(Engine&&) = delete;
		Engine& operator=(Engine&&) = delete;

		void initialize();

		template<typename TApp>
		void run();

		void shutdown();
	};
}

template <typename TApp>
void YForge::Engine::run()
{
	std::cout << "YForge Engine run!\n";
}