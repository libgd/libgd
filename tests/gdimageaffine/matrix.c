#include "gd.h"
#include "gdtest.h"
#include <string.h>

static gdImagePtr create_truecolor_pattern(int width, int height)
{
	gdImagePtr im;
	int transparent, white, black, red, green, blue;

	im = gdImageCreateTrueColor(width, height);
	if (im == NULL) {
		return NULL;
	}

	gdImageAlphaBlending(im, 0);
	gdImageSaveAlpha(im, 1);

	transparent = gdTrueColorAlpha(0, 0, 0, gdAlphaTransparent);
	white = gdTrueColorAlpha(255, 255, 255, gdAlphaOpaque);
	black = gdTrueColorAlpha(0, 0, 0, gdAlphaOpaque);
	red = gdTrueColorAlpha(255, 0, 0, gdAlphaOpaque);
	green = gdTrueColorAlpha(0, 255, 0, gdAlphaOpaque);
	blue = gdTrueColorAlpha(0, 0, 255, gdAlphaOpaque);

	gdImageFilledRectangle(im, 0, 0, width - 1, height - 1, transparent);
	gdImageFilledRectangle(im, 1, 1, width - 2, height - 2, white);
	gdImageLine(im, 0, 0, width - 1, height - 1, black);
	gdImageSetPixel(im, 1, 1, red);
	gdImageSetPixel(im, width - 2, 1, green);
	gdImageSetPixel(im, 1, height - 2, blue);

	return im;
}

static void assert_bbox(const char *label, gdRectPtr src, const double affine[6],
                        int x, int y, int width, int height)
{
	gdRect bbox;

	gdTestAssertMsg(gdTransformAffineBoundingBox(src, affine, &bbox) == GD_TRUE,
	                "%s: expected bounding box calculation to succeed", label);
	gdTestAssertMsg(bbox.x == x && bbox.y == y && bbox.width == width && bbox.height == height,
	                "%s: expected bbox {%d,%d,%d,%d}, got {%d,%d,%d,%d}",
	                label, x, y, width, height, bbox.x, bbox.y, bbox.width, bbox.height);
}

static void assert_getimage_dims(const char *label, gdImagePtr src, gdRectPtr src_region,
                                 const double affine[6], int width, int height)
{
	gdImagePtr dst = NULL;

	gdTestAssertMsg(gdTransformAffineGetImage(&dst, src, src_region, affine) == GD_TRUE,
	                "%s: expected affine image creation to succeed", label);
	gdTestAssertMsg(dst != NULL, "%s: expected affine output image", label);
	if (dst != NULL) {
		gdTestAssertMsg(gdImageSX(dst) == width && gdImageSY(dst) == height,
		                "%s: expected output %dx%d, got %dx%d",
		                label, width, height, gdImageSX(dst), gdImageSY(dst));
		gdImageDestroy(dst);
	}
}

static void test_matrix_bounding_boxes_and_images(void)
{
	gdImagePtr src;
	gdRect rect = {0, 0, 8, 6};
	double identity[6], translate[6], scale[6], nonuniform_scale[6], flip[6];
	double shear_h[6], shear_v[6], rotate[6], mixed[6], tmp[6];
	gdRect bbox;
	gdInterpolationMethod old_m, new_m;

	src = create_truecolor_pattern(rect.width, rect.height);
	if (!gdTestAssert(src != NULL)) {
		return;
	}

	gdAffineIdentity(identity);
	assert_bbox("identity", &rect, identity, 0, 0, 8, 6);
	assert_getimage_dims("identity", src, &rect, identity, 8, 6);

	gdAffineTranslate(translate, 3.0, -2.0);
	assert_bbox("translate", &rect, translate, 3, -2, 8, 6);
	assert_getimage_dims("translate", src, &rect, translate, 8, 6);

	gdAffineScale(scale, 2.0, 2.0);
	assert_bbox("uniform scale", &rect, scale, 0, 0, 16, 12);
	assert_getimage_dims("uniform scale", src, &rect, scale, 16, 12);

	gdAffineScale(nonuniform_scale, 2.0, 1.5);
	assert_bbox("nonuniform scale", &rect, nonuniform_scale, 0, 0, 16, 9);
	assert_getimage_dims("nonuniform scale", src, &rect, nonuniform_scale, 16, 9);

	gdAffineScale(flip, -1.0, 1.0);
	assert_bbox("horizontal flip", &rect, flip, -8, 0, 8, 6);
	assert_getimage_dims("horizontal flip", src, &rect, flip, 8, 6);

	gdAffineShearHorizontal(shear_h, 45.0);
	assert_bbox("horizontal shear", &rect, shear_h, 0, 0, 14, 6);
	assert_getimage_dims("horizontal shear", src, &rect, shear_h, 14, 6);

	gdAffineShearVertical(shear_v, 45.0);
	assert_bbox("vertical shear", &rect, shear_v, 0, 0, 8, 14);
	assert_getimage_dims("vertical shear", src, &rect, shear_v, 8, 14);

	gdAffineRotate(rotate, 12.0);
	gdAffineConcat(tmp, rotate, nonuniform_scale);
	gdAffineConcat(mixed, tmp, shear_h);
	gdAffineConcat(tmp, mixed, translate);
	memcpy(mixed, tmp, sizeof(mixed));

	gdTestAssertMsg(gdImageSetInterpolationMethod(src, GD_CATMULLROM) == GD_TRUE,
	                "expected interpolation method setup to succeed");
	old_m = gdImageGetInterpolationMethod(src);
	gdTestAssertMsg(gdTransformAffineBoundingBox(&rect, mixed, &bbox) == GD_TRUE,
	                "mixed transform: expected bounding box calculation to succeed");
	gdTestAssertMsg(bbox.width > 0 && bbox.height > 0,
	                "mixed transform: expected positive bbox, got %dx%d", bbox.width, bbox.height);
	assert_getimage_dims("mixed transform", src, &rect, mixed, bbox.width, bbox.height);
	new_m = gdImageGetInterpolationMethod(src);
	gdTestAssertMsg(new_m == old_m,
	                "mixed transform: expected interpolation method %d to remain %d", new_m, old_m);

	gdImageDestroy(src);
}

