#include <arvek/core/logger.h>

#include <atomic>
#include <cstdio>
#include <mutex>

namespace
{
    std::mutex logMutex;
    std::atomic<arvek::LogLevel> minimumLevel{
        arvek::LogLevel::Trace
    };

    const char* toString(arvek::LogLevel level) noexcept
    {
        switch (level)
        {
        case arvek::LogLevel::Trace:
            return "TRACE";
        case arvek::LogLevel::Info:
            return "INFO";
        case arvek::LogLevel::Warning:
            return "WARNING";
        case arvek::LogLevel::Error:
            return "ERROR";
        }

        return "UNKNOWN";
    }
}

namespace arvek
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