#include <stdio.h>
#include "gd.h"
#include "gdtest.h"

int main() {
	/* Declare the image */
	gdImagePtr im;

	/* Declare output files */
	/* FILE *pngout; */
	int black, white;

	im = gdImageCreateTrueColor(63318, 771);

	/* Allocate the color white (red, green and blue all maximum). */
	white = gdImageColorAllocate(im, 255, 255, 255);
	/* Allocate the color white (red, green and blue all maximum). */
	black = gdImageColorAllocate(im, 0, 0, 0);

	/* white background */
	gdImageFill(im, 1, 1, white);

	gdImageSetAntiAliased(im, black);

	/* This line fails! */
	gdImageLine(im, 28562, 631, 34266, 750, gdAntiAliased);

	/* Open a file for writing. "wb" means "write binary", important under MSDOS, harmless under Unix. */
	/* pngout = fopen("test.png", "wb"); */

	/* Output the image to the disk file in PNG format. */
	/* gdImagePng(im, pngout); */

	/* Close the files. */
	/* fclose(pngout); */

	/* Destroy the image in memory. */
	gdImageDestroy(im);
}
