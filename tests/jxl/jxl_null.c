#include "gd.h"
#include "gdtest.h"

int main() {
	gdImagePtr im;

	im = gdImageCreateFromJxl(NULL);
	if (!gdTestAssert(im == NULL)) {
		gdImageDestroy(im);
	}

	gdImageJxl(NULL, NULL);

	return gdNumFailures();
}
