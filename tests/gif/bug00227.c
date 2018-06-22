#include <stdio.h>
#include <stdlib.h>
#include "gd.h"
#include "gdtest.h"

#define WIDTH 150
#define HEIGHT 1
#define DELAY 100
#define PROBE_SIZE 11

int main()
{
	FILE *fp;
	char *file;
	gdImagePtr im0, im1, im2;
	int i;
	char *buf;

	/* generate a GIF animation */
	im0 = gdImageCreate(WIDTH, HEIGHT);
	if (!im0) return 1;
	for (i = 0; i < WIDTH; i++) {
		int c = gdImageColorAllocate(im0, i, 0xff, 0xff);
		gdImageSetPixel(im0, i, 0, c);
	}

	file = gdTestTempFile("bug00227.gif");
	fp = fopen(file, "wb");
	if (!fp) return 1;

	gdImageGifAnimBegin(im0, fp, 0, 0);

	gdImageGifAnimAdd(im0, fp, 1, 0, 0, DELAY, 1, NULL);

	im1 = gdImageCreate(WIDTH, HEIGHT);
	if (!im1) return 1;
	for (i = 0; i < WIDTH; i++) {
		int c = gdImageColorAllocate(im1, i, 0x00, 0xff);
		gdImageSetPixel(im1, i, 0, c);
	}
	gdImageGifAnimAdd(im1, fp, 1, 0, 0, DELAY, 1, im0);

	im2 = gdImageCreate(WIDTH, HEIGHT);
	if (!im2) return 1;
	for (i = 0; i < WIDTH; i++) {
		int c = gdImageColorAllocate(im2, i, 0xff, 0x00);
		gdImageSetPixel(im2, i, 0, c);
	}
	gdImageGifAnimAdd(im2, fp, 1, 0, 0, DELAY, 1, im1);

	gdImageGifAnimEnd(fp);

	fclose(fp);

	gdImageDestroy(im0);
	gdImageDestroy(im1);
	gdImageDestroy(im2);

	/* check the Global Color Table flag */
	fp = fopen(file, "rb");
	if (!fp) return 1;
	buf = malloc(PROBE_SIZE);
	if (!buf) return 1;
	if (PROBE_SIZE != fread(buf, 1, PROBE_SIZE, fp)) return 1;
	if (buf[PROBE_SIZE-1] & 0x80) return 1;
	free(buf);
	fclose(fp);

	free(file);
	return 0;
}
