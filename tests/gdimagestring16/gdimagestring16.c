/**
 * Base test for gdImageString16()
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

	gdImageString16(im, fontptr, 2, 2, sptr, foreground);

	if (!gdAssertImageEqualsToFile("gdimagestring16/gdimagestring16_exp.png", im))
		errorcode = 1;

	/* Destroy the image in memory */
	gdImageDestroy(im);

	return errorcode;
}
