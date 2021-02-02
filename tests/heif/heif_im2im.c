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

	src = gdImageCreateTrueColor(100, 100);
	if (src == NULL) {
		gdTestErrorMsg("could not create src\n");
		return 1;
	}
	/* libheif seems to have some rounding issues */
	r = gdImageColorAllocate(src, 0xFE, 0, 0);
	g = gdImageColorAllocate(src, 0, 0xFE, 0);
	b = gdImageColorAllocate(src, 0, 0, 0xFE);
	gdImageFilledRectangle(src, 0, 0, 99, 99, r);
	gdImageRectangle(src, 20, 20, 79, 79, g);
	gdImageEllipse(src, 70, 25, 30, 20, b);

#define OUTPUT_HEIF(name) do {							\
		FILE *fp = gdTestTempFp();						\
		gdImageHeifEx(name, fp, 200,					\
		GD_HEIF_COMPRESSION_HEVC, GD_HEIF_CHROMA_444);	\
		fclose(fp);										\
	} while (0)

	OUTPUT_HEIF(src);
	p = gdImageHeifPtrEx(src, &size, 200, GD_HEIF_COMPRESSION_HEVC, GD_HEIF_CHROMA_444);
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

	dst = gdImageCreateFromHeifPtr(size, p);
	if (dst == NULL) {
		status = 1;
		gdTestErrorMsg("could not create dst\n");
		goto door1;
	}
	OUTPUT_HEIF(dst);
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
