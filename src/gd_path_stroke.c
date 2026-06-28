#include "gd.h"
#include "gdhelpers.h"
#include "gd_path.h"
#include "gd_path_matrix.h"
#include "ftraster/gd_ft_stroker.h"
#include "ftraster/gd_ft_raster.h"
#include <math.h>

extern GD_FT_Outline *gd_ft_outline_convert(const gdPathPtr path, const gdPathMatrixPtr matrix);
extern GD_FT_Outline *gd_ft_outline_create(int points, int contours);
extern void gd_ft_outline_destroy(GD_FT_Outline *ft);

static gdPathPtr ft_outline_to_gdpath(const GD_FT_Outline* outline)
{
    if (!outline || outline->n_points == 0)
        return NULL;

    gdPathPtr path = gdPathCreate();
    if (!path)
        return NULL;

    for (int contour = 0; contour < outline->n_contours; contour++)
    {
        int end_idx = outline->contours[contour];
        int start_idx = (contour == 0) ? 0 : outline->contours[contour - 1] + 1;

        if (end_idx < start_idx)
            continue;

        for (int i = start_idx; i <= end_idx; i++)
        {
            char tag = GD_FT_CURVE_TAG(outline->tags[i]);
            double x = outline->points[i].x / 64.0;
            double y = outline->points[i].y / 64.0;

            if (i == start_idx)
            {
                gdPathMoveTo(path, x, y);
            }
            else
            {
                if (tag == GD_FT_CURVE_TAG_ON)
                {
                    gdPathLineTo(path, x, y);
                }
                else if (tag == GD_FT_CURVE_TAG_CONIC)
                {
                    if (i + 1 <= end_idx)
                    {
                        i++;
                        double x2 = outline->points[i].x / 64.0;
                        double y2 = outline->points[i].y / 64.0;
                        gdPathQuadTo(path, x, y, x2, y2);
                    }
                }
                else if (tag == GD_FT_CURVE_TAG_CUBIC)
                {
                    if (i + 2 <= end_idx)
                    {
                        double x2 = outline->points[i + 1].x / 64.0;
                        double y2 = outline->points[i + 1].y / 64.0;
                        i++;
                        double x3 = outline->points[i + 1].x / 64.0;
                        double y3 = outline->points[i + 1].y / 64.0;
                        i++;
                        gdPathCurveTo(path, x, y, x2, y2, x3, y3);
                    }
                }
            }
        }
        gdPathClose(path);
    }

    return path;
}

BGD_DECLARE(gdPathPtr) gdPathStrokeToPath(const gdPathPtr path, const gdStrokePtr stroke, const gdPathMatrixPtr matrix)
{
    if (!path || !stroke || stroke->width <= 0)
        return NULL;

    GD_FT_Outline *outline = gd_ft_outline_convert(path, matrix);
    if (!outline)
        return NULL;

    GD_FT_Stroker stroker;
    GD_FT_Stroker_New(&stroker);

    double radius = stroke->width / 2.0;
    GD_FT_Fixed ftWidth = (GD_FT_Fixed)(radius * 64);
    GD_FT_Fixed ftMiterLimit = (GD_FT_Fixed)(stroke->miterlimit * 65536);

    GD_FT_Stroker_LineCap ftCap;
    switch (stroke->cap)
    {
        case gdLineCapSquare:
            ftCap = GD_FT_STROKER_LINECAP_SQUARE;
            break;
        case gdLineCapRound:
            ftCap = GD_FT_STROKER_LINECAP_ROUND;
            break;
        case gdLineCapButt:
        default:
            ftCap = GD_FT_STROKER_LINECAP_BUTT;
            break;
    }

    GD_FT_Stroker_LineJoin ftJoin;
    switch (stroke->join)
    {
        case gdLineJoinBevel:
            ftJoin = GD_FT_STROKER_LINEJOIN_BEVEL;
            break;
        case gdLineJoinRound:
            ftJoin = GD_FT_STROKER_LINEJOIN_ROUND;
            break;
        case gdLineJoinMiter:
        default:
            ftJoin = GD_FT_STROKER_LINEJOIN_MITER_FIXED;
            break;
    }

    GD_FT_Stroker_Set(stroker, ftWidth, ftCap, ftJoin, ftMiterLimit);

    GD_FT_Stroker_ParseOutline(stroker, outline);

    GD_FT_UInt points, contours;
    GD_FT_Stroker_GetCounts(stroker, &points, &contours);

    GD_FT_Outline *strokeOutline = gd_ft_outline_create((int)points, (int)contours);
    if (!strokeOutline)
    {
        GD_FT_Stroker_Done(stroker);
        gd_ft_outline_destroy(outline);
        return NULL;
    }

    // Use combined export (both borders + caps) - works for both open and closed paths
    GD_FT_Stroker_Export(stroker, strokeOutline);
    GD_FT_Stroker_Done(stroker);

    gdPathPtr strokePath = ft_outline_to_gdpath(strokeOutline);

    gd_ft_outline_destroy(outline);
    gd_ft_outline_destroy(strokeOutline);

    if (!strokePath)
    {
        return NULL;
    }

    return strokePath;
}
