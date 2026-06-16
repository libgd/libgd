#include "gd.h"
#include "gdtest.h"
#include "jxl_test_helpers.h"

int main() {
	gdImagePtr src, frame;
	gdJxlAnimReaderPtr reader;
	void *data;
	int size = 0;
	int delay = -1;

	src = jxlTestCreatePattern(0);
	if (!gdTestAssert(src != NULL))
		return gdNumFailures();

	data = gdImageJxlPtrEx(src, &size, 1, 0.0f, 7);
	if (!gdTestAssert(data != NULL) || !gdTestAssert(size > 0))
		goto cleanup_src;

	reader = gdImageJxlAnimReaderCreatePtr(size, data);
	if (!gdTestAssert(reader != NULL))
		goto cleanup_data;

	frame = gdJxlReadNextImage(reader, &delay);
	gdTestAssert(frame != NULL);
	gdTestAssert(delay == 0);
	gdAssertImageEquals(src, frame);
	gdImageDestroy(frame);

	frame = gdJxlReadNextImage(reader, &delay);
	gdTestAssert(frame == NULL);

	gdImageJxlAnimReaderDestroy(reader);
cleanup_data:
	gdFree(data);
cleanup_src:
	gdImageDestroy(src);

	return gdNumFailures();
}
