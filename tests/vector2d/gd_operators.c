#include "gd_vector2d.h"
#include "gdtest.h"

#include <stdio.h>

static const char *operator_names[GD_OP_COUNT] = {
	"CLEAR", "SOURCE", "OVER", "IN", "OUT", "ATOP", "DEST", "DEST_OVER",
	"DEST_IN", "DEST_OUT", "DEST_ATOP", "XOR", "ADD", "SATURATE",
	"MULTIPLY", "SCREEN", "OVERLAY", "DARKEN", "LIGHTEN", "COLOR_DODGE",
	"COLOR_BURN", "HARD_LIGHT", "SOFT_LIGHT", "DIFFERENCE", "EXCLUSION",
	"HSL_HUE", "HSL_SATURATION", "HSL_COLOR", "HSL_LUMINOSITY"
};

static const char *reference_names[GD_OP_COUNT] = {
	"clear", "source", "over", "in", "out", "atop", "dest", "dest_over",
	"dest_in", "dest_out", "dest_atop", "xor", "add", "saturate",
	"multiply", "screen", "overlay", "darken", "lighten", "color_dodge",
	"color_burn", "hard_light", "soft_light", "difference", "exclusion",
	"hsl_hue", "hsl_saturation", "hsl_color", "hsl_luminosity"
};

static gdImagePtr render_gd(gdCompositeOperator op)
{
	gdImagePtr image = gdImageCreateTrueColor(160, 120);
	gdContextPtr context;
	if (!image)
		return NULL;
	gdImageAlphaBlending(image, 0);
	gdImageFilledRectangle(image, 0, 0, 159, 119,
		gdTrueColorAlpha(255, 255, 255, 127));
	gdImageAlphaBlending(image, 1);
	gdImageSaveAlpha(image, 1);

	context = gdContextCreateForImage(image);
	if (!context) {
		gdImageDestroy(image);
		return NULL;
	}
	gdContextSetSourceRgba(context, .7, 0, 0, .8);
	gdContextRectangle(context, 0, 0, 120, 90);
	gdContextFill(context);
	gdContextSetOperator(context, op);
	gdContextSetSourceRgba(context, 0, 0, .9, .4);
	gdContextRectangle(context, 40, 30, 120, 90);
	gdContextFill(context);
	gdContextDestroy(context);
	return image;
}

int main(void)
{
	int op;
	for (op = 0; op < GD_OP_COUNT; op++) {
		char reference_path[128];
		gdImagePtr expected;
		gdImagePtr actual = render_gd((gdCompositeOperator) op);
		CuTestImageResult result = {0, 0};
		snprintf(reference_path, sizeof(reference_path),
			"vector2d/operator_reference/%s.png", reference_names[op]);
		expected = gdTestImageFromPng(reference_path);

		gdTestAssertMsg(expected != NULL && actual != NULL,
			"Could not render operator %s", operator_names[op]);
		if (expected && actual) {
			gdTestImagePerceptualDiff(expected, actual, NULL, &result, .01);
			gdTestAssertMsg(result.pixels_changed == 0,
				"%s differs from its reference at %u pixels",
				operator_names[op], result.pixels_changed);
		}
		if (expected)
			gdImageDestroy(expected);
		if (actual)
			gdImageDestroy(actual);
	}
	return gdNumFailures();
}
