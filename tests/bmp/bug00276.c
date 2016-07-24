/* See <https://github.com/libgd/libgd/issues/276> */


#include "gd.h"
#include "gdtest.h"


int main()
{
	gdImagePtr im_orig, im_saved;
	int white;
	char *filename;
	FILE *fp;

	/* create an image */
	im_orig = gdImageCreate(10, 10);
	gdImageColorAllocate(im_orig, 0, 0, 0);
	white = gdImageColorAllocate(im_orig, 255, 255, 255);
	gdImageLine(im_orig, 0,0, 9,9, white);

	filename = gdTestFilePath2("bmp", "bug00276_act.bmp");

	/* save image as compressed BMP */
	fp = fopen(filename, "w+");
	gdTestAssert(fp != NULL);
	gdImageBmp(im_orig, fp, 1);
	fclose(fp);

	/* read saved image */
	fp = fopen(filename, "rb");
	gdTestAssert(fp != NULL);
	im_saved = gdImageCreateFromBmp(fp);
	gdTestAssert(im_saved != NULL);
	fclose(fp);

	gdAssertImageEquals(im_orig, im_saved);

	/* clean up */
	gdImageDestroy(im_orig);
	gdImageDestroy(im_saved);
	free(filename);

	return gdNumFailures();
}
