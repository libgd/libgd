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

    surface = gdSurfaceCreate(256, 256, GD_SURFACE_ARGB32);
    if (!surface)
    {
        fprintf(stderr, "Can't create 400x400 surface\n");
        return 1;
    }

    gdContextPtr cr = gdContextCreate(surface);

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
    gdContextSetLineWidth(cr, 6.0);

    gdContextArc(cr, xc, yc, 10.0, 0, 2 * M_PI);
    gdContextFill(cr);

    gdContextArc(cr, xc, yc, radius, angle1, angle1);
    gdContextLineTo(cr, xc, yc);

    gdContextArc(cr, xc, yc, radius, angle2, angle2);
    gdContextLineTo(cr, xc, yc);

    gdContextStroke(cr);
    save_png(surface, "patharc.png");
    gdContextDestroy(cr);
    gdSurfaceDestroy(surface);
    return 0;
}
