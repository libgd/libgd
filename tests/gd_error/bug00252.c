/* See <https://github.com/libgd/libgd/issues/252>. */

#include "gd.h"
#include "gd_errors.h"
#include "gdtest.h"

void my_error_handler(int priority, const char *format, va_list args)
{
    int n;

    /* The following works according to
       <https://www.gnu.org/software/libc/manual/html_node/Receiving-Arguments.html>.
       It might not be portable, though. */
	n = va_arg(args, int);
    gdTestAssert(n == 42);
}

int main()
{
    gdSetErrorMethod(my_error_handler);
    gd_error_ex(GD_WARNING, "The answer is %i\n", 42);
    gd_error("The answer is %i\n", 42);
    return gdNumFailures();
}
