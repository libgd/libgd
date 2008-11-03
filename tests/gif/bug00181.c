#include <stdio.h>
#include <gd.h>
#include <gdtest.h>

int main()
{
	gdImagePtr im, im2, im3;
	FILE *fp;
    int black, white, trans;
	int error = 0;

	/* GIFEncode */
	im = gdImageCreate(100, 100);
	im->interlace = 1;
	fp = fopen("bug00181.gif", "wb");
	gdImageGif(im, fp);
	gdImageDestroy(im);
	fclose(fp);

	fp = fopen("bug00181.gif", "rb");
	im = gdImageCreateFromGif(fp);
	error = !im->interlace;
	gdImageDestroy(im);
	fclose(fp);

	if (error) return error;

	/* GIFAnimEncode */
	im = gdImageCreate(100, 100);
	im->interlace = 1;
    white = gdImageColorAllocate(im, 255, 255, 255);
    black = gdImageColorAllocate(im, 0, 0, 0);
    trans = gdImageColorAllocate(im, 1, 1, 1);
    gdImageRectangle(im, 0, 0, 10, 10, black);
	fp = fopen("bug00181a.gif", "wb");
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

	fp = fopen("bug00181a.gif", "rb");
	im = gdImageCreateFromGif(fp);
	error = !im->interlace;
	gdImageDestroy(im);
	fclose(fp);

	return error;
}
