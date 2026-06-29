#include "vector2d_example.h"
#include <math.h>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

int main(void)
{
	const int width = 512, height = 512;
	gdImagePtr source = vector2d_create_checker(128, 128);
	gdImagePtr image = vector2d_create_image(width, height, gdTrueColorAlpha(20, 25, 40, 0));
	gdContextPtr context;
	gdPathPatternPtr pattern;
	gdPaintPtr paint;
	gdPathMatrix matrix;
	if (!source || !image) return 1;
	context = gdContextCreateForImage(image);
	pattern = gdPathPatternCreateForImage(source);
	if (!context || !pattern) return 1;
	gdPathPatternSetExtend(pattern, GD_EXTEND_REPEAT);
	gdPathMatrixInitScale(&matrix, 0.4, 0.4);
	gdPathPatternSetMatrix(pattern, &matrix);
	paint = gdPaintCreateFromPattern(pattern);
	gdContextSetSource(context, paint);
	gdContextTranslate(context, width / 2.0, height / 2.0);
	gdContextRotate(context, M_PI / 4);
	gdContextScale(context, 1 / sqrt(2.0), 1 / sqrt(2.0));
	gdContextTranslate(context, -width / 2.0, -height / 2.0);
	gdContextRectangle(context, 0, 0, width, height);
	gdContextFill(context);
	gdContextFlushImage(context);
	vector2d_save_png(image, "path_pattern.png");
	gdPaintDestroy(paint);
	gdPathPatternDestroy(pattern);
	gdContextDestroy(context);
	gdImageDestroy(image);
	gdImageDestroy(source);
	return 0;
}
