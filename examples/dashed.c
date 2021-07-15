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

typedef struct
{
  double x, y;
} point;

/* The control points (0 and 1) and end point (2) of a cubic Bezier
   curve. */

typedef struct
{
  point pt[3];
} curve;

int lerp(point a, point b, double t, point *res)
{
  const double s = 1 - t;
  res->x = a.x * s + b.x * t;
  res->y = a.y * s + b.y * t;
  return 1;
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

  point pts[] = {{.x = 20, .y = 20},
               {.x = 20, .y = 100},
               {.x = 200, .y = 100},
               {.x = 200, .y = 20}};
  point p0 = pts[0], p1 = pts[1], p2 = pts[2], p3 = pts[3];
  double division_factor = 0.5;

  point p4, p5, p6, p7, p8, p9;
  lerp(p0, p1, division_factor, &p4);
  lerp(p1, p2, division_factor, &p5);
  lerp(p2, p3, division_factor, &p6);
  lerp(p4, p5, division_factor, &p7);
  lerp(p5, p6, division_factor, &p8);
  lerp(p7, p8, division_factor, &p9);

  gdContextSetLineWidth(cr, 15);
  gdContextSetSourceRgba(cr, 1, 0.0, 0.0, 0.5);
  gdContextMoveTo(cr,p0.x,p0.y);
  gdContextCurveTo(cr, p4.x,p4.y, p7.x,p7.y, p9.x,p9.y);
  gdContextStroke(cr);

  gdContextSetLineWidth(cr,15);
  gdContextSetSourceRgba(cr, 0, 1.0, 0.0,0.5);
  gdContextMoveTo(cr, p9.x,p9.y);
  gdContextCurveTo(cr, p8.x,p8.y,p6.x,p6.y,p3.x,p3.y);
  gdContextStroke(cr);

  save_png(surface, "dashed.png");
  gdContextDestroy(cr);
  gdSurfaceDestroy(surface);
  return 0;
}
