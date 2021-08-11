#include <stdio.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>

#include <ft2build.h>
#include FT_FREETYPE_H
#include FT_OUTLINE_H
#include FT_BBOX_H
#define FT2POS_TO_DOUBLE(num) ((double)(num) / 64.0)

#include "gd.h"
#include "math.h"
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

int MoveToFunction(const FT_Vector *to, void *user)
{
    gdContextPtr cr = (gdContextPtr)user;

    FT_Pos x = to->x;
    FT_Pos y = to->y;
    gdContextMoveTo(cr, FT2POS_TO_DOUBLE(x), FT2POS_TO_DOUBLE(y));
    printf("move_to(%f, %f)\n", FT2POS_TO_DOUBLE(x), FT2POS_TO_DOUBLE(y));
    return 0;
}
int LineToFunction(const FT_Vector *to, void *user)
{
    gdContextPtr cr = (gdContextPtr)user;
    FT_Pos x = to->x;
    FT_Pos y = to->y;
    gdContextLineTo(cr, FT2POS_TO_DOUBLE(x), FT2POS_TO_DOUBLE(y));
    printf("line_to(%f, %f)\n", FT2POS_TO_DOUBLE(x), FT2POS_TO_DOUBLE(y));
    return 0;
}

int CubicToFunction(const FT_Vector *cp1, const FT_Vector *cp2, const FT_Vector *to, void *user)
{
    gdContextPtr cr = (gdContextPtr)user;
    FT_Pos x = to->x;
    FT_Pos y = to->y;
    gdContextCurveTo(cr,
                     FT2POS_TO_DOUBLE(cp1->x), FT2POS_TO_DOUBLE(cp1->y),
                     FT2POS_TO_DOUBLE(cp2->x), FT2POS_TO_DOUBLE(cp2->y),
                     FT2POS_TO_DOUBLE(to->x), FT2POS_TO_DOUBLE(to->y)
    );
    printf("cubic_to(%f, %f, %f, %f, %f, %f)\n",
           FT2POS_TO_DOUBLE(cp1->x), FT2POS_TO_DOUBLE(cp1->y),
           FT2POS_TO_DOUBLE(cp2->x), FT2POS_TO_DOUBLE(cp2->y),
           FT2POS_TO_DOUBLE(to->x), FT2POS_TO_DOUBLE(to->y));
    return 0;
}

int ConicToFunction(const FT_Vector *cp1, const FT_Vector *to, void *user)
{
    gdContextPtr cr = (gdContextPtr)user;
    FT_Pos x = to->x;
    FT_Pos y = to->y;
    gdContextQuadTo(cr,
             FT2POS_TO_DOUBLE(cp1->x), FT2POS_TO_DOUBLE(cp1->y),
             FT2POS_TO_DOUBLE(to->x), FT2POS_TO_DOUBLE(to->y)
             );
    printf("conic_to(%f, %f, %f, %f)\n",
           FT2POS_TO_DOUBLE(cp1->x), FT2POS_TO_DOUBLE(cp1->y),
           FT2POS_TO_DOUBLE(to->x), FT2POS_TO_DOUBLE(to->y));
    return 0;
}
const FT_Fixed multiplier = 65536L;
int FlipOutline(FT_Face face)
{
    const FT_Fixed multiplier = 65536L;
    FT_Matrix matrix;
    matrix.xx = 1L * multiplier;
    matrix.xy = 0L * multiplier;
    matrix.yx = 0L * multiplier;
    matrix.yy = -1L * multiplier;
    FT_GlyphSlot slot = face->glyph;
    FT_Outline outline = slot->outline;
    FT_Outline_Transform(&outline, &matrix);

    return 1;
}

/*
 xx = m00
 yx = m10
 xy = m01
 yy = m11
 x0 = m02
 y0 = m12
*/

