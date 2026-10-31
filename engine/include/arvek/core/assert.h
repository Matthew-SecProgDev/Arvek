#pragma once

namespace arvek::detail
{
    [[noreturn]]
    void assertionFailed(
        const char* expression,
        const char* file,
        int line,
        const char* message) noexcept;
}

#ifndef NDEBUG
    #define ARV_ASSERT(condition, message)                     \
        do                                                    \
        {                                                     \
            if (!(condition))                                 \
            {                                                 \
                ::arvek::detail::assertionFailed(            \
                    #condition, __FILE__, __LINE__, message   \
                );                                            \
            }                                                 \
        } while (false)
#else
    #define ARV_ASSERT(condition, message) ((void)0)
#endif