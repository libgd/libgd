#include "gd.h"
#include "gdtest.h"

int main()
{
	gdImagePtr src, dst;
	int b;
	void *p;
	int size = 0;
	int status = 0;
	CuTestImageResult result = {0, 0};

	src = gdImageCreate(100, 100);
	if (src == NULL) {
		gdTestErrorMsg("could not create src\n");
		return 1;
	}
	gdImageColorAllocate(src, 0xFF, 0xFF, 0xFF); /* allocate white for background color */
	b = gdImageColorAllocate(src, 0, 0, 0);
	gdImageRectangle(src, 20, 20, 79, 79, b);
	gdImageEllipse(src, 70, 25, 30, 20, b);

#define OUTPUT_WBMP(name) do {							\
		FILE *fp = gdTestTempFp();						\
		gdImageWBMP(name, 1, fp);						\
		fclose(fp);										\
	} while (0)

	OUTPUT_WBMP(src);
	p = gdImageWBMPPtr(src, &size, 1);
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

	dst = gdImageCreateFromWBMPPtr(size, p);
	if (dst == NULL) {
		status = 1;
		gdTestErrorMsg("could not create dst\n");
		goto door1;
	}
	OUTPUT_WBMP(dst);
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
