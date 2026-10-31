#include <arvek/core/assert.h>

#include <string_view>
#include <cstdlib>

int main(int argc, char* argv[])
{
#ifdef _MSC_VER
    // Prevent the MSVC CRT from displaying an abort dialog
    // during automated assertion tests.
    _set_abort_behavior(
        0,
        _WRITE_ABORT_MSG | _CALL_REPORTFAULT
    );
#endif

    const bool shouldFail = argc > 1 && std::string_view(argv[1]) == "fail";

    if (shouldFail)
    {
        ARV_ASSERT(false, "Intentional assertion failure");
    }
    else
    {
        ARV_ASSERT(true, "This should never fail");
    }

    return EXIT_SUCCESS;
}