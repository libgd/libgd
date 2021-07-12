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

    gdContextSetLineWidth(cr, 30.0);
    gdContextSetLineCap(cr, gdLineCapButt); /* default */
    gdContextMoveTo(cr, 64.0, 50.0);
    gdContextLineTo(cr, 64.0, 200.0);
    gdContextStroke(cr);
    gdContextSetLineCap(cr, gdLineCapRound);
    gdContextMoveTo(cr, 128.0, 50.0);
    gdContextLineTo(cr, 128.0, 200.0);
    gdContextStroke(cr);
    gdContextSetLineCap(cr, gdLineCapSquare);
    gdContextMoveTo(cr, 192.0, 50.0);
    gdContextLineTo(cr, 192.0, 200.0);
    gdContextStroke(cr);

    /* draw helping lines */
    gdContextSetSourceRgba(cr, 1, 0.2, 0.2, 1.0);
    gdContextSetLineWidth(cr, 2.56);
    gdContextMoveTo(cr, 64.0, 50.0);
    gdContextLineTo(cr, 64.0, 200.0);
    gdContextMoveTo(cr, 128.0, 50.0);
    gdContextLineTo(cr, 128.0, 200.0);
    gdContextMoveTo(cr, 192.0, 50.0);
    gdContextLineTo(cr, 192.0, 200.0);
    gdContextStroke(cr);

    save_png(surface, "linecap.png");
    gdContextDestroy(cr);
    gdSurfaceDestroy(surface);
    return 0;
}
