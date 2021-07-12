
#include <stdio.h>
#include <math.h>
#include <string.h>
#include <stdlib.h>
#include <stdarg.h>

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif
#include "gd.h"
#include "gd_intern.h"
#include "gdhelpers.h"
#include "gd_errors.h"

#include "gd_surface.h"
#include "gd_array.h"
#include "gd_path_matrix.h"
#include "gd_span_rle.h"
#include "gd_path.h"
#include "gd_path_arc.h"

gdPaintPtr gdPaintAddRef(gdPaintPtr paint)
{
    if (paint == NULL)
        return NULL;

    ++paint->ref;
    return paint;
}

void gdPaintDestroy(gdPaintPtr paint)
{
    if (paint == NULL)
        return;
    paint->ref--;

    if (paint->ref == 0)
    {
        switch (paint->type)
        {
        case gdPaintTypeColor:
            gdFree(paint->color);
            break;
        case gdPaintTypeGradient:
            // Not implemented
            break;
        case gdPaintTypeSurface:
            // Not implemented
            break;
        default:
        {
        }
        }
        gdFree(paint);
    }
}

void gdColorInitRgba(gdColorPtr color, double r, double g, double b, double a)
{
    color->r = CLAMP(r, 0.0, 1.0);
    color->g = CLAMP(g, 0.0, 1.0);
    color->b = CLAMP(b, 0.0, 1.0);
    color->a = CLAMP(a, 0.0, 1.0);
}

gdPaintPtr gdPaintCreateRgba(double r, double g, double b, double a)
{
    gdPaintPtr paint = gdMalloc(sizeof(gdPaint));
    if (!paint)
    {
        return NULL;
    }
    paint->color = gdMalloc(sizeof(gdColor));
    if (!paint->color)
    {
        gdFree(paint);
        return NULL;
    }

    paint->ref = 1;
    paint->type = gdPaintTypeColor;
    gdColorInitRgba(paint->color, r, g, b, a);
    return paint;
}

gdPaintPtr gdPaintCreateRgb(double r, double g, double b)
{
    return gdPaintCreateRgba(r, g, b, 1.0);
}

void gdContextSetSource(gdContextPtr context, gdPaintPtr source)
{
    source = gdPaintAddRef(source);
    gdPaintDestroy(context->state->source);
    context->state->source = source;
}

void gdContextSetSourceColorRgb(gdContextPtr context, double r, double g, double b)
{
    gdContextSetSourceRgba(context, r, g, b, 1.0);
}

gdStatePtr gdStateCreate()
{
    gdStatePtr state = gdMalloc(sizeof(gdState));
    if (!state)
    {
        return NULL;
    }
    //state->font = NULL;
    state->source = gdPaintCreateRgba(0, 0, 0, 1.0);
    if (!state->source)
    {
        gdFree(state);
        return NULL;
    }
    gdPathMatrixInitIdentity(&state->matrix);
    state->winding = gdFillRuleNonZero;
    state->stroke.width = 1.0;
    state->stroke.miterlimit = 4.0;
    state->stroke.cap = gdLineCapButt;
    state->stroke.join = gdLineJoinMiter;
    //state->stroke.dash = NULL;
    state->op = gdImageOpsSrcOver;
    //state->fontsize = 12.0;
    state->opacity = 1.0;
    state->clippath = NULL;
    state->next = NULL;
    return state;
}

void gdStateDestroy(gdStatePtr state)
{
    //state->font
    gdSpanRleDestroy(state->clippath);
    gdPaintDestroy(state->source);
    //(state->stroke.dash);
    gdFree(state);
}

gdPathPtr gdPathCreate()
{
    gdPathPtr path = gdMalloc(sizeof(gdPath));
    if (!path)
        return NULL;
    path->ref = 1;
    path->contours = 0;
    path->start.x = 0.0;
    path->start.y = 0.0;
    gdArrayInit(&path->elements, sizeof(gdPathOps));
    gdArrayInit(&path->points, sizeof(gdPointF));
    return path;
}

