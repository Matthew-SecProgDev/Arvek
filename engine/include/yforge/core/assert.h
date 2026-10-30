#pragma once

#include <cstdlib>
#include <cstdio>

namespace yforge::detail
{
    [[noreturn]]
    inline void assertionFailed(
        const char* expression,
        const char* file,
        int line,
        const char* message) noexcept
    {
        std::fprintf(
            stderr,
            "[ASSERTION FAILED] %s\n"
            "Location: %s:%d\n"
            "Message: %s\n",
            expression,
            file,
            line,
            message
        );

        std::fflush(stderr);
        std::abort();
    }
}

#ifndef NDEBUG
    #define YF_ASSERT(condition, message)                     \
        do                                                    \
        {                                                     \
            if (!(condition))                                 \
            {                                                 \
                ::yforge::detail::assertionFailed(            \
                    #condition, __FILE__, __LINE__, message   \
                );                                            \
            }                                                 \
        } while (false)
#else
    #define YF_ASSERT(condition, message) ((void)0)
#endif