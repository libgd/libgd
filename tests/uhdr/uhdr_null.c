#include "gd.h"
#include "gdtest.h"

#include <string.h>

int main()
{
	gdUhdrError err;
	int rc;

	memset(&err, 0, sizeof(err));

	gdTestAssertMsg(gdUhdrIsAvailable() == 1, "UltraHDR support should be enabled for this test\n");

	gdTestAssertMsg(gdUhdrImageCreateFromFile(NULL, GD_UHDR_FORMAT_JPEG, &err) == NULL,
		"gdUhdrImageCreateFromFile(NULL, ...) should fail\n");
	gdTestAssertMsg(err.code == GD_UHDR_E_INVALID,
		"expected GD_UHDR_E_INVALID for NULL file path, got %d\n", err.code);

	memset(&err, 0, sizeof(err));
	rc = gdUhdrImageResize(NULL, 100, 100, &err);
	gdTestAssertMsg(rc == GD_UHDR_E_INVALID,
		"gdUhdrImageResize(NULL, ...) should return GD_UHDR_E_INVALID, got %d\n", rc);

	memset(&err, 0, sizeof(err));
	rc = gdUhdrImageRotate(NULL, 90, &err);
	gdTestAssertMsg(rc == GD_UHDR_E_INVALID,
		"gdUhdrImageRotate(NULL, ...) should return GD_UHDR_E_INVALID, got %d\n", rc);

	memset(&err, 0, sizeof(err));
	gdTestAssertMsg(gdUhdrImageCreateFromPtr(0, NULL, GD_UHDR_FORMAT_JPEG, &err) == NULL,
		"gdUhdrImageCreateFromPtr with empty buffer should fail\n");
	gdTestAssertMsg(err.code == GD_UHDR_E_INVALID,
		"expected GD_UHDR_E_INVALID for empty buffer, got %d\n", err.code);

	return gdNumFailures();
}
