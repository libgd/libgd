#include "gd.h"
#include "gdtest.h"

int main() {
	gdImagePtr src, dst;
	void *data;
	int size = 0;
	int red, green, blue;

	src = gdImageCreate(16, 16);
	if (!gdTestAssert(src != NULL))
		return gdNumFailures();

	red = gdImageColorAllocate(src, 255, 0, 0);
	green = gdImageColorAllocate(src, 0, 255, 0);
	blue = gdImageColorAllocate(src, 0, 0, 255);

	gdImageFilledRectangle(src, 0, 0, 15, 15, red);
	gdImageFilledRectangle(src, 2, 2, 13, 13, green);
	gdImageLine(src, 0, 15, 15, 0, blue);

	data = gdImageJxlPtrEx(src, &size, 1, 0.0f, 7);
	if (!gdTestAssert(data != NULL) || !gdTestAssert(size > 0))
		goto cleanup_src;

	dst = gdImageCreateFromJxlPtr(size, data);
	if (!gdTestAssert(dst != NULL))
		goto cleanup_data;
	gdTestAssert(gdImageTrueColor(dst));
	gdTestAssert(gdImageSX(dst) == gdImageSX(src));
	gdTestAssert(gdImageSY(dst) == gdImageSY(src));

	gdImageDestroy(dst);
cleanup_data:
	gdFree(data);
cleanup_src:
	gdImageDestroy(src);

	return gdNumFailures();
}
