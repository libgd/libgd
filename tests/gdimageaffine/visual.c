#include "gd.h"
#include "gdtest.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static gdImagePtr create_visual_source(void)
{
	gdImagePtr im;
	int transparent, white, black, red, green, blue, yellow;

	im = gdImageCreateTrueColor(40, 30);
	if (im == NULL) {
		return NULL;
	}

	gdImageAlphaBlending(im, 0);
	gdImageSaveAlpha(im, 1);

	transparent = gdTrueColorAlpha(0, 0, 0, gdAlphaTransparent);
	white = gdTrueColorAlpha(255, 255, 255, gdAlphaOpaque);
	black = gdTrueColorAlpha(0, 0, 0, gdAlphaOpaque);
	red = gdTrueColorAlpha(220, 30, 30, gdAlphaOpaque);
	green = gdTrueColorAlpha(30, 180, 70, gdAlphaOpaque);
	blue = gdTrueColorAlpha(40, 90, 220, gdAlphaOpaque);
	yellow = gdTrueColorAlpha(240, 210, 30, 24);

	gdImageFilledRectangle(im, 0, 0, 39, 29, transparent);
	gdImageFilledRectangle(im, 4, 4, 35, 25, white);
	gdImageLine(im, 4, 4, 35, 25, black);
	gdImageLine(im, 35, 4, 4, 25, black);
	gdImageFilledRectangle(im, 5, 5, 12, 12, red);
	gdImageFilledRectangle(im, 27, 5, 34, 12, green);
	gdImageFilledRectangle(im, 5, 17, 12, 24, blue);
	gdImageFilledEllipse(im, 28, 20, 12, 10, yellow);

	return im;
}

static int write_png(const char *path, gdImagePtr im)
{
	FILE *fp;

	fp = fopen(path, "wb");
	if (fp == NULL) {
		return GD_FALSE;
	}
	gdImagePng(im, fp);
	fclose(fp);
	return GD_TRUE;
}

int main()
{
	gdImagePtr src = NULL, actual = NULL, ref = NULL, diff = NULL;
	gdRect src_region = {0, 0, 40, 30};
	double rotate[6], scale[6], shear[6], translate[6], tmp[6], mixed[6];
	CuTestImageResult result = {0, 0};
	char *ref_path = NULL;
	int error = 0;

	src = create_visual_source();
	if (!gdTestAssert(src != NULL)) {
		return gdNumFailures();
	}

	gdImageSetInterpolationMethod(src, GD_BICUBIC);
	gdAffineRotate(rotate, 7.0);
	gdAffineScale(scale, 1.2, 0.9);
	gdAffineShearHorizontal(shear, 8.0);
	gdAffineTranslate(translate, 3.0, -2.0);
	gdAffineConcat(tmp, rotate, scale);
	gdAffineConcat(mixed, tmp, shear);
	gdAffineConcat(tmp, mixed, translate);
	memcpy(mixed, tmp, sizeof(mixed));

	if (!gdTestAssert(gdTransformAffineGetImage(&actual, src, &src_region, mixed) == GD_TRUE)) {
		error = 1;
		goto done;
	}
	if (!gdTestAssert(actual != NULL)) {
		error = 1;
		goto done;
	}

	ref_path = gdTestFilePath("gdimageaffine/visual_mixed.png");
	if (getenv("GD_UPDATE_AFFINE_VISUAL_FIXTURE") != NULL) {
		if (!write_png(ref_path, actual)) {
			gdTestErrorMsg("failed to write affine visual reference image\n");
			error = 1;
		}
		goto done;
	}

	ref = gdTestImageFromPng("gdimageaffine/visual_mixed.png");
	if (ref == NULL) {
		gdTestErrorMsg("failed to load affine visual reference image\n");
		error = 1;
		goto done;
	}
	if (gdImageSX(actual) != gdImageSX(ref) || gdImageSY(actual) != gdImageSY(ref)) {
		gdTestErrorMsg("affine visual dimensions changed: got %dx%d, expected %dx%d\n",
		               gdImageSX(actual), gdImageSY(actual), gdImageSX(ref), gdImageSY(ref));
		error = 1;
		goto done;
	}

	diff = gdImageCreateTrueColor(gdImageSX(actual), gdImageSY(actual));
	if (!gdTestAssert(diff != NULL)) {
		error = 1;
		goto done;
	}

	gdTestImagePerceptualDiff(actual, ref, diff, &result, 0.05);
	if (result.pixels_changed > 0) {
		write_png("gdimageaffine_visual_diff.png", diff);
		write_png("/tmp/gdimageaffine_visual_actual.png", actual);
		write_png("/tmp/gdimageaffine_visual_ref.png", ref);
		gdTestErrorMsg("affine visual perceptual diff changed %u pixels\n", result.pixels_changed);
		error = 1;
	}

done:
	if (ref_path != NULL) {
		free(ref_path);
	}
	if (diff != NULL) {
		gdImageDestroy(diff);
	}
	if (ref != NULL) {
		gdImageDestroy(ref);
	}
	if (actual != NULL) {
		gdImageDestroy(actual);
	}
	if (src != NULL) {
		gdImageDestroy(src);
	}

	return error || gdNumFailures();
}
