/**
 * File: bad_input.c
 *
 * Make sure that the AVIF encoding and decoding functions handle bad input gracefully.
 */

#include <stdio.h>
#include <string.h>
#include "gd.h"
#include "gdtest.h"

int main() {
	FILE *fp;
	int retval;
	gdImagePtr realIm, badIm;
	void *rv;
	int size;

	// Read in an AVIF image for testing.

	fp = gdTestFileOpen2("avif", "sunset.avif");
	realIm = gdImageCreateFromAvif(fp);
	fclose(fp);
	if (!gdTestAssertMsg(realIm != NULL, "gdImageCreateFromAvif() failed\n"))
		return 1;

	// Try to decode a non-AVIF file.
	fp = gdTestFileOpen2("avif", "sunset.png");
	badIm = gdImageCreateFromAvif(fp);
	fclose(fp);
	gdTestAssertMsg(badIm == NULL, "gdImageCreateFromAvif() failed to return NULL when passed a non-AVIF file\n");

	if (badIm)
		gdImageDestroy(badIm);

	// Try to encode a valid image with bad quality parameters. This should still work.

	rv = gdImageAvifPtrEx(realIm, &size, 400, 10);
	gdTestAssertMsg(rv != NULL, "gdImageAvifPtrEx() rejected an overly high quality param instead of clamping it to a valid value");
	gdFree(rv);

	rv = gdImageAvifPtrEx(realIm, &size, -4, 10);
	gdTestAssertMsg(rv != NULL, "gdImageAvifPtrEx() rejected a negative quality param instead of clamping it to a valid value");
	gdFree(rv);

	rv = gdImageAvifPtrEx(realIm, &size, 30, 30);
	gdTestAssertMsg(rv != NULL, "gdImageAvifPtrEx() rejected an overly high speed param instead of clamping it to a valid value");
	gdFree(rv);

	rv = gdImageAvifPtrEx(realIm, &size, 30, -4);
	gdTestAssertMsg(rv != NULL, "gdImageAvifPtrEx() rejected a negative speed param instead of clamping it to a valid value");
	gdFree(rv);

	gdImageDestroy(realIm);

	return gdNumFailures();
}
