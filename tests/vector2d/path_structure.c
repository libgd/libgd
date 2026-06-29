#include "gd_vector2d_private.h"
#include "gd_array.h"
#include "gd_path.h"
#include "gd_path_dash.h"
#include "gd_path_matrix.h"
#include "gdtest.h"

#include <math.h>

static gdPathOps op_at(gdPathPtr path, unsigned int index)
{
    return *(gdPathOpsPtr)gdArrayIndex(&path->elements, index);
}

static gdPointF point_at(gdPathPtr path, unsigned int index)
{
    return *(gdPointFPtr)gdArrayIndex(&path->points, index);
}

static void assert_point(gdPathPtr path, unsigned int index, double x, double y)
{
    gdPointF point = point_at(path, index);
    gdTestAssert(fabs(point.x - x) < 1e-12);
    gdTestAssert(fabs(point.y - y) < 1e-12);
}

static void test_path_representation(void)
{
    gdPathPtr path = gdPathCreate();
    gdPathClose(path);
    gdTestAssert(gdArrayNumElements(&path->elements) == 0);
    gdTestAssert(gdArrayNumElements(&path->points) == 0);

    gdPathMoveTo(path, 1, 2);
    gdPathLineTo(path, 3, 4);
    gdPathQuadTo(path, 5, 6, 7, 8);
    gdPathCurveTo(path, 9, 10, 11, 12, 13, 14);
    gdPathClose(path);
    gdPathClose(path);

    gdTestAssert(gdArrayNumElements(&path->elements) == 5);
    gdTestAssert(gdArrayNumElements(&path->points) == 8);
    gdTestAssert(path->contours == 1);
    gdTestAssert(op_at(path, 0) == gdPathOpsMoveTo);
    gdTestAssert(op_at(path, 1) == gdPathOpsLineTo);
    gdTestAssert(op_at(path, 2) == gdPathOpsQuadTo);
    gdTestAssert(op_at(path, 3) == gdPathOpsCubicTo);
    gdTestAssert(op_at(path, 4) == gdPathOpsClose);
    assert_point(path, 0, 1, 2);
    assert_point(path, 1, 3, 4);
    assert_point(path, 2, 5, 6);
    assert_point(path, 3, 7, 8);
    assert_point(path, 4, 9, 10);
    assert_point(path, 5, 11, 12);
    assert_point(path, 6, 13, 14);
    assert_point(path, 7, 1, 2);

    gdPathMoveTo(path, -2, -3);
    gdPathLineTo(path, -4, -5);
    gdTestAssert(path->contours == 2);
    gdTestAssert(gdArrayNumElements(&path->elements) == 7);
    gdTestAssert(gdArrayNumElements(&path->points) == 10);
    gdPathDestroy(path);
}

static void test_dash_structure(void)
{
    const double values[] = {10, 5};
    gdPathDashPtr dash = gdPathDashCreate(values, 2, 0);
    gdPathPtr path = gdPathCreate();
    gdPathMoveTo(path, 0, 0);
    gdPathLineTo(path, 30, 0);

    gdPathPtr dashed = gdPathApplyDash(dash, path);
    gdTestAssert(dashed != NULL);
    gdTestAssert(gdArrayNumElements(&dashed->elements) == 4);
    gdTestAssert(op_at(dashed, 0) == gdPathOpsMoveTo);
    gdTestAssert(op_at(dashed, 1) == gdPathOpsLineTo);
    gdTestAssert(op_at(dashed, 2) == gdPathOpsMoveTo);
    gdTestAssert(op_at(dashed, 3) == gdPathOpsLineTo);
    assert_point(dashed, 0, 0, 0);
    assert_point(dashed, 1, 10, 0);
    assert_point(dashed, 2, 15, 0);
    assert_point(dashed, 3, 25, 0);

    gdPathDestroy(dashed);
    gdPathDestroy(path);
    gdPathDashDestroy(dash);
    gdTestAssert(gdPathDashCreate(NULL, 0, 0) == NULL);
}

static void path_bounds(gdPathPtr path, double *min_x, double *min_y,
                        double *max_x, double *max_y)
{
    unsigned int count = gdArrayNumElements(&path->points);
    gdTestAssert(count > 0);
    gdPointF point = point_at(path, 0);
    *min_x = *max_x = point.x;
    *min_y = *max_y = point.y;
    for (unsigned int i = 1; i < count; i++) {
        point = point_at(path, i);
        gdTestAssert(isfinite(point.x) && isfinite(point.y));
        if (point.x < *min_x) *min_x = point.x;
        if (point.x > *max_x) *max_x = point.x;
        if (point.y < *min_y) *min_y = point.y;
        if (point.y > *max_y) *max_y = point.y;
    }
}

static void test_stroke_caps_and_matrix(void)
{
    gdPathPtr path = gdPathCreate();
    gdPathMoveTo(path, 20, 20);
    gdPathLineTo(path, 20, 40);

    gdStroke stroke = {30, 4, gdLineCapRound, gdLineJoinRound, NULL};
    gdPathMatrix matrix;
    gdPathMatrixInitIdentity(&matrix);
    gdPathPtr round = gdPathStrokeToPath(path, &stroke, &matrix);
    gdTestAssert(round != NULL);

    double min_x, min_y, max_x, max_y;
    path_bounds(round, &min_x, &min_y, &max_x, &max_y);
    gdTestAssert(fabs(min_x - 5.0) <= 1.0 / 64.0);
    gdTestAssert(fabs(max_x - 35.0) <= 1.0 / 64.0);
    gdTestAssert(fabs(min_y - 5.0) <= 1.0 / 64.0);
    gdTestAssert(fabs(max_y - 55.0) <= 1.0 / 64.0);
    gdTestAssert(fabs((min_x + max_x) / 2.0 - 20.0) <= 1.0 / 64.0);

    gdPathDestroy(round);
    for (int cap = gdLineCapButt; cap <= gdLineCapSquare; cap++) {
        for (int join = gdLineJoinMiter; join <= gdLineJoinBevel; join++) {
            stroke.cap = (gdLineCap)cap;
            stroke.join = (gdLineJoin)join;
            gdPathMatrixInit(&matrix, 2, 0, 0, 1.5, 7, -3);
            gdPathPtr stroked = gdPathStrokeToPath(path, &stroke, &matrix);
            gdTestAssert(stroked != NULL);
            gdTestAssert(gdArrayNumElements(&stroked->elements) > 0);
            path_bounds(stroked, &min_x, &min_y, &max_x, &max_y);
            gdPathDestroy(stroked);
        }
    }
    gdPathDestroy(path);
}

int main(void)
{
    test_path_representation();
    test_dash_structure();
    test_stroke_caps_and_matrix();
    return gdNumFailures();
}
