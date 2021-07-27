#include <stdio.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>

#include <ft2build.h>
#include FT_FREETYPE_H
#include FT_OUTLINE_H
#include FT_BBOX_H
#include FT_GLYPH_H
#include FT_SIZES_H
#define FT2POS_TO_DOUBLE(num) ((double)(num) / 64.0)

#include "gd.h"
#include "math.h"
#include "entities.h"
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

int _move_to(const FT_Vector *to, void *user)
{
    gdPathPtr path = (gdPathPtr)user;

    gdPathMoveTo(path, FT2POS_TO_DOUBLE(to->x), FT2POS_TO_DOUBLE(to->y));
/*    printf("move_to(%f, %f)\n", FT2POS_TO_DOUBLE(x), FT2POS_TO_DOUBLE(y));*/
    return 0;
}
int _line_to(const FT_Vector *to, void *user)
{
    gdPathPtr path = (gdPathPtr)user;

    gdPathLineTo(path, FT2POS_TO_DOUBLE(to->x), FT2POS_TO_DOUBLE(to->y));
    /*printf("line_to(%f, %f)\n", FT2POS_TO_DOUBLE(x), FT2POS_TO_DOUBLE(y));*/
    return 0;
}

int _cubic_to(const FT_Vector *cp1, const FT_Vector *cp2, const FT_Vector *to, void *user)
{
    gdPathPtr path = (gdPathPtr)user;

    gdPathCurveTo(path,
                     FT2POS_TO_DOUBLE(cp1->x), FT2POS_TO_DOUBLE(cp1->y),
                     FT2POS_TO_DOUBLE(cp2->x), FT2POS_TO_DOUBLE(cp2->y),
                     FT2POS_TO_DOUBLE(to->x), FT2POS_TO_DOUBLE(to->y)
    );
/*    printf("cubic_to(%f, %f, %f, %f, %f, %f)\n",
           FT2POS_TO_DOUBLE(cp1->x), FT2POS_TO_DOUBLE(cp1->y),
           FT2POS_TO_DOUBLE(cp2->x), FT2POS_TO_DOUBLE(cp2->y),
           FT2POS_TO_DOUBLE(to->x), FT2POS_TO_DOUBLE(to->y));*/
    return 0;
}

