#include "gd.h"
#include "gdtest.h"
#include "jxl_test_helpers.h"

int main() {
	gdImagePtr src, dst;
	void *data;
	int size = 0;

	src = jxlTestCreatePattern(0);
	if (!gdTestAssert(src != NULL))
		return gdNumFailures();

	data = gdImageJxlPtrEx(src, &size, 1, 0.0f, 7);
	if (!gdTestAssert(data != NULL) || !gdTestAssert(size > 0))
		goto cleanup_src;

	dst = gdImageCreateFromJxlPtr(size, data);
	if (!gdTestAssert(dst != NULL))
		goto cleanup_data;
	gdAssertImageEquals(src, dst);

	gdImageDestroy(dst);
cleanup_data:
	gdFree(data);
cleanup_src:
	gdImageDestroy(src);

	return gdNumFailures();
}
