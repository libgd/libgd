/**
 * Base test for gdImageString()
 */
#include <gd.h>
#include <gdfontl.h>
#include "gdtest.h"
#include <string.h>

int main()
{
	/* Declare the image */
	gdImagePtr im = NULL;
	char *s = "Hello gd";
	int foreground = 0;
	int error = 0;
	gdFontPtr fontptr = gdFontGetLarge();

	im = gdImageCreate(100, 50);
	gdImageColorAllocate(im, 202, 202, 0);
	foreground = gdImageColorAllocate(im, 22, 4, 238);

	gdImageString(im, fontptr,
			im->sx / 2 - (strlen(s) * fontptr->w / 2),
			im->sy / 2 - fontptr->h / 2,
			(unsigned char*)s, foreground);

	if (!gdAssertImageEqualsToFile("gdimagestring/gdimagestring_exp.png", im))
		error= 1;

	/* Destroy the image in memory. */
	gdImageDestroy(im);

	return error;
}
