#include <stdio.h>
#include <string.h>

#include "gd.h"
#include "gdtest.h"

int main()
{
	unsigned char not_qoi[] = "not a qoi image";
	unsigned char truncated_qoi[] = {'q', 'o', 'i', 'f', 0, 0, 0, 1};
	void *data;
	int size = 123;
	FILE *fp;

	gdTestAssert(gdImageCreateFromQoiPtr(0, NULL) == NULL);
	gdTestAssert(gdImageCreateFromQoiPtr((int)sizeof(not_qoi), not_qoi) == NULL);
	gdTestAssert(gdImageCreateFromQoiPtr((int)sizeof(truncated_qoi), truncated_qoi) == NULL);
	gdTestAssert(gdImageCreateFromQoiCtx(NULL) == NULL);
	gdTestAssert(gdImageCreateFromQoi(NULL) == NULL);

	data = gdImageQoiPtr(NULL, &size);
	gdTestAssert(data == NULL);
	gdTestAssert(size == 0);

	data = gdImageQoiPtr(NULL, NULL);
	gdTestAssert(data == NULL);

	fp = gdTestTempFp();
	gdTestAssert(fp != NULL);
	if (fp != NULL) {
		gdImageQoi(NULL, fp);
		fclose(fp);
	}

	return gdNumFailures();
}
