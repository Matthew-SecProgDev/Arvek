#include "YForge/engine.h"

class Application
{
};

int main()
{
    YForge::Engine engine;

    engine.initialize();
    engine.run<Application>();
    engine.shutdown();

    return 0;
}