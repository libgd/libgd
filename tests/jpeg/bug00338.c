/**
 * Regression test for <https://github.com/libgd/libgd/issues/338>
 *
 * We're testing that reading a JPEG image with gdImageCreateFromJpeg()
 * raises a GD_WARNING for the fatal libjpeg error, but not a GD_ERROR.
 * We also make sure, that the fatal libjpeg error is actually reported.
 *
 * See also ../png/bug00338.c
 */

#include <string.h>
#include "gd.h"
#include "gd_errors.h"
#include "gdtest.h"

#define MSG "gd-jpeg: JPEG library reports unrecoverable error: %s"

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
	gdImagePng(im, fp);
	gdImageDestroy(im);

	im = gdImageCreateFromJpeg(fp);
	gdTestAssert(im == NULL);

	gdTestAssert(error_handler_called);

	return gdNumFailures();
}
