#include "gd_vector2d.h"
#include "gdtest.h"

static void test_context_rules_and_flush(void)
{
	gdImagePtr palette = gdImageCreate(2, 2);
	gdImagePtr image = gdImageCreateTrueColor(2, 2);
	gdContextPtr context;
	int red = gdTrueColorAlpha(255, 0, 0, 0);

	gdTestAssert(palette != NULL && image != NULL);
	gdTestAssert(gdContextCreateForImage(palette) == NULL);
	context = gdContextCreateForImage(image);
	gdTestAssert(context != NULL && gdContextGetImage(context) == image);
	gdTestAssert(gdContextSave(context) == 1);
	gdContextTranslate(context, 3.0, 4.0);
	gdTestAssert(gdContextRestore(context) == 1);
	gdContextSetSourceRgb(context, 1.0, 0.0, 0.0);
	gdContextPaint(context);
	gdTestAssert(gdImageGetTrueColorPixel(image, 0, 0) != red);
	gdContextFlushImage(context);
	gdTestAssert(gdImageGetTrueColorPixel(image, 0, 0) == red);
	gdContextSetSourceRgb(context, 0.0, 1.0, 0.0);
	gdContextPaint(context);
	gdContextDestroy(context);
	gdTestAssert(gdTrueColorGetGreen(gdImageGetTrueColorPixel(image, 1, 1)) == 255);
	gdImageDestroy(image);
	gdImageDestroy(palette);
}

static void test_pattern_snapshot(void)
{
	gdImagePtr source = gdImageCreate(1, 1);
	gdImagePtr target = gdImageCreateTrueColor(2, 2);
	gdPathPatternPtr pattern;
	gdPaintPtr paint;
	gdContextPtr context;
	int index;

	gdTestAssert(source != NULL && target != NULL);
	index = gdImageColorAllocateAlpha(source, 12, 34, 56, 0);
	gdImageSetPixel(source, 0, 0, index);
	pattern = gdPathPatternCreateForImage(source);
	gdTestAssert(pattern != NULL);
	gdPathPatternSetExtend(pattern, GD_EXTEND_REPEAT);
	gdImageColorDeallocate(source, index);
	gdImageDestroy(source);
	paint = gdPaintCreateFromPattern(pattern);
	gdPathPatternDestroy(pattern);
	gdTestAssert(paint != NULL);
	context = gdContextCreateForImage(target);
	gdTestAssert(context != NULL);
	gdContextSetSource(context, paint);
	gdPaintDestroy(paint);
	gdContextPaint(context);
	gdContextDestroy(context);
	gdTestAssert(gdTrueColorGetRed(gdImageGetTrueColorPixel(target, 0, 0)) == 12);
	gdTestAssert(gdTrueColorGetGreen(gdImageGetTrueColorPixel(target, 0, 0)) == 34);
	gdTestAssert(gdTrueColorGetBlue(gdImageGetTrueColorPixel(target, 0, 0)) == 56);
	gdTestAssert(gdTrueColorGetRed(gdImageGetTrueColorPixel(target, 1, 1)) == 12);
	gdImageDestroy(target);
}

int main(void)
{
	test_context_rules_and_flush();
	test_pattern_snapshot();
	return gdNumFailures();
}
