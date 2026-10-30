#include "yforge/core/logger.h"

#include <atomic>
#include <cstdio>
#include <mutex>

namespace
{
    std::mutex logMutex;
    std::atomic<yforge::LogLevel> minimumLevel{
        yforge::LogLevel::Trace
    };

    const char* toString(yforge::LogLevel level) noexcept
    {
        switch (level)
        {
        case yforge::LogLevel::Trace:
            return "TRACE";
        case yforge::LogLevel::Info:
            return "INFO";
        case yforge::LogLevel::Warning:
            return "WARNING";
        case yforge::LogLevel::Error:
            return "ERROR";
        }

        return "UNKNOWN";
    }
}

namespace yforge
{
    void Logger::setMinLevel(LogLevel level) noexcept
    {
        minimumLevel.store(level);
    }

    bool Logger::isEnabled(LogLevel level) noexcept
    {
        return static_cast<int>(level) >= static_cast<int>(minimumLevel.load());
    }

    void Logger::write(
        LogLevel level,
        const char* file,
        int line,
        const std::string& message)
    {
        std::lock_guard<std::mutex> lock(logMutex);

        fmt::print(
            stderr,
            "[{}] {}:{} | {}\n",
            toString(level),
            file,
            line,
            message
        );

        std::fflush(stderr);
    }
}