/**
 * Base test for gdImageStringUp16()
 */
#include <gd.h>
#include <gdfontl.h>
#include "gdtest.h"

int main()
{
	/* Declare the image */
	gdImagePtr im = NULL;
	wchar_t *wchr = L"H";
	unsigned short *sptr;
	sptr = (unsigned short *)wchr;
	int foreground;
	int errorcode = 0;
	gdFontPtr fontptr = gdFontGetLarge();

	im = gdImageCreate(20, 20);
	gdImageColorAllocate(im, 255, 255, 255);
	foreground = gdImageColorAllocate(im, 22, 4, 238);

	gdImageStringUp16(im, fontptr, 2, 18, sptr, foreground);

	if (!gdAssertImageEqualsToFile("gdimagestringup16/gdimagestringup16_exp.png", im))
		errorcode = 1;

	gdImageDestroy(im);

	return errorcode;
}
