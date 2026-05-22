#include "gd.h"
#include "gdtest.h"
#include <math.h>

int main()
{
	gdImagePtr im;
	gdImagePtr im_affine = NULL;
	gdImagePtr im_rotate = NULL;
	double affine[6];
	double rot;
	const double deg = -1.0;
	int error = 0;
	int white;
	int black;

	im = gdImageCreateTrueColor(400, 300);
	if (!gdTestAssert(im != NULL)) {
		return 1;
	}

	white = gdImageColorAllocate(im, 255, 255, 255);
	black = gdImageColorAllocate(im, 0, 0, 0);
	gdImageFilledRectangle(im, 0, 0, gdImageSX(im) - 1, gdImageSY(im) - 1, white);
	gdImageLine(im, 0, 0, gdImageSX(im) - 1, gdImageSY(im) - 1, black);
	gdImageLine(im, gdImageSX(im) - 1, 0, 0, gdImageSY(im) - 1, black);
	gdImageFilledEllipse(im, gdImageSX(im) / 2, gdImageSY(im) / 2, 120, 80, black);
    FILE *f2;
    f2 = fopen("rotate_input.png", "wb");
    if (f2) {
        gdImagePng(im, f2);
        fclose(f2);
    }
	/* imageaffine([cos(rot), sin(rot), -sin(rot), cos(rot), 0, 0]) */
	rot = deg * M_PI / 180.0;
	affine[0] = cos(rot);
	affine[1] = sin(rot);
	affine[2] = -sin(rot);
	affine[3] = cos(rot);
	affine[4] = 0.0;
	affine[5] = 0.0;

	if (!gdTestAssert(gdTransformAffineGetImage(&im_affine, im, NULL, affine) == GD_TRUE)) {
		error = 1;
		goto out;
	}
	if (!gdTestAssert(im_affine != NULL)) {
		error = 1;
		goto out;
	}

	/* PHP calls gdImageRotateInterpolated() with degree input directly. */
	im_rotate = gdImageRotateInterpolated(im, (float)(-deg), 0);
	if (!gdTestAssert(im_rotate != NULL)) {
		error = 1;
		goto out;
	}

	/* Both paths should produce a valid transformed canvas. */
	if (!gdTestAssert(gdImageSX(im_affine) > 0 && gdImageSY(im_affine) > 0)) {
		error = 1;
	}
	if (!gdTestAssert(gdImageSX(im_rotate) > 0 && gdImageSY(im_rotate) > 0)) {
		error = 1;
	}
    FILE *f;
    f = fopen("rotate_affine.png", "wb");
    if (f) {
        gdImagePng(im_affine, f);
        fclose(f);
    }
    f = fopen("rotate_interpolated.png", "wb");
    if (f) {
        gdImagePng(im_rotate, f);
        fclose(f);
    }
out:
	gdImageDestroy(im_rotate);
	gdImageDestroy(im_affine);
	gdImageDestroy(im);

	if (error) {
		return 1;
	}

	return gdNumFailures();
}