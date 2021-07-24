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
    int width = 512, height = 512;
    int w,h;
    gdSurface *surface;
    unsigned char *data;
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

    gdContextPtr cr = gdContextCreate(surface);
    gdContextTranslate(cr, width/2, height/2);
    gdContextRotate(cr, 45* M_PI/180);
    gdContextScale(cr, width/w, height/h);
    gdContextTranslate(cr, -0.5*w, -0.5*h);
    gdContextSetSourceSurface(cr, surface_fill, 0, 0);
    gdContextPaint(cr);

    save_png(surface, "path_imagesource.png");

    gdContextDestroy(cr);
    gdSurfaceDestroy(surface);
    gdSurfaceDestroy(surface_fill);
    return 0;
}
