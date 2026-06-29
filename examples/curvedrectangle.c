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

    double x0 = 25.6,
           y0 = 25.6,
           rect_width = 204.8,
           rect_height = 204.8,
           radius = 102.4;

    double x1 = x0 + rect_width,
           y1 = y0 + rect_height;

    if (rect_width / 2 < radius)
    {
        if (rect_height / 2 < radius)
        {
            gdContextMoveTo(cr, x0, (y0 + y1) / 2);
            gdContextCurveTo(cr, x0, y0, x0, y0, (x0 + x1) / 2, y0);
            gdContextCurveTo(cr, x1, y0, x1, y0, x1, (y0 + y1) / 2);
            gdContextCurveTo(cr, x1, y1, x1, y1, (x1 + x0) / 2, y1);
            gdContextCurveTo(cr, x0, y1, x0, y1, x0, (y0 + y1) / 2);
        }
        else
        {
            gdContextMoveTo(cr, x0, y0 + radius);
            gdContextCurveTo(cr, x0, y0, x0, y0, (x0 + x1) / 2, y0);
            gdContextCurveTo(cr, x1, y0, x1, y0, x1, y0 + radius);
            gdContextLineTo(cr, x1, y1 - radius);
            gdContextCurveTo(cr, x1, y1, x1, y1, (x1 + x0) / 2, y1);
            gdContextCurveTo(cr, x0, y1, x0, y1, x0, y1 - radius);
        }
    }
    else
    {
        if (rect_height / 2 < radius)
        {
            gdContextMoveTo(cr, x0, (y0 + y1) / 2);
            gdContextCurveTo(cr, x0, y0, x0, y0, x0 + radius, y0);
            gdContextLineTo(cr, x1 - radius, y0);
            gdContextCurveTo(cr, x1, y0, x1, y0, x1, (y0 + y1) / 2);
            gdContextCurveTo(cr, x1, y1, x1, y1, x1 - radius, y1);
            gdContextLineTo(cr, x0 + radius, y1);
            gdContextCurveTo(cr, x0, y1, x0, y1, x0, (y0 + y1) / 2);
        }
        else
        {
            gdContextMoveTo(cr, x0, y0 + radius);
            gdContextCurveTo(cr, x0, y0, x0, y0, x0 + radius, y0);
            gdContextLineTo(cr, x1 - radius, y0);
            gdContextCurveTo(cr, x1, y0, x1, y0, x1, y0 + radius);
            gdContextLineTo(cr, x1, y1 - radius);
            gdContextCurveTo(cr, x1, y1, x1, y1, x1 - radius, y1);
            gdContextLineTo(cr, x0 + radius, y1);
            gdContextCurveTo(cr, x0, y1, x0, y1, x0, y1 - radius);
        }
    }
    gdContextClosePath(cr);

    gdContextSetSourceRgba(cr, 0.5, 0.5, 1, 1);
    gdContextFillPreserve(cr);
    gdContextSetSourceRgba(cr, 0.5, 0, 0, 0.5);
    gdContextSetLineWidth(cr, 10.0);
    gdContextStroke(cr);

    gdContextFlushImage(cr);
    vector2d_save_png(image, "curvedrectangle.png");
    gdContextDestroy(cr);
    gdImageDestroy(image);
    return 0;
}
