/*
 * Test that reading and writing image resolution values to/from TIFF files
 * works correctly.  Set the image resolution, write the file, read the file
 * back and test that the image resolution comes back correct.
 */

#include "gd.h"
#include "gdtest.h"

int main()
{
	gdImagePtr src, dst;
	int r, res_x, res_y;
	void *p;
	int size = 0;
	int status = 0;

	src = gdImageCreate(100, 100);
	if (src == NULL) {
		gdTestErrorMsg("could not create src\n");
		return 1;
	}
	r = gdImageColorAllocate(src, 0xFF, 0, 0);
	gdImageFilledRectangle(src, 0, 0, 99, 99, r);

	// gd default DPI is 96; libtiff default is 72.
	// Use something else so we know the value has been
	// written / read correctly.
	res_x = 100;
	res_y = 200;
	src->res_x = res_x;
	src->res_y = res_y;

#define OUTPUT_TIFF(name) do {							\
		FILE *fp = gdTestTempFp();						\
		gdImageTiff(name, fp);							\
		fclose(fp);										\
	} while (0)

	OUTPUT_TIFF(src);
	p = gdImageTiffPtr(src, &size);
	if (p == NULL) {
		status = 1;
		gdTestErrorMsg("p is null\n");
		goto door0;
	}
	if (size <= 0) {
		status = 1;
		gdTestErrorMsg("size is non-positive\n");
		goto door1;
	}

	dst = gdImageCreateFromTiffPtr(size, p);
	if (dst == NULL) {
		status = 1;
		gdTestErrorMsg("could not create dst\n");
		goto door1;
	}

	if (dst->res_x != res_x) {
		status = 1;
		gdTestErrorMsg("mismatch in res_x (got %d, expected %d)\n", dst->res_x, res_x);
	}

	if (dst->res_y != res_y) {
		status = 1;
		gdTestErrorMsg("mismatch in res_y (got %d, expected %d)\n", dst->res_y, res_y);
	}

	gdImageDestroy(dst);
door1:
	gdFree(p);
door0:
	gdImageDestroy(src);
	return status;
}