gdPathPtr gdPathAddRef(gdPathPtr path)
{
    if (path == NULL)
        return NULL;
    path->ref++;
    return path;
}

void gdPathDestroy(gdPathPtr path)
{
    if (path == NULL)
        return;
    path->ref--;
    if (path->ref == 0)
    {
        gdArrayDestroy(&path->elements);
        gdArrayDestroy(&path->points);
        gdFree(path);
    }
}

void gdPathClear(gdPathPtr path)
{
    gdArrayTruncate(&path->elements, 0);
    gdArrayTruncate(&path->points, 0);
    path->contours = 0;
    path->start.x = 0.0;
    path->start.y = 0.0;
}

/* dump a path, could be nicer but good enough for now
Not exported, only for debugging purposes here */
void gdPathDumpPathTransform(const gdPathPtr path, const gdPathMatrixPtr matrix)
{
    //GD_FT_Outline* outline = gd_ft_outline_create(path->points.size, path->contours);
    gdPointF p[3];
    unsigned int numElements = gdArrayNumElements(&path->elements);
    unsigned int pointsIndex = 0;
    unsigned int i = 0;

    memset(p, 0, sizeof(gdPointF) * 3);
    printf("NEWOUTLINE CONVERT\n");
    for (i = 0; i < numElements; i++)
    {
        gdPathOpsPtr element = (gdPathOpsPtr)gdArrayIndex(&path->elements, i);
        gdPointFPtr point = gdArrayIndex(&path->points, pointsIndex);
        printf("-------\n");
        switch (*element)
        {
        case gdPathOpsMoveTo:
            gdPathMatrixMapPoint(matrix, point, &p[0]);
            printf("MoveTo(%f, %f)", point->x, point->y);
            printf("(%f, %f)", p[0].x, p[0].y);
            pointsIndex += 1;
            break;
        case gdPathOpsLineTo:
            gdPathMatrixMapPoint(matrix, point, &p[0]);
            printf("LineTo(%f, %f)", point->x, point->y);
            printf("(%f, %f)", p[0].x, p[0].y);
            pointsIndex += 1;
            break;
        case gdPathOpsCubicTo:
            printf("CubicTo(%f, %f)", point->x, point->y);
            gdPathMatrixMapPoint(matrix, point, &p[0]);
            printf("(%f, %f)", p[0].x, p[0].y);

            point = gdArrayIndex(&path->points, pointsIndex + 1);
            gdPathMatrixMapPoint(matrix, point, &p[1]);
            printf("(%f, %f)", p[1].x, p[1].y);

            point = gdArrayIndex(&path->points, pointsIndex + 2);
            gdPathMatrixMapPoint(matrix, point, &p[2]);
            printf("(%f, %f)", p[2].x, p[2].y);
            pointsIndex += 3;
            break;
        case gdPathOpsClose:
            printf("Outline close");
            pointsIndex += 1;
            break;
        }
        printf("\n-------\n");
    }
}

static inline void _get_current_point(const gdPathPtr path, double *x, double *y)
{
    unsigned int numElems = gdArrayNumElements(&path->points);
    if (numElems < 1)
        return;
    if (x)
        *x = 0.0;
    if (y)
        *y = 0.0;
    gdPointFPtr point = gdArrayIndex(&path->points, numElems - 1);
    if (x)
    {
        *x = point->x;
    }
    if (y)
    {
        *y = point->y;
    }
}

static inline void _relativeTo(const gdPathPtr path, double *x, double *y)
{
    double _x = -1, _y = -1;
    _get_current_point(path, &_x, &_y);
    *x += _x;
    *y += _y;
}

void gdPathMoveTo(gdPathPtr path, double x, double y)
{
    const gdPathOps op = gdPathOpsMoveTo;
    gdPointF point;

    gdArrayAppend(&path->elements, &op);
    path->contours += 1;

    point.x = x;
    point.y = y;
    gdArrayAppend(&path->points, &point);
    path->start.x = x;
    path->start.y = y;
}

