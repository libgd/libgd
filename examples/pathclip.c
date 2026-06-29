#include "vector2d_example.h"
#include <stdio.h>


int main()
{
    gdImagePtr image;
    const int width = 256;
    const int height = 256;
    const double pi = M_PI;

    image = vector2d_create_image(width, height, gdTrueColorAlpha(255, 255, 255, 127));
    if (!image)
    {
        fprintf(stderr, "Can't create image\n");
        return 1;
    }
    gdContextPtr cr = gdContextCreateForImage(image);


    double xc = 128.0;
    double yc = 128.0;
    double radius = 100.0;

    gdContextSetLineWidth(cr, 10.0);
    gdContextArc(cr, xc, yc, radius, 0, pi*2);
    gdContextClip(cr);

    gdContextSetSourceRgba(cr, 1, 0.2, 0.2, 0.6);
    gdContextSetLineWidth(cr, 40.0);

    gdContextArc(cr, xc - 100, yc -100, 10.0, 0, 2 * pi);
    gdContextFill(cr);
    gdContextMoveTo(cr, 0, 0);
    gdContextLineTo(cr, width, height);
    gdContextMoveTo(cr, width, 0);
    gdContextLineTo(cr, 0, height);
    gdContextStroke(cr);

    gdContextFlushImage(cr);
    vector2d_save_png(image, "pathclip.png");
    gdContextDestroy(cr);
    gdImageDestroy(image);
    return 0;
}