int _conic_to(const FT_Vector *cp1, const FT_Vector *to, void *user)
{
    gdPathPtr path = (gdPathPtr)user;

    gdPathQuadTo(path,
             FT2POS_TO_DOUBLE(cp1->x), FT2POS_TO_DOUBLE(cp1->y),
             FT2POS_TO_DOUBLE(to->x), FT2POS_TO_DOUBLE(to->y)
             );
/*    printf("conic_to(%f, %f, %f, %f)\n",
           FT2POS_TO_DOUBLE(cp1->x), FT2POS_TO_DOUBLE(cp1->y),
           FT2POS_TO_DOUBLE(to->x), FT2POS_TO_DOUBLE(to->y));*/
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

#define ENTITY_NAME_LENGTH_MAX 8
#define Tcl_UniChar int
#define TCL_UTF_MAX 3
static int comp_entities(const void *e1, const void *e2)
{
    struct entities_s *en1 = (struct entities_s *) e1;
    struct entities_s *en2 = (struct entities_s *) e2;
    return strcmp(en1->name, en2->name);
}
static int
gdTcl_UtfToUniChar (const char *str, Tcl_UniChar * chPtr)
/* str is the UTF8 next character pointer */
/* chPtr is the int for the result */
{
    int byte;
    char entity_name_buf[ENTITY_NAME_LENGTH_MAX+1];
    char *p;
    struct entities_s key, *res;

    /* HTML4.0 entities in decimal form, e.g. &#197; */
    /*           or in hexadecimal form, e.g. &#x6C34; */
    byte = *((unsigned char *) str);
    if (byte == '&') {
        int i, n = 0;

        byte = *((unsigned char *) (str + 1));
        if (byte == '#') {
            byte = *((unsigned char *) (str + 2));
            if (byte == 'x' || byte == 'X') {
                for (i = 3; i < 8; i++) {
                    byte = *((unsigned char *) (str + i));
                    if (byte >= 'A' && byte <= 'F')
                        byte = byte - 'A' + 10;
                    else if (byte >= 'a' && byte <= 'f')
                        byte = byte - 'a' + 10;
                    else if (byte >= '0' && byte <= '9')
                        byte = byte - '0';
                    else
                        break;
                    n = (n * 16) + byte;
                }
            } else {
                for (i = 2; i < 8; i++) {
                    byte = *((unsigned char *) (str + i));
                    if (byte >= '0' && byte <= '9')
                        n = (n * 10) + (byte - '0');
                    else
                        break;
                }
            }
            if (byte == ';') {
                *chPtr = (Tcl_UniChar) n;
                return ++i;
            }
        } else {
            key.name = p = entity_name_buf;
            for (i = 1; i <= 1 + ENTITY_NAME_LENGTH_MAX; i++) {
                byte = *((unsigned char *) (str + i));
                if (byte == '\0')
                    break;
                if (byte == ';') {
                    *p++ = '\0';
                    res = bsearch(&key, entities, NR_OF_ENTITIES,
                                  sizeof(entities[0]), *comp_entities);
                    if (res) {
                        *chPtr = (Tcl_UniChar) res->value;
                        return ++i;
                    }
                    break;
                }
                *p++ = byte;
            }
        }
    }

    /*
     * Unroll 1 to 3 byte UTF-8 sequences, use loop to handle longer ones.
     */

    byte = *((unsigned char *) str);

    if (byte < 0xC0) {
        /*
         * Handles properly formed UTF-8 characters between
         * 0x01 and 0x7F.  Also treats \0 and naked trail
         * bytes 0x80 to 0xBF as valid characters representing
         * themselves.
         */

        *chPtr = (Tcl_UniChar) byte;
        return 1;
    } else if (byte < 0xE0) {
        if ((str[1] & 0xC0) == 0x80) {
            /*
             * Two-byte-character lead-byte followed
             * by a trail-byte.
             */

            *chPtr = (Tcl_UniChar) (((byte & 0x1F) << 6) | (str[1] & 0x3F));
            return 2;
        }
        /*
         * A two-byte-character lead-byte not followed by trail-byte
         * represents itself.
         */

        *chPtr = (Tcl_UniChar) byte;
        return 1;
    } else if (byte < 0xF0) {
        if (((str[1] & 0xC0) == 0x80) && ((str[2] & 0xC0) == 0x80)) {
            /*
             * Three-byte-character lead byte followed by
             * two trail bytes.
             */

            *chPtr = (Tcl_UniChar) (((byte & 0x0F) << 12)
                                    | ((str[1] & 0x3F) << 6) | (str[2] & 0x3F));
            return 3;
        }
        /*
         * A three-byte-character lead-byte not followed by
         * two trail-bytes represents itself.
         */

        *chPtr = (Tcl_UniChar) byte;
        return 1;
    }
#if TCL_UTF_MAX > 3
    else {
			int ch, total, trail;

			total = totalBytes[byte];
			trail = total - 1;
			if (trail > 0) {
				ch = byte & (0x3F >> trail);
				do {
					str++;
					if ((*str & 0xC0) != 0x80) {
						*chPtr = byte;
						return 1;
					}
					ch <<= 6;
					ch |= (*str & 0x3F);
					trail--;
				} while (trail > 0);
				*chPtr = ch;
				return total;
			}
		}
#endif

    *chPtr = (Tcl_UniChar) byte;
    return 1;
}

typedef struct {
    unsigned int index;
    FT_Pos x_advance;
    FT_Pos x_offset;
    FT_Pos y_offset;
    uint32_t cluster;
} glyphInfo;

static ssize_t
textLayout(uint32_t *text, unsigned int len,
           FT_Face face,
           glyphInfo **glyph_info)
{
    size_t count;
    glyphInfo *info;

    if (!len) {
        return 0;
    }

    FT_UInt glyph_index = 0;
    FT_Vector delta;
    FT_Error err;
    info = (glyphInfo*) malloc(sizeof (glyphInfo) * len);
    if (!info) {
        return -1;
    }
    for (count = 0; count < len; count++) {
        /* Convert character code to glyph index */
        glyph_index = FT_Get_Char_Index (face, text[count]);

        delta.x = delta.y = 0;

        err = FT_Load_Glyph (face, glyph_index, FT_LOAD_DEFAULT);
        if (err) {
            free (info);
            return -1;
        }
        info[count].index = glyph_index;
        info[count].x_offset = 0;
        info[count].y_offset = 0;
        if (delta.x != 0)
            info[count - 1].x_advance += delta.x;
        info[count].x_advance = face->glyph->metrics.horiAdvance;
        info[count].cluster = count;
    }


    *glyph_info = info;
    return count <= SSIZE_MAX ? count : 0;
}

#define METRIC_RES 300

typedef struct {
    char *fontlist;		/* key */
    int flags;			/* key */
    char *fontpath;
    FT_Library *library;
    FT_Face face;
}font_t;

typedef struct {
    const char *fontlist;		/* key */
    int flags;			/* key */
    FT_Library *library;
} fontkey_t;

static FT_Library library;

font_t *
fontFetch (char **error, fontkey_t *key, char *fontpath)
{
    font_t *a;
    fontkey_t *b = key;
    FT_Error err;
    const unsigned int b_font_list_len = strlen(b->fontlist);

    *error = NULL;

    a = (font_t *) malloc(sizeof (font_t));
    if (!a) {
        return NULL;
    }

    a->fontlist = (char *) malloc(b_font_list_len + 1);
    if (a->fontlist == NULL) {
        free(a);
        *error = "could not alloc full list of fonts";
        return NULL;
    }
    memcpy(a->fontlist, b->fontlist, b_font_list_len);
    a->fontlist[b_font_list_len] = 0;

    a->flags = b->flags;
    //a->library = b->library;
    a->library = &library;
    a->fontpath = NULL;

/*
    err = FT_New_Face(*b->library, fontpath, 0, &a->face);
*/

    err = FT_New_Face(library, "/home/pierre/projects/libgd/libgd/build/EBGaramond08-Regular.ttf", 0, &a->face);
    if (err) {
        free (a->fontlist);
        free(a->fontpath);
        free(a);
        //*error = "Could not read font";
        return NULL;
    }
    return a;
}
static void
fontRelease (font_t * font)
{
    FT_Done_Face (font->face);
    gdFree (font->fontlist);
    gdFree (font->fontpath);
    gdFree ((char *) font);
}
#define LINESPACE 1.05

char * gdContextString(gdContextPtr cr, const char *fontlist,
                                       double ptsize, double angle,
                                       double x, double y,
                                       const char *string)
{
    FT_Matrix matrix;
    FT_Vector penf;
    FT_Face face;
    FT_CharMap charmap = NULL;
    FT_Error err;
    FT_UInt glyph_index;
    double sin_a = sin (angle);
    double cos_a = cos (angle);
    int  ch;
    unsigned int i;
    font_t *font;
    fontkey_t fontkey;
    const char *next;
    uint32_t *text;
    glyphInfo *info = NULL;
    ssize_t count;
    int render_mode = FT_LOAD_NO_BITMAP;
    int encoding, encodingfound;
    /* Now tuneable thanks to Wez Furlong */
    double linespace = LINESPACE;
    /* Tuneable horizontal and vertical resolution in dots per inch */
    int hdpi, vdpi, horiAdvance;
    FT_Size platform_specific, platform_independent;

    /* get the font (via font cache) */
    fontkey.fontlist = fontlist;
    fontkey.flags = 0;
    fontkey.library = &library;
    char *error;
    font = fontFetch(&error, &fontkey, "/home/pierre/projects/libgd/libgd/build/EBGaramond08-Regular.ttf");
    face = font->face;		/* shortcut */
    FT_Vector delta;
    matrix.xx = 1L * multiplier;;
    matrix.xy = 0L * multiplier;
    matrix.yx = 0L * multiplier;
    matrix.yy = -1L * multiplier;
    delta.x = 0*64;
    delta.y = y*64;
    FT_Set_Transform(face,&matrix,&delta);
    /*
     * Added hdpi and vdpi to support images at non-screen resolutions, i.e. 300 dpi TIFF,
     *    or 100h x 50v dpi FAX format. 2.0.23.
     * 2004/02/27 Mark Shackelford, mark.shackelford@acs-inc.com
     */
    hdpi = GD_RESOLUTION;
    vdpi = GD_RESOLUTION;
    encoding = gdFTEX_Unicode;

    FT_New_Size (face, &platform_independent);
    FT_Activate_Size (platform_independent);
    if (FT_Set_Char_Size (face, 0, (FT_F26Dot6)(ptsize*64), METRIC_RES, METRIC_RES)) {
        return "Could not set character size";
    }

    FT_New_Size (face, &platform_specific);
    FT_Activate_Size (platform_specific);
    if (FT_Set_Char_Size (face, 0, (FT_F26Dot6)(ptsize*64), hdpi, vdpi)) {
        return "Could not set character size";
    }

    /* find requested charmap */
    encodingfound = 0;
    for (i = 0; i < face->num_charmaps; i++) {
        charmap = face->charmaps[i];

        if (encoding == gdFTEX_Unicode) {
            if (charmap->encoding == FT_ENCODING_MS_SYMBOL
                || charmap->encoding == FT_ENCODING_UNICODE
                || charmap->encoding == FT_ENCODING_ADOBE_CUSTOM
                || charmap->encoding == FT_ENCODING_ADOBE_STANDARD) {
                encodingfound++;
                break;
            }
        }
    }
    if (encodingfound) {
        FT_Set_Charmap(face, charmap);
    } else {
        /* No character set found! */
        return "No character set found";
    }

    next = string;
    penf.x = penf.y = 0;	/* running position of non-rotated glyphs */
    text = (uint32_t*) calloc(sizeof (uint32_t), strlen(next));
    i = 0;
    while (*next) {
        int len;
        ch = *next;


        /* use UTF-8 mapping from ASCII */
        len = gdTcl_UtfToUniChar (next, &ch);
        /* EAM DEBUG */
        /* TBB: get this exactly right: 2.1.3 *or better*, all possible cases. */
        /* 2.0.24: David R. Morrison: use the more complete ifdef here. */
        if (charmap->encoding == FT_ENCODING_MS_SYMBOL) {
            /* I do not know the significance of the constant 0xf000. */
            /* It was determined by inspection of the character codes */
            /* stored in Microsoft font symbol.ttf                    */
            ch |= 0xf000;
        }
        /* EAM DEBUG */
        next += len;


        text[i] = ch;
        i++;
    }

    FT_Activate_Size (platform_independent);

    count = textLayout (text , i, face, &info);

    if (count < 0) {
        free (text);
        return "Problem doing text layout";
    }

    gdContextClosePath(cr);
#ifdef SHOWPT
    gdContextArc(cr, x, y, 5, 0, M_PI*2);
#endif
    double off_x = x;
    gdPathPtr glyph_path;
    gdPathMatrix glyph_matrix;
#ifdef SHOWPT
    gdContextArc(cr, off_x, y, 5, 0, M_PI*2);
#endif
    for (i = 0; i < count; i++) {
        FT_Activate_Size (platform_independent);

        ch = text[info[i].cluster];

        /* carriage returns */
        if (ch == '\r') {
            penf.x = 0;
            continue;
        }

        /* newlines */
        if (ch == '\n') {
            /* 2.0.13: reset penf.x. Christopher J. Grayce */
            penf.x = 0;
            penf.y += linespace * ptsize * 64 * METRIC_RES / 72;
            penf.y &= ~63;	/* round down to 1/METRIC_RES */
            continue;
        }

        glyph_index = info[i].index;

        /* load glyph into the slot (erase previous one) */
        err = FT_Load_Glyph (face, glyph_index, render_mode);
        if (err) {
            free(text);
            return "Problem loading glyph";
        }

        horiAdvance = info[i].x_advance;

        FT_Activate_Size (platform_specific);

        /* load glyph again into the slot (erase previous one)  - this time with scaling */
        err = FT_Load_Glyph (face, glyph_index, render_mode);
        if (err) {
            free(text);
            return "Problem loading glyph";
        }

        FT_Outline_Funcs callbacks;
        callbacks.move_to = _move_to;
        callbacks.line_to = _line_to;
        callbacks.conic_to = _conic_to;
        callbacks.cubic_to = _cubic_to;
        callbacks.shift = 0;
        callbacks.delta = 0;

        FT_GlyphSlot glyph = face->glyph;
        FT_Outline outline = glyph->outline;
        glyph_path = gdPathCreate();
        err = FT_Outline_Decompose(&outline, &callbacks, glyph_path);

        if (err) {
            free(text);
            return "Problem rendering glyph";
        }
        FT_Pos pen_x = penf.x + info[i].x_offset;
        FT_Pos pen_y = penf.y - info[i].y_offset;
        double cur_x = (x + (pen_x * cos_a + pen_y * sin_a)*hdpi/(METRIC_RES*64));
        double cur_y = (y - (pen_x * sin_a - pen_y * cos_a)*vdpi/(METRIC_RES*64));
        off_x += horiAdvance *hdpi / (METRIC_RES*64);
        gdPathClose(glyph_path);
        gdPathMatrixInitTranslate(&glyph_matrix, x+(pen_x * cos_a + pen_y * sin_a)*hdpi/(METRIC_RES*64), 0);
#ifdef SHOWPT
        gdContextArc(cr, off_x, cur_y, 5, 0, M_PI*2);
#endif
        gdPathTransform(glyph_path, &glyph_matrix);

        gdContextAppendPath(cr, glyph_path);
        gdPathDestroy(glyph_path);
        gdContextMoveTo(cr, cur_x, cur_y);
        penf.x += horiAdvance;
    }
    free(text);
    if (info) {
        free(info);
    }

    FT_Done_Size (platform_independent);
    FT_Done_Size (platform_specific);
    fontRelease(font);
    return (char *) NULL;
}

int main(int argc,
         char **argv)
{
    if (argc != 2)
    {
        const char *program = argv[0];
        printf("%s <string>\n", program);
        return 1;
    }

    const char *symbol = argv[1];
    // For simplicity, only accept single-byte characters like `A'.
    if (strlen(symbol) < 1 || *symbol == 20)
    {
        printf("Invalid character <%i>\n", *symbol);
        return 2;
    }

    int status = 0;

    FT_Error error = FT_Init_FreeType(&library);
    if (error)
    {
        printf("Couldn't initialize the library: FT_Init_FreeType() failed.\n");
        exit(1);
    }

    gdSurface *surface;
    int width = 360;
    int height = 100;
    surface = gdSurfaceCreate(width, height, GD_SURFACE_ARGB32);
    if (!surface)
    {
        fprintf(stderr, "Can't create 400x400 surface\n");
        return 1;
    }
    gdContextPtr cr = gdContextCreate(surface);

    char * res = gdContextString(cr, "/home/pierre/projects/libgd/libgd/build/EBGaramond08-Regular.ttf",
                           100, 0,
                           20, 95,
                           "Hello!");
    printf("Error: %s\n", res);

    FT_Done_FreeType(library);
    gdSurfacePtr surface_fill = read_png("../examples/images/isar_512x.png");
    if (!surface_fill) {
        fprintf(stderr,  "can't read image\n");
        return 1;
    }
    gdPathPatternPtr pattern = gdPathPatternCreate(surface_fill);
    gdPathPatternSetExtend(pattern, GD_EXTEND_REPEAT);
    gdPaintPtr paint = gdPaintCreateFromPattern(pattern);
    gdContextSetSource(cr, paint);
    gdContextFillPreserve(cr);

    gdContextSetSourceRgba(cr, 1,1,1,1);
    gdContextSetLineWidth(cr, 3);

    gdContextStroke(cr);

    save_png(surface, "ft2.png");
    gdContextDestroy(cr);
    gdSurfaceDestroy(surface);;
    return status;
}