static void test_clipped_region_does_not_read_neighboring_pixels(void)
{
	gdImagePtr src, dst = NULL;
	gdRect region = {1, 1, 2, 2};
	double scale[6];
	int red, green, px;

	src = gdImageCreateTrueColor(4, 4);
	if (!gdTestAssert(src != NULL)) {
		return;
	}

	gdImageAlphaBlending(src, 0);
	gdImageSaveAlpha(src, 1);
	red = gdTrueColorAlpha(255, 0, 0, gdAlphaOpaque);
	green = gdTrueColorAlpha(0, 255, 0, gdAlphaOpaque);
	gdImageFilledRectangle(src, 0, 0, 3, 3, red);
	gdImageFilledRectangle(src, 1, 1, 2, 2, green);
	gdImageSetInterpolationMethod(src, GD_BICUBIC);

	gdAffineScale(scale, 2.0, 2.0);
	gdTestAssertMsg(gdTransformAffineGetImage(&dst, src, &region, scale) == GD_TRUE,
	                "clipped region: expected affine image creation to succeed");
	gdTestAssertMsg(dst != NULL, "clipped region: expected affine output image");
	if (dst != NULL) {
		px = gdImageGetTrueColorPixel(dst, 0, 0);
		gdTestAssertMsg(gdTrueColorGetRed(px) == 0,
		                "clipped region: expected no red leakage from outside src_region, got %x", px);
		gdTestAssertMsg(gdTrueColorGetGreen(px) > 0,
		                "clipped region: expected green contribution from inside src_region, got %x", px);
		gdTestAssertMsg(gdTrueColorGetAlpha(px) > gdAlphaOpaque,
		                "clipped region: expected edge sample to blend with transparent outside, got %x", px);
		gdImageDestroy(dst);
	}

	gdImageDestroy(src);
}

static void test_affine_copy_preserves_destination_for_transparent_samples(void)
{
	gdImagePtr src, dst;
	gdRect rect = {0, 0, 4, 4};
	double identity[6];
	int transparent, blue, px;

	src = gdImageCreateTrueColor(4, 4);
	dst = gdImageCreateTrueColor(8, 8);
	if (!gdTestAssert(src != NULL && dst != NULL)) {
		if (src != NULL) {
			gdImageDestroy(src);
		}
		if (dst != NULL) {
			gdImageDestroy(dst);
		}
		return;
	}

	gdImageAlphaBlending(src, 0);
	gdImageSaveAlpha(src, 1);
	gdImageAlphaBlending(dst, 0);
	gdImageSaveAlpha(dst, 1);

	transparent = gdTrueColorAlpha(0, 0, 0, gdAlphaTransparent);
	blue = gdTrueColorAlpha(0, 0, 255, gdAlphaOpaque);
	gdImageFilledRectangle(src, 0, 0, 3, 3, transparent);
	gdImageFilledRectangle(dst, 0, 0, 7, 7, blue);

	gdAffineIdentity(identity);
	gdTestAssertMsg(gdTransformAffineCopy(dst, 2, 2, src, &rect, identity) == GD_TRUE,
	                "copy transparent: expected affine copy to succeed");
	px = gdImageGetTrueColorPixel(dst, 2, 2);
	gdTestAssertMsg(px == blue,
	                "copy transparent: expected destination pixel to be preserved, got %x", px);

	gdImageDestroy(src);
	gdImageDestroy(dst);
}

static void test_palette_source_transform(void)
{
	gdImagePtr src, dst = NULL;
	gdRect rect = {0, 0, 6, 6};
	double shear[6];
	int white, black;

	src = gdImageCreate(6, 6);
	if (!gdTestAssert(src != NULL)) {
		return;
	}

	white = gdImageColorAllocate(src, 255, 255, 255);
	black = gdImageColorAllocate(src, 0, 0, 0);
	gdImageFilledRectangle(src, 0, 0, 5, 5, white);
	gdImageLine(src, 0, 0, 5, 5, black);
	gdAffineShearHorizontal(shear, 20.0);

	gdTestAssertMsg(gdTransformAffineGetImage(&dst, src, &rect, shear) == GD_TRUE,
	                "palette source: expected affine image creation to succeed");
	gdTestAssertMsg(dst != NULL, "palette source: expected affine output image");
	if (dst != NULL) {
		gdTestAssertMsg(gdImageSX(dst) > 0 && gdImageSY(dst) > 0,
		                "palette source: expected nonempty output, got %dx%d",
		                gdImageSX(dst), gdImageSY(dst));
		gdImageDestroy(dst);
	}

	gdImageDestroy(src);
}

int main()
{
	test_matrix_bounding_boxes_and_images();
	test_clipped_region_does_not_read_neighboring_pixels();
	test_affine_copy_preserves_destination_for_transparent_samples();
	test_palette_source_transform();

	return gdNumFailures();
}
