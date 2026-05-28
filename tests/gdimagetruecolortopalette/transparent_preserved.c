#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "gd.h"
#include "gdtest.h"

static const char *method_name(enum gdPaletteQuantizationMethod method)
{
	switch (method) {
	case GD_QUANT_DEFAULT:
		return "GD_QUANT_DEFAULT";
	case GD_QUANT_JQUANT:
		return "GD_QUANT_JQUANT";
	case GD_QUANT_NEUQUANT:
		return "GD_QUANT_NEUQUANT";
	case GD_QUANT_LIQ:
		return "GD_QUANT_LIQ";
	default:
		return "unknown";
	}
}

static gdImagePtr create_source_image(void)
{
	gdImagePtr im;
	int transparent;

	im = gdImageCreateTrueColor(8, 8);
	if (im == NULL) {
		return NULL;
	}

	gdImageFilledRectangle(im, 0, 0, 7, 7, gdTrueColor(255, 0, 0));
	transparent = gdTrueColor(0, 0, 0);
	gdImageSetPixel(im, 0, 0, transparent);
	gdImageColorTransparent(im, transparent);

	return im;
}

static void assert_transparent_preserved(gdImagePtr im, enum gdPaletteQuantizationMethod method, const char *mode)
{
	int transparent = gdImageGetTransparent(im);
	int transparent_pixel = gdImageGetPixel(im, 0, 0);
	int opaque_pixel = gdImageGetPixel(im, 1, 0);

	gdTestAssertMsg(!gdImageTrueColor(im),
		"%s %s result must be palette", method_name(method), mode);
	gdTestAssertMsg(transparent >= 0 && transparent < gdImageColorsTotal(im),
		"%s %s transparent index must be valid, got %d",
		method_name(method), mode, transparent);
	if (transparent >= 0 && transparent < gdImageColorsTotal(im)) {
		gdTestAssertMsg(im->alpha[transparent] == gdAlphaTransparent,
			"%s %s transparent alpha must be %d, got %d",
			method_name(method), mode, gdAlphaTransparent, im->alpha[transparent]);
	}
	gdTestAssertMsg(transparent_pixel == transparent,
		"%s %s transparent pixel must map to transparent index %d, got %d",
		method_name(method), mode, transparent, transparent_pixel);
	gdTestAssertMsg(opaque_pixel != transparent,
		"%s %s opaque pixel must not map to transparent index %d",
		method_name(method), mode, transparent);
}

static void test_in_place(enum gdPaletteQuantizationMethod method)
{
	gdImagePtr im;

	im = create_source_image();
	if (!gdTestAssertMsg(im != NULL, "%s in-place source image", method_name(method))) {
		return;
	}

	gdTestAssertMsg(gdImageTrueColorToPaletteSetMethod(im, method, 0),
		"%s in-place set method", method_name(method));
	gdTestAssertMsg(gdImageTrueColorToPalette(im, 0, 256),
		"%s in-place conversion", method_name(method));
	assert_transparent_preserved(im, method, "in-place");

	gdImageDestroy(im);
}

static void test_create_palette(enum gdPaletteQuantizationMethod method)
{
	gdImagePtr im;
	gdImagePtr pal;

	im = create_source_image();
	if (!gdTestAssertMsg(im != NULL, "%s create source image", method_name(method))) {
		return;
	}

	gdTestAssertMsg(gdImageTrueColorToPaletteSetMethod(im, method, 0),
		"%s create set method", method_name(method));
	pal = gdImageCreatePaletteFromTrueColor(im, 0, 256);
	if (gdTestAssertMsg(pal != NULL, "%s create conversion", method_name(method))) {
		assert_transparent_preserved(pal, method, "create");
		gdImageDestroy(pal);
	}

	gdImageDestroy(im);
}

static void test_method(enum gdPaletteQuantizationMethod method)
{
	test_in_place(method);
	test_create_palette(method);
}

int main(void)
{
	gdSetErrorMethod(gdSilence);

	test_method(GD_QUANT_JQUANT);
	test_method(GD_QUANT_NEUQUANT);
	test_method(GD_QUANT_DEFAULT);
#ifdef HAVE_LIBIMAGEQUANT
	test_method(GD_QUANT_LIQ);
#endif

	gdClearErrorMethod();

	return gdNumFailures();
}
