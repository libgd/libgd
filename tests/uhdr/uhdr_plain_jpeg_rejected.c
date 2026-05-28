#include "gd.h"
#include "gdtest.h"

#include <string.h>

int main()
{
	gdUhdrImagePtr im;
	gdUhdrError err;
	char *path;

	path = gdTestFilePath2("jpeg", "conv_test.jpeg");
	if (!gdTestAssertMsg(path != NULL, "failed to resolve plain JPEG fixture path\n")) {
		return gdNumFailures();
	}

	memset(&err, 0, sizeof(err));
	im = gdUhdrImageCreateFromFile(path, GD_UHDR_FORMAT_JPEG, &err);
	gdTestAssertMsg(im == NULL, "plain JPEG should not load as UltraHDR\n");
	gdTestAssertMsg(err.code != GD_UHDR_SUCCESS,
		"plain JPEG rejection should set a failure code\n");
	gdFree(path);

	return gdNumFailures();
}
