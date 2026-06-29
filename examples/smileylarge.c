#include "vector2d_example.h"
#include <stdio.h>


int main()
{
    const int width = 8000;
    const int height = 8000;
    gdImagePtr image;

    image = vector2d_create_image(width, height, gdTrueColorAlpha(255, 255, 255, 127));
    if (!image)
    {
        fprintf(stderr, "Can't create image\n");
        return 1;
    }

    gdContextPtr cr = gdContextCreateForImage(image);



    double center_x = 150 / 2;
    double center_y = 150 / 2;
    double radius = 70;
    double eye_radius = 10;
    double eye_offset_x = 25;
    double eye_offset_y = 20;
    double eye_x = center_x - eye_offset_x;
    double eye_y = center_y - eye_offset_y;

    const double pi = 3.14159265358979323846;
    double dash[7] = {10 , 5, 20, 5, 30, 5, 40};


    gdContextSetSourceRgba(cr, 1, 1, 0, 1);
    gdContextScale(cr, width/150, height/150);
    gdContextArc(cr, center_x, center_y, radius, 0,2* pi);
    gdContextFillPreserve(cr);
    gdContextSetDash(cr, 0, dash, 7);
    gdContextSetLineWidth(cr, 5);
    gdContextSetSourceRgba(cr, 0, 0, 0, 1);
    gdContextStroke(cr);

    gdContextArc(cr, eye_x, eye_y, eye_radius, 0, 2 * pi);
    gdContextArc(cr, center_x + eye_offset_x, eye_y, eye_radius, 0, 2 * pi);
    gdContextSetSourceRgba(cr, 0, 0, 0, 1);
    gdContextFill(cr);

    gdContextArc(cr, center_x, center_y, 50, 0, pi);
    gdContextSetSourceRgba(cr, 0, 0, 0, 1);
    gdContextStroke(cr);

    gdContextFlushImage(cr);
    vector2d_save_png(image, "smileylarge.png");
    gdContextDestroy(cr);
    gdImageDestroy(image);
    return 0;
}
