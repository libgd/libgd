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

    save_png(surface, "fillandstroke.png");
    gdContextDestroy(cr);
    gdSurfaceDestroy(surface);
    return 0;
}
