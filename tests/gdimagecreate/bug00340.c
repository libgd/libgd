/**
 * Regression test for <https://github.com/libgd/libgd/issues/340>
 *
 * We're testing that trying to create an oversized image fails early,
 * triggering an appropriate warning.
 */

#include <string.h>
#include "gd.h"
#include "gd_errors.h"
#include "gdtest.h"

#define MSG "product of memory allocation multiplication would exceed INT_MAX, failing operation gracefully\n"

void error_handler(int priority, const char *format, ...)
{
	gdTestAssert(priority == GD_WARNING);
	gdTestAssert(!strcmp(format, MSG));
}

int main()
{
	gdImagePtr im;

	im = gdImageCreate(64970, 65111);
	gdTestAssert(im == NULL);

	return gdNumFailures();
}
