#include <stdio.h>
#include <stdlib.h>
#include "gd.h"

#include "gdtest.h"

int main()
{
	gdImagePtr im;
	FILE *fp;
	int i;
	const char header[] = {
		0x00, 0x02,
		0x00, 0x01,
		0x00, 0x01,
		0x00, 0x40,
		0x00, 0x02,
		0x54, 0xA0,
		0x5B, 0x00
	};

	/* we're creating the test image dynamically, due to its size */
	fp = gdTestTempFp();
	fwrite(header, sizeof(header[0]), sizeof(header), fp);
	for (i = 0; i < 0x4000000; i++) {
		fputc(0x41, fp);
	}
	rewind(fp);

	im = gdImageCreateFromGd2(fp);
	fclose(fp);

	gdTestAssertMsg(im == NULL, "Image should have failed to be loaded\n");

	return gdNumFailures();
}
