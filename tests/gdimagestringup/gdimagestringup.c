/**
 * Base test for gdImageStringUp()
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
	int errorcode = 0;
	int foreground;
	gdFontPtr fontptr = gdFontGetLarge();

	im = gdImageCreate(50, 100);
	gdImageColorAllocate(im, 202, 202, 0);
	foreground = gdImageColorAllocate(im, 22, 4, 238);

	gdImageStringUp(im, fontptr,
			im->sx / 2 - fontptr->h / 2,
			im->sy / 2 + (strlen(s) * fontptr->w / 2),
			(unsigned char*)s, foreground);

	if (!gdAssertImageEqualsToFile("gdimagestringup/gdimagestringup_exp.png", im))
		errorcode = 1;

	/* Destroy the image im memory */
	gdImageDestroy(im);

	return errorcode;
}
