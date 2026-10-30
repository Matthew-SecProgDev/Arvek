#include <yforge/core/logger.h>

#include <cstdlib>
#include <iostream>

int main()
{
    using namespace yforge;

    Logger::setMinLevel(LogLevel::Warning);

    if (Logger::isEnabled(LogLevel::Trace))
    {
        std::cerr << "FAILED: Trace should be disabled.\n";
        return EXIT_FAILURE;
    }

    if (Logger::isEnabled(LogLevel::Info))
    {
        std::cerr << "FAILED: Info should be disabled.\n";
        return EXIT_FAILURE;
    }

    if (!Logger::isEnabled(LogLevel::Warning))
    {
        std::cerr << "FAILED: Warning should be enabled.\n";
        return EXIT_FAILURE;
    }

    if (!Logger::isEnabled(LogLevel::Error))
    {
        std::cerr << "FAILED: Error should be enabled.\n";
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}