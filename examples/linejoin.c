#include "gd.h"
#include <stdio.h>


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

    gdContextSetLineWidth(cr, 40.96);
    gdContextMoveTo(cr, 76.8, 84.48);
    gdContextRelLineTo(cr, 51.2, -51.2);
    gdContextRelLineTo(cr, 51.2, 51.2);
    gdContextSetLineJoin(cr, gdLineJoinMiter); /* default */
    gdContextStroke(cr);

    gdContextMoveTo(cr, 76.8, 161.28);
    gdContextRelLineTo(cr, 51.2, -51.2);
    gdContextRelLineTo(cr, 51.2, 51.2);
    gdContextSetLineJoin(cr, gdLineJoinBevel);
    gdContextStroke(cr);

    gdContextMoveTo(cr, 76.8, 238.08);
    gdContextRelLineTo(cr, 51.2, -51.2);
    gdContextRelLineTo(cr, 51.2, 51.2);
    gdContextSetLineJoin(cr, gdLineJoinRound);
    gdContextStroke(cr);

    save_png(surface, "linejoin.png");
    gdContextDestroy(cr);
    gdSurfaceDestroy(surface);
    return 0;
}
