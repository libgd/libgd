#include <stdio.h>
#include "gd.h"
#include "gdtest.h"

/*
 * This file is truncated immediately after the LZW minimum code size byte; it
 * has no image data sub-blocks. The reader must reject it cleanly instead of
 * manufacturing pixels from stale LZW bit-buffer contents.
 * Some viewers may show it but it is not a valid GIF file per see:
 *  - GIF89a
 *  - logical screen: 8224 x 8224
 *  - tiny global color table
 *  - image descriptor: 32 x 32 at offset 32,32
 *  - LZW minimum code size: 12
 *  - then EOF immediately
 */
int main()
{
	gdImagePtr im;
	FILE *fp = gdTestFileOpen("gif/ossfuzz5700.gif");
	im = gdImageCreateFromGif(fp);
	fclose(fp);

	gdTestAssert(im == NULL);

	return gdNumFailures();
}
