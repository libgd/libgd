#include "vector2d_example.h"
#include <stdio.h>
#include <stdlib.h>


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
    gdContextMoveTo(cr, 128.0, 25.6);
    gdContextLineTo(cr, 230.4, 230.4);
    gdContextRelLineTo(cr, -102.4, 0.0);
    gdContextCurveTo(cr, 51.2, 230.4, 51.2, 128.0, 128.0, 128.0);
    gdContextClosePath(cr);


    gdContextMoveTo(cr, 64.0, 25.6);
    gdContextRelLineTo(cr, 51.2, 51.2);
    gdContextRelLineTo(cr, -51.2, 51.2);
    gdContextRelLineTo(cr, -51.2, -51.2);
    gdContextClosePath(cr);

    gdContextSetLineWidth(cr, 10.0);
    gdContextSetSourceRgba(cr, 0, 0, 1,0.5);
    gdContextFillPreserve(cr);
    gdContextSetSourceRgba(cr, 0, 0, 0, 1.0);
    gdContextStroke(cr);

    gdContextFlushImage(cr);
    vector2d_save_png(image, "fillandstroke.png");
    gdContextDestroy(cr);
    gdImageDestroy(image);
    return 0;
}
