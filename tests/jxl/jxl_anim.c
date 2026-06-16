#include "gd.h"
#include "gdtest.h"
#include "jxl_test_helpers.h"

int main() {
	gdImagePtr red, blue, frame;
	gdJxlAnimReaderPtr reader;
	void *data;
	int size = 0;
	int delay = -1;

	red = jxlTestCreateSolid(255, 0, 0, gdAlphaOpaque);
	blue = jxlTestCreateSolid(0, 0, 255, gdAlphaOpaque);
	if (!gdTestAssert(red != NULL) || !gdTestAssert(blue != NULL))
		goto cleanup_images;

	data = jxlTestEncodeAnimation(red, 120, blue, 80, 0, &size);
	if (!gdTestAssert(data != NULL) || !gdTestAssert(size > 0))
		goto cleanup_images;

	reader = gdImageJxlAnimReaderCreatePtr(size, data);
	if (!gdTestAssert(reader != NULL))
		goto cleanup_data;

	frame = gdJxlReadNextImage(reader, &delay);
	gdTestAssert(frame != NULL);
	gdTestAssert(delay == 120);
	gdTestAssert(gdImageSX(frame) == 4);
	gdTestAssert(gdImageSY(frame) == 4);
	gdImageDestroy(frame);

	frame = gdJxlReadNextImage(reader, &delay);
	gdTestAssert(frame != NULL);
	gdTestAssert(delay == 80);
	gdImageDestroy(frame);

	frame = gdJxlReadNextImage(reader, &delay);
	gdTestAssert(frame == NULL);

	gdImageJxlAnimReaderDestroy(reader);
cleanup_data:
	gdFree(data);
cleanup_images:
	gdImageDestroy(red);
	gdImageDestroy(blue);

	return gdNumFailures();
}
