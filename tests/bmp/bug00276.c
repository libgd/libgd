/* See <https://github.com/libgd/libgd/issues/276> */


#include "gd.h"
#include "gdtest.h"


int main()
{
	gdImagePtr im_orig, im_saved;
	int white;
	void *data;
	int size;

	/* create an image */
	im_orig = gdImageCreate(10, 10);
	gdImageColorAllocate(im_orig, 0, 0, 0);
	white = gdImageColorAllocate(im_orig, 255, 255, 255);
	gdImageLine(im_orig, 0,0, 9,9, white);

	/* save the image, re-read it and compare it with the original */
	data = gdImageBmpPtr(im_orig, &size, 1);
	im_saved = gdImageCreateFromBmpPtr(size, data);
	gdTestAssert(im_saved != NULL);
	gdAssertImageEquals(im_orig, im_saved);

	/* clean up */
	gdImageDestroy(im_orig);
	gdImageDestroy(im_saved);

	gdFree(data);

	return gdNumFailures();
}
