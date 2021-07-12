#include "gd_surface.h"
#include "gd.h"
#include <stdio.h>
#include <stdlib.h>

void save_png(gdSurfacePtr surface, const char *filename)
{
    FILE *fp;
    fp = fopen(filename, "wb");
    if (!fp)
    {
        fprintf(stderr, "Can't save png image %s\n", filename);
        return;
    }
    gdSurfacePng(surface, fp);
    fclose(fp);
}

int main()
{
    const int width = 150;
    const int height = 150;
    gdSurface *surface;
    unsigned char *data;

    surface = gdSurfaceCreate(width, height, GD_SURFACE_ARGB32);
    if (!surface)
    {
        fprintf(stderr, "Can't create 400x400 surface\n");
        return 1;
    }

    //gdContextPtr cr = gdContextCreate(surface);
    gdContextPtr cr = gdContextCreate(surface);

    data = gdSurfaceGetData(surface);
    for (int y = 0; y < height; y++)
    {
        unsigned int *img = (unsigned int *)(data + surface->stride * y);
        for (int x = 0; x < width; x++)
        {
            img[x] = 0xFFFFFFFF;
        }
    }



    double center_x = width / 2;
    double center_y = height / 2;
    double radius = 70;
    double eye_radius = 10;
    double eye_offset_x = 25;
    double eye_offset_y = 20;
    double eye_x = center_x - eye_offset_x;
    double eye_y = center_y - eye_offset_y;

    const double pi = 3.14159265358979323846;

    gdContextArc(cr, center_x, center_y, radius, 0, 2 * pi);
    gdContextSetSourceRgba(cr, 1, 1, 0, 1);
    gdContextFillPreserve(cr);
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

    save_png(surface, "smiley.png");
    gdContextDestroy(cr);
    gdSurfaceDestroy(surface);
    return 0;
}
