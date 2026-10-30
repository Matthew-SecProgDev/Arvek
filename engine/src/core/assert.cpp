#include "yforge/core/assert.h"

#include <cstdlib>
#include <cstdio>

namespace yforge::detail
{
    void assertionFailed(
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