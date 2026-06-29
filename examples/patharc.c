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

    double xc = 128.0;
    double yc = 128.0;
    double radius = 100.0;
    double angle1 = 45.0 * (M_PI / 180.0);  /* angles are specified */
    double angle2 = 270.0 * (M_PI / 180.0); /* in radians           */

    gdContextSetLineWidth(cr, 10.0);
    gdContextArc(cr, xc, yc, radius, angle1, angle2);
    gdContextStroke(cr);
 
    /* draw helping lines */
    gdContextSetSourceRgba(cr, 1, 0.2, 0.2, 0.6);
    gdContextArc(cr, xc, yc, 10.0, 0, 2 * M_PI);
    gdContextFill(cr);

    gdContextSetLineWidth(cr, 6.0);
    gdContextArc(cr, xc, yc, radius, angle1, angle1);
    gdContextLineTo(cr, xc, yc);
    gdContextArc(cr, xc, yc, radius, angle2, angle2);
    gdContextLineTo(cr, xc, yc);
    gdContextStroke(cr);

    gdContextFlushImage(cr);
    vector2d_save_png(image, "patharc.png");
    gdContextDestroy(cr);
    gdImageDestroy(image);
    return 0;
}
