#include "gd.h"
#include "gdtest.h"

int main()
{
	gdImagePtr src, dst;
	int r, g, b;
	void *p;
	int size = 0;
	int status = 0;
	CuTestImageResult result = {0, 0};

	src = gdImageCreate(100, 100);
	if (src == NULL) {
		gdTestErrorMsg("could not create src\n");
		return 1;
	}
	r = gdImageColorAllocate(src, 0xFF, 0, 0);
	g = gdImageColorAllocate(src, 0, 0xFF, 0);
	b = gdImageColorAllocate(src, 0, 0, 0xFF);
	gdImageFilledRectangle(src, 0, 0, 99, 99, r);
	gdImageRectangle(src, 20, 20, 79, 79, g);
	gdImageEllipse(src, 70, 25, 30, 20, b);

#define OUTPUT_GD2(x) do {												\
		FILE *fp = gdTestTempFp();										\
		gdImageGd2(x, fp, (GD2_CHUNKSIZE_MIN+GD2_CHUNKSIZE_MAX)/2, GD2_FMT_COMPRESSED); \
		fclose(fp);														\
	} while (0)

	OUTPUT_GD2(src);
	p = gdImageGd2Ptr(src, (GD2_CHUNKSIZE_MIN+GD2_CHUNKSIZE_MAX)/2, GD2_FMT_COMPRESSED, &size);
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

	dst = gdImageCreateFromGd2Ptr(size, p);
	if (dst == NULL) {
		status = 1;
		gdTestErrorMsg("could not create dst\n");
		goto door1;
	}
	OUTPUT_GD2(dst);
	gdTestImageDiff(src, dst, NULL, &result);
	if (result.pixels_changed > 0) {
		status = 1;
		gdTestErrorMsg("pixels changed: %d\n", result.pixels_changed);
	}
	gdImageDestroy(dst);
door1:
	gdFree(p);
door0:
	gdImageDestroy(src);
	return status;
}
