#pragma once

#include <fmt/format.h>

#include <string>
#include <utility>

namespace arvek
{
    enum class LogLevel
    {
        Trace,
        Info,
        Warning,
        Error
    };

    class Logger final
    {
    public:
        Logger() = delete;

        static void setMinLevel(LogLevel level) noexcept;
        static bool isEnabled(LogLevel level) noexcept;

        template<typename... Args>
        static void log(
            LogLevel level,
            const char* file,
            int line,
            fmt::string_view formatStr,
            Args&&... args);

    private:
        static void write(
            LogLevel level,
            const char* file,
            int line,
            const std::string& message);
    };
}

template <typename... Args>
void arvek::Logger::log(
    LogLevel level,
    const char* file,
    int line,
    fmt::string_view formatStr,
    Args&&... args)
{
    if (!isEnabled(level))
    {
        return;
    }

    auto message = fmt::format(
        fmt::runtime(formatStr),
        std::forward<Args>(args)...
    );

    write(level, file, line, message);
}

#define ARV_LOG_TRACE(...) \
    ::arvek::Logger::log( \
        ::arvek::LogLevel::Trace, \
        __FILE__, __LINE__, __VA_ARGS__)

#define ARV_LOG_INFO(...) \
    ::arvek::Logger::log( \
        ::arvek::LogLevel::Info, \
        __FILE__, __LINE__, __VA_ARGS__)

#define ARV_LOG_WARN(...) \
    ::arvek::Logger::log( \
        ::arvek::LogLevel::Warning, \
        __FILE__, __LINE__, __VA_ARGS__)

#define ARV_LOG_ERROR(...) \
    ::arvek::Logger::log( \
        ::arvek::LogLevel::Error, \
        __FILE__, __LINE__, __VA_ARGS__)