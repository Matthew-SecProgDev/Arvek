#pragma once

#include <fmt/format.h>

#include <string>
#include <utility>

namespace yforge
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
void yforge::Logger::log(
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

#define YF_LOG_TRACE(...) \
    ::yforge::Logger::log( \
        ::yforge::LogLevel::Trace, \
        __FILE__, __LINE__, __VA_ARGS__)

#define YF_LOG_INFO(...) \
    ::yforge::Logger::log( \
        ::yforge::LogLevel::Info, \
        __FILE__, __LINE__, __VA_ARGS__)

#define YF_LOG_WARN(...) \
    ::yforge::Logger::log( \
        ::yforge::LogLevel::Warning, \
        __FILE__, __LINE__, __VA_ARGS__)

#define YF_LOG_ERROR(...) \
    ::yforge::Logger::log( \
        ::yforge::LogLevel::Error, \
        __FILE__, __LINE__, __VA_ARGS__)