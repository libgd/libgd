/**
 * Regression test for <https://github.com/libgd/libgd/issues/338>
 *
 * We're testing that writing a PNG image with an unsupported quality
 * raises a GD_WARNING for the fatal libpng error, but not a GD_ERROR.
 * We also make sure, that the fatal libpng error is actually reported.
 *
 * See also ../jpeg/bug00338.c
 */

#include <string.h>
#include "gd.h"
#include "gd_errors.h"
#include "gdtest.h"

#define MSG "gd-png: fatal libpng error: %s\n"

static int error_handler_called = 0;

static void error_handler(int priority, const char *format, va_list args)
{
	if (!strcmp(format, MSG)) {
		gdTestAssertMsg(priority == GD_WARNING, "expected priority %d, but got %d", GD_WARNING, priority);
		error_handler_called = 1;
	}
}

int main()
{
	gdImagePtr im;
	FILE *fp;

	gdSetErrorMethod(error_handler);

	im = gdImageCreateTrueColor(10, 10);
	fp = gdTestTempFp();
	gdImagePngEx(im, fp, 100);
	gdImageDestroy(im);
	fclose(fp);

	gdTestAssert(error_handler_called);

	return gdNumFailures();
}
