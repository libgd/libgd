/**
 * File: bad_input.c
 *
 * Make sure that the AVIF encoding and decoding functions handle bad input gracefully.
 */

#include <stdio.h>
#include <string.h>
#include "gd.h"
#include "gdtest.h"

#define PATH "avif/"
#define MAX_FILEPATH_LENGTH 50

#define NON_AVIF_FILE_NAME "sunset.png"
#define AVIF_FILE_NAME "sunset.avif"

int main() {
	FILE *fp;
	int retval;
	char nonAvifFilePath[MAX_FILEPATH_LENGTH], avifFilePath[MAX_FILEPATH_LENGTH];
	gdImagePtr realIm, badIm;
	void *rv;
	int size;

// Create paths for our files.
	strcpy(avifFilePath, PATH);
	strcat(avifFilePath, AVIF_FILE_NAME);

	strcpy(nonAvifFilePath, PATH);
	strcat(nonAvifFilePath, NON_AVIF_FILE_NAME);

// Read in an AVIF image for testing.

	fp = gdTestFileOpen(avifFilePath);
	realIm = gdImageCreateFromAvif(fp);
	fclose(fp);
	if (!gdTestAssertMsg(realIm != NULL, "gdImageCreateFromAvif() failed\n"))
		return 1;

// Try to decode a non-AVIF file.

	fp = gdTestFileOpen(nonAvifFilePath);
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
