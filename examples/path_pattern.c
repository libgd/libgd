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
    gdSurfacePtr surface_fill = read_png("../examples/images/isar_256x.png");
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

    gdPathPatternPtr pattern = gdPathPatternCreate(surface_fill);
    gdPathPatternSetExtend(pattern, GD_EXTEND_REPEAT);

    gdContextTranslate(cr, width/2, height/2);
    gdContextRotate(cr, M_PI / 4);
    gdContextScale (cr, 1 / sqrt (2), 1 / sqrt (2));
    gdContextTranslate (cr, -width/2, -height/2);

    gdPathMatrix matrix;
    gdPathPatternSetMatrix(pattern, &matrix);
    gdPathMatrixInitScale (&matrix, 1/(w/(double)width * 5.0), 1/(h/(double)height * 5.0));
    gdPathPatternSetMatrix (pattern, &matrix);

    gdPaintPtr paint = gdPaintCreateFromPattern(pattern);
    gdContextSetSource(cr, paint);

    gdContextMoveTo (cr, 0, 0);
    gdContextLineTo(cr, 0, (double)height);
    gdContextLineTo(cr, (double)height, (double)height);
    gdContextLineTo(cr, (double)width, 0);
    gdContextFill(cr);

    save_png(surface, "path_pattern.png");

    gdPathPatternDestroy(pattern);
    gdPaintDestroy(paint);
    gdContextDestroy(cr);
    gdSurfaceDestroy(surface);
    gdSurfaceDestroy(surface_fill);
    return 0;
}
