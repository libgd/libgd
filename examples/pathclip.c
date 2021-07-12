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
    gdSurface *surface;
    const int width = 256;
    const int height = 256;
    const double pi = M_PI;

    surface = gdSurfaceCreate(width, height, GD_SURFACE_ARGB32);
    if (!surface)
    {
        fprintf(stderr, "Can't create 400x400 surface\n");
        return 1;
    }
    gdContextPtr cr = gdContextCreate(surface);


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

    save_png(surface, "pathclip.png");
    gdContextDestroy(cr);
    gdSurfaceDestroy(surface);
    return 0;
}