void gdPathLineTo(gdPathPtr path, double x, double y)
{
    const gdPathOps op = gdPathOpsLineTo;
    gdPointF point;

    gdArrayAppend(&path->elements, &op);

    point.x = x;
    point.y = y;
    gdArrayAppend(&path->points, &point);
}
void gdPathRelLineTo(gdPathPtr path, double dx, double dy)
{
    _relativeTo(path, &dx, &dy);
    gdPathLineTo(path, dx, dy);
}

void gdPathCurveTo(gdPathPtr path, double x1, double y1, double x2, double y2, double x3, double y3)
{
    const gdPathOps op = gdPathOpsCubicTo;
    gdPointF points[3];

    gdArrayAppend(&path->elements, &op);
    points[0].x = x1;
    points[0].y = y1;
    points[1].x = x2;
    points[1].y = y2;
    points[2].x = x3;
    points[2].y = y3;
    gdArrayAppend(&path->points, &points[0]);
    gdArrayAppend(&path->points, &points[1]);
    gdArrayAppend(&path->points, &points[2]);
}

/*
Based on http://www.whizkidtech.redprince.net/bezier/circle/kappa/
*/
void gdPathAddArc(gdPathPtr path, double cx, double cy, double radius, double angle1, double angle2, int ccw)
{
    if (ccw)
        _gd_arc_path_negative(path, cx,cy, radius, angle1, angle2);
    else
        _gd_arc_path(path, cx,cy, radius, angle1, angle2);
}

void gdPathArcTo(gdPathPtr path, double x1, double y1, double x2, double y2, double radius)
{
    double x0, y0;
    _get_current_point(path, &x0, &y0);
    if ((x0 == x1 && y0 == y1) || (x1 == x2 && y1 == y2) || radius == 0.0)
    {
        gdPathLineTo(path, x1, y2);
        return;
    }

    double dir = (x2 - x1) * (y0 - y1) + (y2 - y1) * (x1 - x0);
    if (dir == 0.0)
    {
        gdPathLineTo(path, x1, y2);
        return;
    }

    double a2 = (x0 - x1) * (x0 - x1) + (y0 - y1) * (y0 - y1);
    double b2 = (x1 - x2) * (x1 - x2) + (y1 - y2) * (y1 - y2);
    double c2 = (x0 - x2) * (x0 - x2) + (y0 - y2) * (y0 - y2);

    double cosx = (a2 + b2 - c2) / (2 * sqrt(a2 * b2));
    double sinx = sqrt(1 - cosx * cosx);
    double d = radius / ((1 - cosx) / sinx);

    double anx = (x1 - x0) / sqrt(a2);
    double any = (y1 - y0) / sqrt(a2);
    double bnx = (x1 - x2) / sqrt(b2);
    double bny = (y1 - y2) / sqrt(b2);

    double x3 = x1 - anx * d;
    double y3 = y1 - any * d;
    double x4 = x1 - bnx * d;
    double y4 = y1 - bny * d;

    int ccw = dir < 0.0;
    double cx = x3 + any * radius * (ccw ? 1 : -1);
    double cy = y3 - anx * radius * (ccw ? 1 : -1);
    double a0 = atan2(y3 - cy, x3 - cx);
    double a1 = atan2(y4 - cy, x4 - cx);

    gdPathLineTo(path, x3, y3);
    gdPathAddArc(path, cx, cy, radius, a0, a1, ccw);
}

void gdPathClose(gdPathPtr path)
{
    const int numElements = gdArrayNumElements(&path->elements);
    const int numPoints = gdArrayNumElements(&path->points);
    const gdPathOpsPtr lastOpPtr = gdArrayIndex(&path->points, numPoints - 1);
    const gdPathOps OpClose = gdPathOpsClose;
    if (numElements == 0)
        return;
    if (*lastOpPtr == gdPathOpsClose)
        return;

    gdPointF point;
    point.x = path->start.x;
    point.y = path->start.y;
    gdArrayAppend(&path->elements, &OpClose);
    gdArrayAppend(&path->points, &point);
}

void gdPathBlend(gdContextPtr context, const gdSpanRlePtr rle);



