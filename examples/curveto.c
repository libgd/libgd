#include "vector2d_example.h"
#include <stdio.h>


int main()
{
    gdImagePtr image;

    image = vector2d_create_image(256, 256, gdTrueColorAlpha(255, 255, 255, 127));
    if (!image)
    {
        fprintf(stderr, "Can't create image\n");
        return 1;
    }
    
    gdContextPtr cr = gdContextCreateForImage(image);

    double x = 25.6, y = 128.0;
    double x1 = 102.4, y1 = 230.4,
           x2 = 153.6, y2 = 25.6,
           x3 = 230.4, y3 = 128.0;
    double dash[7] = {10, 5, 20, 5, 30, 5, 40};

    gdContextSetDash(cr, 0, dash, 4);
    gdContextSetSourceRgba(cr, 0, 0.0, 0.0, 1.0);
    gdContextMoveTo(cr, x, y);
    gdContextCurveTo(cr, x1, y1, x2, y2, x3, y3);
    gdContextSetLineWidth(cr, 10.0);
    gdContextStroke(cr);

    gdContextSetSourceRgba(cr, 1, 0.2, 0.2, 0.6);
    gdContextSetLineWidth(cr, 6.0);
    gdContextMoveTo(cr, x, y);
    gdContextLineTo(cr, x1, y1);
    gdContextMoveTo(cr, x2, y2);
    gdContextLineTo(cr, x3, y3);

    gdContextStroke(cr);
    gdContextFlushImage(cr);
    vector2d_save_png(image, "curveto.png");
    gdContextDestroy(cr);
    gdImageDestroy(image);
    return 0;
}
