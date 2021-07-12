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
    unsigned char *data;

    surface = gdSurfaceCreate(256, 256, GD_SURFACE_ARGB32);
    if (!surface)
    {
        fprintf(stderr, "Can't create 400x400 surface\n");
        return 1;
    }
    
    gdContextPtr cr = gdContextCreate(surface);
    data = gdSurfaceGetData(surface);
    for (int y = 0; y < 255; y++)
    {
        unsigned int *img = (unsigned int *)(data + surface->stride * y);
        for (int x = 0; x < 255; x++)
        {
            img[x] = 0xFFFFFFFF;
        }
    }

    double x = 25.6, y = 128.0;
    double x1 = 102.4, y1 = 230.4,
           x2 = 153.6, y2 = 25.6,
           x3 = 230.4, y3 = 128.0;

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
    save_png(surface, "curveto.png");
    gdContextDestroy(cr);
    gdSurfaceDestroy(surface);
    return 0;
}
