/*
	Testing thick styled horizontal, vertical and diagonal lines.

	We're drawing four triangles with basically the same pattern style. Each
	triangle has exactly the same shape (right isosceles, with the hypotenuse
	being diagonal) and size. The triangles are arranged from lower left to
	upper right with increasing line thickness.

	To have the same pattern style, it has to be adapted according to the
	thickness (i.e. the pixels have to be repeated thickness times), which
	happens in setStyleAndThickness().

	See also <https://github.com/libgd/libgd/issues/167>.
*/

#include "gd.h"
#include "gdtest.h"

static void setStyleAndThickness(gdImagePtr im, int color, int thickness)
{
	int style[192], i = 0;

	while (i < 16 * thickness) {
		style[i++] = color;
	}
	while (i < 20 * thickness) {
		style[i++] = gdTransparent;
	}
	while (i < 28 * thickness) {
		style[i++] = color;
	}
	while (i < 32 * thickness) {
		style[i++] = gdTransparent;
	}
	gdImageSetStyle(im, style, 32 * thickness);
	gdImageSetThickness(im, thickness);
}

int main()
{
	gdImagePtr im;
	int error = 0, black;

	im = gdImageCreate(800, 800);
	gdImageColorAllocate(im, 255, 255, 255);
	black = gdImageColorAllocate(im, 0, 0, 0);

	setStyleAndThickness(im, black, 1);
	gdImageLine(im,  50, 250, 550, 250, gdStyled);
	gdImageLine(im, 550, 250, 550, 750, gdStyled);
	gdImageLine(im, 550, 750,  50, 250, gdStyled);

	setStyleAndThickness(im, black, 2);
	gdImageLine(im, 100, 200, 600, 200, gdStyled);
	gdImageLine(im, 600, 200, 600, 700, gdStyled);
	gdImageLine(im, 600, 700, 100, 200, gdStyled);

	setStyleAndThickness(im, black, 4);
	gdImageLine(im, 150, 150, 650, 150, gdStyled);
	gdImageLine(im, 650, 150, 650, 650, gdStyled);
	gdImageLine(im, 650, 650, 150, 150, gdStyled);

	setStyleAndThickness(im, black, 6);
	gdImageLine(im, 200, 100, 700, 100, gdStyled);
	gdImageLine(im, 700, 100, 700, 600, gdStyled);
	gdImageLine(im, 700, 600, 200, 100, gdStyled);

	if (!gdAssertImageEqualsToFile("gdimageline/github_bug_167_exp.png", im)) {
		error = 1;
	}

	gdImageDestroy(im);

	return error;
}
