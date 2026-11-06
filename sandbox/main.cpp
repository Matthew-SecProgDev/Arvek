#include <arvek/core/engine.h>
#include <arvek/core/logger.h>

#include <cstdlib>

namespace 
{
	class MyGame
	{
	};
}

int main()
{
    ARV_LOG_INFO("Arvek sandbox started.");

    arvek::Engine engine;

    engine.initialize();

    engine.run<MyGame>();

    engine.shutdown();

    ARV_LOG_INFO("Arvek sandbox finished.");

    return EXIT_SUCCESS;
}