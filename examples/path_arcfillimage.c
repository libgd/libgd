#include "vector2d_example.h"
#include <math.h>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

int main(void)
{
	gdImagePtr source = vector2d_create_checker(128, 128);
	gdImagePtr image = vector2d_create_image(256, 256, gdTrueColorAlpha(255, 255, 255, 127));
	gdContextPtr context;
	if (!source || !image) return 1;
	context = gdContextCreateForImage(image);
	if (!context) return 1;
	gdContextArc(context, 128, 128, 76.8, 0, 2 * M_PI);
	gdContextClip(context);
	gdContextScale(context, 2, 2);
	gdContextSetSourceImage(context, source, 0, 0);
	gdContextPaint(context);
	gdContextFlushImage(context);
	vector2d_save_png(image, "path_arcfillimage.png");
	gdContextDestroy(context);
	gdImageDestroy(image);
	gdImageDestroy(source);
	return 0;
}
