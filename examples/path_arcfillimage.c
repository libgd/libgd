#include "gd_surface.h"
#include "gd.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

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

gdSurfacePtr read_png(const char *filename) {
    FILE *fp;
    fp = fopen(filename, "rb");
    if (!fp)
    {
        fprintf(stderr, "Can't read png image %s\n", filename);
        return NULL;
    }
    gdSurfacePtr surface = gdSurfaceCreateFromPng(fp);
    fclose(fp);
    return surface;
}
int main()
{
    int width = 256, height = 256;
    int w,h;
    gdSurface *surface;
    unsigned char *data;
    gdContextPtr cr;
    gdSurfacePtr surface_fill = read_png("../examples/images/isar_512x.png");
    if (!surface_fill) {
        fprintf(stderr,  "can't read image\n");
        return 1;
    }
    w = gdSurfaceGetWidth(surface_fill);
    h = gdSurfaceGetHeight(surface_fill);
    surface = gdSurfaceCreate(width, height, GD_SURFACE_ARGB32);
    if (!surface)
    {
        fprintf(stderr, "Can't create 400x400 surface\n");
        return 1;
    }

    cr = gdContextCreate(surface);
    gdContextArc(cr, 128.0, 128.0, 76.8, 0, 2*M_PI);
    gdContextClip(cr);
    gdContextNewPath(cr); /* path not consumed by clip()*/

    gdContextScale(cr, 256.0/w, 256.0/h);

    gdContextSetSourceSurface(cr, surface_fill, 0, 0);
    gdContextPaint(cr);


    save_png(surface, "path_arcfillimage.png");

    gdContextDestroy(cr);
    gdSurfaceDestroy(surface);
    gdSurfaceDestroy(surface_fill);
    return 0;
}
