/**
 * File: avif_null.c
 *
 * Simple test case, confirming that if you try to create an AVIF image from a
 * null file pointer, the creation will fail, and it will return NULL.
 */

#include "gd.h"
#include "gdtest.h"


int main()
{
	gdImagePtr im;

	im = gdImageCreateFromAvif(NULL);
	if (!gdTestAssert(im == NULL))
		gdImageDestroy(im);

	gdImageAvif(im, NULL); /* noop safely */

	return gdNumFailures();
}