int main(int argc,
         char **argv)
{
    if (argc != 2)
    {
        const char *program = argv[0];
        printf("%s <char>\n", program);
        return 1;
    }

    const char *symbol = argv[1];
    // For simplicity, only accept single-byte characters like `A'.
    if (strlen(symbol) != 1 || *symbol == 20)
    {
        printf("Invalid character <%i>\n", *symbol);
        return 2;
    }

    int status = 0;

    FT_Library library;
    FT_Face face;
    FT_Error error = FT_Init_FreeType(&library);
    if (error)
    {
        printf("Couldn't initialize the library: FT_Init_FreeType() failed.\n");
        exit(1);
    }
    error = FT_New_Face(library, "/home/pierre/projects/libgd/libgd/build/EBGaramond08-Regular.ttf", 0, &face);
    if (error)
    {
        printf("Couldn't load the TTF font: FT_New_Face()    failed.\n");
        exit(1);
    }

#define METRIC_RES 96
    //FT_Set_Char_Size (face, 0, (FT_F26Dot6)(100*64), METRIC_RES, METRIC_RES);
    FT_Set_Pixel_Sizes(face, 0, 100);
    FT_Matrix matrix;
    FT_Vector delta;
    gdPathMatrixPtr scale_matrix;

    matrix.xx = 1L * multiplier;;
    matrix.xy = 0L * multiplier;
    matrix.yx = 0L * multiplier;
    matrix.yy = -1L * multiplier;
    delta.x = 0;
    //delta.x = 0;
    delta.y = 200*64;
/*    delta.x = 0;
    delta.y = 0;*/
    FT_Set_Transform(face,&matrix,&delta);

    //error = FT_Set_Pixel_Sizes(face, 0, 100);
    FT_ULong code = symbol[0];
    FT_UInt index = FT_Get_Char_Index(face, code);

/*    error = FT_Load_Glyph(face,
                          index,
                          FT_LOAD_NO_SCALE | FT_LOAD_NO_BITMAP);*/
    error = FT_Load_Glyph(face,
                          index,
                           FT_LOAD_NO_BITMAP);
    gdSurface *surface;
    int width = 800;
    int height = 800;
    surface = gdSurfaceCreate(width, height, GD_SURFACE_ARGB32);
    if (!surface)
    {
        fprintf(stderr, "Can't create 400x400 surface\n");
        return 1;
    }
    gdContextPtr cr = gdContextCreate(surface);

    gdContextMoveTo(cr, 900, 900);
    FT_Outline_Funcs callbacks;
    callbacks.move_to = MoveToFunction;
    callbacks.line_to = LineToFunction;
    callbacks.conic_to = ConicToFunction;
    callbacks.cubic_to = CubicToFunction;
/*
    callbacks.shift = 100 << 64;
    callbacks.delta = 500 * 64;
*/
    callbacks.shift = 0;
    callbacks.delta = 0;

    FT_GlyphSlot glyph = face->glyph;
    FT_Outline outline = glyph->outline;

    error = FT_Outline_Decompose(&outline, &callbacks, cr);

    //  free face
    FT_Done_Face(face);
    face = NULL;

    //  free FreeType Lib
    FT_Done_FreeType(library);

    gdSurfacePtr surface_fill = read_png("../examples/images/isar_512x.png");
    if (!surface_fill) {
        fprintf(stderr,  "can't read image\n");
        return 1;
    }
    int w = gdSurfaceGetWidth(surface_fill);
    int h = gdSurfaceGetHeight(surface_fill);

    gdPathPatternPtr pattern = gdPathPatternCreate(surface_fill);
    gdPathPatternSetExtend(pattern, GD_EXTEND_REPEAT);

    gdPathMatrix pattern_matrix;
    gdPathPatternSetMatrix(pattern, &pattern_matrix);
    gdPathMatrixInitScale (&pattern_matrix, 1, 1);
    gdPathPatternSetMatrix (pattern, &pattern_matrix);
    gdPaintPtr paint = gdPaintCreateFromPattern(pattern);
    gdContextSetSource(cr, paint);

    gdContextScale(cr, 1, 1 );

    gdContextFillPreserve(cr);
    gdContextSetSourceRgba(cr, 1,1,1,1);
    gdContextSetLineWidth(cr, 0.2);

    gdContextStroke(cr);
    save_png(surface, "ft.png");
    gdContextDestroy(cr);
    gdSurfaceDestroy(surface);
    return status;
}
