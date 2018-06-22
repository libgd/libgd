#include <stdio.h>
#include "gd.h"
#include "gdtest.h"

int main()
{
	gdImagePtr im, im2, im3;
	char *file;
	FILE *fp;
	int black, trans;
	int error = 0;

	/* GIFEncode */
	im = gdImageCreate(100, 100);
        if (!im) {
                gdTestErrorMsg("Cannot create image.\n");
                return 1;
        }
	im->interlace = 1;
	file = gdTestTempFile("bug00181.gif");
	fp = fopen(file, "wb");
	if (!fp) {
		gdTestErrorMsg("Cannot open <%s> for writing.\n", file);
		return 1;
	}
	gdImageGif(im, fp);
	gdImageDestroy(im);
	fclose(fp);

	fp = fopen(file, "rb");
	if (!fp) {
		gdTestErrorMsg("Cannot open <%s> for reading.\n", file);
		return 1;
	}
	im = gdImageCreateFromGif(fp);
	fclose(fp);
        if (!im) {
                gdTestErrorMsg("Cannot create image from <%s>\n", file);
                return 1;
        }
	error = !im->interlace;
	gdImageDestroy(im);
	free(file);

	if (error) return error;

	/* GIFAnimEncode */
	im = gdImageCreate(100, 100);
	im->interlace = 1;
	gdImageColorAllocate(im, 255, 255, 255); /* allocate white for background color */
	black = gdImageColorAllocate(im, 0, 0, 0);
	trans = gdImageColorAllocate(im, 1, 1, 1);
	gdImageRectangle(im, 0, 0, 10, 10, black);
	file = gdTestTempFile("bug00181a.gif");
	fp = fopen(file, "wb");
	if (!fp) {
		gdTestErrorMsg("Cannot open <%s> for writing.\n", file);
		return 1;
	}
	gdImageGifAnimBegin(im, fp, 1, 3);
	gdImageGifAnimAdd(im, fp, 0, 0, 0, 100, 1, NULL);
	im2 = gdImageCreate(100, 100);
	im2->interlace = 1;
	(void)gdImageColorAllocate(im2, 255, 255, 255);
	gdImagePaletteCopy (im2, im);
	gdImageRectangle(im2, 0, 0, 15, 15, black);
	gdImageColorTransparent (im2, trans);
	gdImageGifAnimAdd(im2, fp, 0, 0, 0, 100, 1, im);
	im3 = gdImageCreate(100, 100);
	im3->interlace = 1;
	(void)gdImageColorAllocate(im3, 255, 255, 255);
	gdImagePaletteCopy (im3, im);
	gdImageRectangle(im3, 0, 0, 15, 20, black);
	gdImageColorTransparent (im3, trans);
	gdImageGifAnimAdd(im3, fp, 0, 0, 0, 100, 1, im2);
	gdImageGifAnimEnd(fp);
	fclose(fp);
	gdImageDestroy(im);
	gdImageDestroy(im2);
	gdImageDestroy(im3);

	fp = fopen(file, "rb");
	if (!fp) {
		gdTestErrorMsg("Cannot open <%s> for reading.\n", file);
		return 1;
	}
	im = gdImageCreateFromGif(fp);
	fclose(fp);
        if (!im) {
                gdTestErrorMsg("Cannot create image from <%s>\n", file);
                return 1;
        }
	error = !im->interlace;
	gdImageDestroy(im);
	free(file);

	return error;
}
