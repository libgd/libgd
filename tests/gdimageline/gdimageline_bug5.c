#include <stdio.h>
#include "gd.h"
#include "gdtest.h"

int main() {
	/* Declare the image */
	gdImagePtr im, ref;

	/* Declare output files */
	/* FILE *pngout; */
	int black, white;

	/* If the data seg size is less than 195000,
	 * gdImageCrateTrueColor will return NULL.
	 * See https://github.com/libgd/libgd/issues/621 */
	im = gdImageCreateTrueColor(63318, 771);
	if (gdTestAssertMsg(im != NULL, "gdImageCreateTrueColor() returns NULL\n") == 0) {
		return gdNumFailures();
	}

	/* Allocate the color white (red, green and blue all maximum). */
	white = gdImageColorAllocate(im, 255, 255, 255);
	/* Allocate the color white (red, green and blue all maximum). */
	black = gdImageColorAllocate(im, 0, 0, 0);

	/* white background */
	gdImageFill(im, 1, 1, white);

    /* Make a reference copy. */
    ref = gdImageClone(im);

	gdImageSetAntiAliased(im, black);

	/* This line used to fail. */
	gdImageLine(im, 28562, 631, 34266, 750, gdAntiAliased);

    gdTestAssert(gdMaxPixelDiff(im, ref) > 0);

#if 0
    {
        FILE *pngout;

        /* Open a file for writing. "wb" means "write binary",
         * important under MSDOS, harmless under Unix. */
        pngout = fopen("test.png", "wb");

        /* Output the image to the disk file in PNG format. */
        gdImagePng(im, pngout);

        /* Close the files. */
        fclose(pngout);
    }
#endif

	/* Destroy the image in memory. */
	gdImageDestroy(im);
    gdImageDestroy(ref);

    return gdNumFailures();
}
