#include "gd.h"
#include "gdtest.h"
#include "jxl_test_helpers.h"

int main() {
	gdImagePtr red, blue, frame;
	gdJxlAnimReaderPtr reader;
	gdJxlFrameInfo info;
	void *data;
	int size = 0;

	red = jxlTestCreateSolid(255, 0, 0, gdAlphaOpaque);
	blue = jxlTestCreateSolid(0, 0, 255, gdAlphaOpaque);
	if (!gdTestAssert(red != NULL) || !gdTestAssert(blue != NULL))
		goto cleanup_images;

	data = jxlTestEncodeAnimation(red, 120, blue, 80, 1, &size);
	if (!gdTestAssert(data != NULL) || !gdTestAssert(size > 0))
		goto cleanup_images;

	reader = gdImageJxlAnimReaderCreateRawPtr(size, data);
	if (!gdTestAssert(reader != NULL))
		goto cleanup_data;

	frame = gdJxlReadNextFrame(reader, &info);
	gdTestAssert(frame != NULL);
	gdTestAssert(info.delay_ms == 120);
	gdTestAssert(info.width == 4);
	gdTestAssert(info.height == 4);
	gdTestAssert(info.blend_mode == GD_JXL_BLEND_REPLACE);
	gdImageDestroy(frame);

	frame = gdJxlReadNextFrame(reader, &info);
	gdTestAssert(frame != NULL);
	gdTestAssert(info.delay_ms == 80);
	gdTestAssert(info.width == 4);
	gdTestAssert(info.height == 4);
	gdImageDestroy(frame);

	gdImageJxlAnimReaderDestroy(reader);
cleanup_data:
	gdFree(data);
cleanup_images:
	gdImageDestroy(red);
	gdImageDestroy(blue);

	return gdNumFailures();
}
