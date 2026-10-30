#pragma once

namespace yforge::detail
{
    [[noreturn]]
    void assertionFailed(
        const char* expression,
        const char* file,
        int line,
        const char* message) noexcept;
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