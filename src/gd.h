#ifndef GD_H
#define GD_H 1

#include <stdlib.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Version information.  This gets parsed by build scripts as well as
 * gcc so each #define line in this group must also be splittable on
 * whitespace, take the form GD_*_VERSION and contain the magical
 * trailing comment. */
#define GD_MAJOR_VERSION 2      /*version605b5d1778*/
#define GD_MINOR_VERSION 4      /*version605b5d1778*/
#define GD_RELEASE_VERSION 0    /*version605b5d1778*/
#define GD_EXTRA_VERSION "-dev" /*version605b5d1778*/

/* End parsable section. */

/* The version string.  This is constructed from the version number
 * parts above via macro abuse^Wtrickery. */
#define GDXXX_VERSION_STR(mjr, mnr, rev, ext) mjr "." mnr "." rev ext
#define GDXXX_STR(s) GDXXX_SSTR(s) /* Two levels needed to expand args. */
#define GDXXX_SSTR(s) #s

#define GD_VERSION_STRING                                                                          \
    GDXXX_VERSION_STR(GDXXX_STR(GD_MAJOR_VERSION), GDXXX_STR(GD_MINOR_VERSION),                    \
                      GDXXX_STR(GD_RELEASE_VERSION), GD_EXTRA_VERSION)

/* Do the DLL dance: dllexport when building the DLL,
   dllimport when importing from it, nothing when
   not on Silly Silly Windows (tm Aardman Productions). */

/* 2.0.20: for headers */

/* 2.0.24: __stdcall also needed for Visual BASIC
   and other languages. This breaks ABI compatibility
   with previous DLL revs, but it's necessary. */

/* 2.0.29: WIN32 programmers can declare the NONDLL macro if they
   wish to build gd as a static library or by directly including
   the gd sources in a project. */

/* http://gcc.gnu.org/wiki/Visibility */
#if defined(_WIN32) || defined(CYGWIN) || defined(_WIN32_WCE)
#ifdef BGDWIN32
#ifdef NONDLL
#define BGD_EXPORT_DATA_PROT
#else
#ifdef __GNUC__
#define BGD_EXPORT_DATA_PROT __attribute__((__dllexport__))
#else
#define BGD_EXPORT_DATA_PROT __declspec(dllexport)
#endif
#endif
#else
#ifdef __GNUC__
#define BGD_EXPORT_DATA_PROT __attribute__((__dllimport__))
#else
#define BGD_EXPORT_DATA_PROT __declspec(dllimport)
#endif
#endif
#define BGD_STDCALL __stdcall
#define BGD_EXPORT_DATA_IMPL
#define BGD_MALLOC
#else
#if defined(__GNUC__) || defined(__clang__)
#define BGD_EXPORT_DATA_PROT __attribute__((__visibility__("default")))
#define BGD_EXPORT_DATA_IMPL __attribute__((__visibility__("hidden")))
#else
#define BGD_EXPORT_DATA_PROT
#define BGD_EXPORT_DATA_IMPL
#endif
#define BGD_STDCALL
#define BGD_MALLOC __attribute__((__malloc__))
#endif

#define BGD_DECLARE(rt) BGD_EXPORT_DATA_PROT rt BGD_STDCALL

/* VS2012+ disable keyword macroizing unless _ALLOW_KEYWORD_MACROS is set
   We define inline, and strcasecmp if they're missing
*/
#ifdef _MSC_VER
#define _ALLOW_KEYWORD_MACROS
#ifndef inline
#define inline __inline
#endif
#ifndef strcasecmp
#define strcasecmp _stricmp
#endif
#endif

#undef ARG_NOT_USED
#define ARG_NOT_USED(arg) (void)arg

/* gd.h: declarations file for the graphic-draw module.
 * Permission to use, copy, modify, and distribute this software and its
 * documentation for any purpose and without fee is hereby granted, provided
 * that the above copyright notice appear in all copies and that both that
 * copyright notice and this permission notice appear in supporting
 * documentation.  This software is provided "AS IS." Thomas Boutell and
 * Boutell.Com, Inc. disclaim all warranties, either express or implied,
 * including but not limited to implied warranties of merchantability and
 * fitness for a particular purpose, with respect to this code and accompanying
 * documentation. */

/* stdio is needed for file I/O. */
#include "gd_io.h"
#include <stdarg.h>
#include <stdio.h>

/* The maximum number of palette entries in palette-based images.
   In the wonderful new world of gd 2.0, you can of course have
   many more colors when using truecolor mode. */

#define gdMaxColors 256

/* Image type. See functions below; you will not need to change
   the elements directly. Use the provided macros to
   access sx, sy, the color table, and colorsTotal for
   read-only purposes. */

/* If 'truecolor' is set true, the image is truecolor;
   pixels are represented by integers, which
   must be 32 bits wide or more.

   True colors are represented as follows:

   ARGB

   Where 'A' (alpha channel) occupies only the
   LOWER 7 BITS of the MSB. This very small
   loss of alpha channel resolution allows gd 2.x
   to keep backwards compatibility by allowing
   signed integers to be used to represent colors,
   and negative numbers to represent special cases,
   just as in gd 1.x. */

#define gdAlphaMax 127
#define gdAlphaOpaque 0
#define gdAlphaTransparent 127
#define gdRedMax 255
#define gdGreenMax 255
#define gdBlueMax 255

/**
 * Group: Color Decomposition
 */

/**
 * Macro: gdTrueColorGetAlpha
 *
 * Gets the alpha channel value
 *
 * Parameters:
 *   c - The color
 *
 * See also:
 *   - <gdTrueColorAlpha>
 */
#define gdTrueColorGetAlpha(c) (((c) & 0x7F000000) >> 24)

/**
 * Macro: gdTrueColorGetRed
 *
 * Gets the red channel value
 *
 * Parameters:
 *   c - The color
 *
 * See also:
 *   - <gdTrueColorAlpha>
 */
#define gdTrueColorGetRed(c) (((c) & 0xFF0000) >> 16)

/**
 * Macro: gdTrueColorGetGreen
 *
 * Gets the green channel value
 *
 * Parameters:
 *   c - The color
 *
 * See also:
 *   - <gdTrueColorAlpha>
 */
#define gdTrueColorGetGreen(c) (((c) & 0x00FF00) >> 8)

/**
 * Macro: gdTrueColorGetBlue
 *
 * Gets the blue channel value
 *
 * Parameters:
 *   c - The color
 *
 * See also:
 *   - <gdTrueColorAlpha>
 */
#define gdTrueColorGetBlue(c) ((c) & 0x0000FF)

/**
 * Group: Effects
 *
 * The layering effect
 *
 * When pixels are drawn the new colors are "mixed" with the background
 * depending on the effect.
 *
 * Note that the effect does not apply to palette images, where pixels
 * are always replaced.
 *
 * Modes:
 *   gdEffectReplace    - replace pixels
 *   gdEffectAlphaBlend - blend pixels, see <gdAlphaBlend>
 *   gdEffectNormal     - default mode; same as gdEffectAlphaBlend
 *   gdEffectOverlay    - overlay pixels, see <gdLayerOverlay>
 *   gdEffectMultiply   - overlay pixels with multiply effect, see
 *                        <gdLayerMultiply>
 *
 * See also:
 *   - <gdImageAlphaBlending>
 */
#define gdEffectReplace 0
#define gdEffectAlphaBlend 1
#define gdEffectNormal 2
#define gdEffectOverlay 3
#define gdEffectMultiply 4

#define GD_TRUE 1
#define GD_FALSE 0

#define GD_EPSILON 1e-6
#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

/* This function accepts truecolor pixel values only. The
   source color is composited with the destination color
   based on the alpha channel value of the source color.
   The resulting color is opaque. */

/**
 * @brief Blend two colors
 *
 * This function accepts truecolor pixel values only. The
 * source color is composited with the destination color
 * based on the alpha channel value of the source color.
 * The resulting color is opaque.
 * @param dst The color to blend onto.
 * @param src The color to blend.
 *
 * @see gdImageAlphaBlending gdLayerOverlay gdLayerMultiply
 */
BGD_DECLARE(int) gdAlphaBlend(int dest, int src);

/**
 * @brief Overlay two colors
 *
 * @param dst The color to overlay onto.
 * @param src The color to overlay.
 *
 * @return The resulting color.
 *
 * @see gdImageAlphaBlending gdAlphaBlend gdLayerMultiply
 */
BGD_DECLARE(int) gdLayerOverlay(int dest, int src);

/**
 * @brief Overlay two colors with multiply effect
 *
 * @param dst The color to overlay onto.
 * @param src The color to overlay.
 *
 * @return The resulting color.
 * 
 * @see gdImageAlphaBlending  gdAlphaBlend gdLayerOverlay
 */
BGD_DECLARE(int) gdLayerMultiply(int dest, int src);

/**
 * @addtogroup TransformScaleRotate Transform, scale and rotate
 * @{
 */
/**
* @brief Let @ref gdImageScaleWithOptions choose the interpolation method from the scale
 * direction.
 *
 * Automatic selection uses @ref GD_LANCZOS3 for downscales or mixed-axis scales,
 * and @ref GD_CATMULLROM for pure upscales.
 */
#define GD_SCALE_INTERPOLATION_AUTO -1

/**
 * @brief gdInterpolationMethod
 *
 * Interpolation kernels used by image scaling, rotation and affine
 * transformation functions. Newly-created images use @ref GD_BILINEAR_FIXED
 * by default. Call @ref gdImageSetInterpolationMethod on the source image before
 * using APIs that read the image's current interpolation method.
 *
 * @ref gdImageScaleWithOptions can either use one of these values explicitly or
 * use @ref GD_SCALE_INTERPOLATION_AUTO to choose a method from the requested
 * scale direction.
 * @note
 *   @ref GD_WEIGHTED4 is not supported by @ref gdImageScale. For downscales or
 *   mixed-axis scales, @ref gdImageScale maps the fixed compatibility methods
 *   (@ref GD_DEFAULT, @ref GD_BILINEAR_FIXED, @ref GD_LINEAR, @ref GD_BICUBIC_FIXED and
 *   @ref GD_BICUBIC) to @ref GD_TRIANGLE to avoid the blur and aliasing of the old
 *   fixed scalers.
 *
 * @see gdImageSetInterpolationMethod gdImageScale gdImageScaleWithOptions gdImageRotateInterpolated gdTransformAffineCopy
 */
typedef enum {
    GD_DEFAULT = 0, /**< Compatibility default. Setting this resolves to @ref GD_LINEAR */
    GD_BELL,        /**< Bell filter. */
    GD_BESSEL,      /**< Bessel filter. */
    GD_BILINEAR_FIXED, /**< Compatibility bilinear scaler. */
    GD_BICUBIC,     /**< Bicubic interpolation. */
    GD_BICUBIC_FIXED, /**< Compatibility bicubic scaler. */
    GD_BLACKMAN,    /**< Blackman filter. */
    GD_BOX,         /**< Box filter. */
    GD_BSPLINE,     /**< B-spline filter. */
    GD_CATMULLROM,  /**< Catmull-Rom filter. */
    GD_GAUSSIAN,    /**< Gaussian filter. */
    GD_GENERALIZED_CUBIC, /**< Generalized cubic filter. */
    GD_HERMITE,     /**< Hermite filter. */
    GD_HAMMING,     /**< Hamming filter. */
    GD_HANNING,     /**< Hanning filter. */
    GD_MITCHELL,    /**< Mitchell filter. */
    GD_NEAREST_NEIGHBOUR, /**< Nearest-neighbour interpolation. */
    GD_POWER,     /**< Power filter. */
    GD_QUADRATIC, /**< Quadratic filter. */
    GD_SINC,      /**< Sinc filter. */
    GD_TRIANGLE,  /**< Triangle filter. */
    GD_WEIGHTED4, /**< Four-pixel weighted interpolation for rotation and affine sampling. */
    GD_LINEAR,    /**< Bilinear interpolation. */
    GD_LANCZOS3,  /**< Lanczos filter with radius 3. */
    GD_LANCZOS8,  /**< Lanczos filter with radius 8. */
    GD_BLACKMAN_BESSEL, /**< Blackman-windowed Bessel filter. */
    GD_BLACKMAN_SINC,   /**< Blackman-windowed sinc filter. */
    GD_QUADRATIC_BSPLINE, /**< Quadratic B-spline filter. */
    GD_CUBIC_SPLINE,      /**< Cubic spline filter. */
    GD_COSINE,         /**< Cosine filter. */
    GD_WELSH,       /**< Welsh filter. */
    GD_METHOD_COUNT = 30
} gdInterpolationMethod;

/* Interpolation function ptr */
typedef double (*interpolation_method)(double, double);
/** @} */

/*
   Group: Types

   typedef: gdImage

   typedef: gdImagePtr

   The data structure in which gd stores images. <gdImageCreate>,
   <gdImageCreateTrueColor> and the various image file-loading functions
   return a pointer to this type, and the other functions expect to
   receive a pointer to this type as their first argument.

   *gdImagePtr* is a pointer to *gdImage*.

   See also:
         <Accessor Macros>

   (Previous versions of this library encouraged directly manipulating
   the contents of the struct.
   We are attempting to move away from this practice so the fields
   will be considered private in 2.5 and later. )
*/
typedef struct gdImageStruct {
    /* Palette-based image pixels */
    unsigned char **pixels;
    int sx;
    int sy;
    /* These are valid in palette images only. See also
       'alpha', which appears later in the structure to
       preserve binary backwards compatibility */
    int colorsTotal;
    int red[gdMaxColors];
    int green[gdMaxColors];
    int blue[gdMaxColors];
    int open[gdMaxColors];
    /* For backwards compatibility, this is set to the
       first palette entry with 100% transparency,
       and is also set and reset by the
       gdImageColorTransparent function. Newer
       applications can allocate palette entries
       with any desired level of transparency; however,
       bear in mind that many viewers, notably
       many web browsers, fail to implement
       full alpha channel for PNG and provide
       support for full opacity or transparency only. */
    int transparent;
    int *polyInts;
    int polyAllocated;
    struct gdImageStruct *brush;
    struct gdImageStruct *tile;
    int brushColorMap[gdMaxColors];
    int tileColorMap[gdMaxColors];
    int styleLength;
    int stylePos;
    int *style;
    int interlace;
    /* New in 2.0: thickness of line. Initialized to 1. */
    int thick;
    /* New in 2.0: alpha channel for palettes. Note that only
       Macintosh Internet Explorer and (possibly) Netscape 6
       really support multiple levels of transparency in
       palettes, to my knowledge, as of 2/15/01. Most
       common browsers will display 100% opaque and
       100% transparent correctly, and do something
       unpredictable and/or undesirable for levels
       in between. TBB */
    int alpha[gdMaxColors];
    /* Truecolor flag and pixels. New 2.0 fields appear here at the
       end to minimize breakage of existing object code. */
    int trueColor;
    int **tpixels;
    /* Should alpha channel be copied, or applied, each time a
       pixel is drawn? This applies to truecolor images only.
       No attempt is made to alpha-blend in palette images,
       even if semitransparent palette entries exist.
       To do that, build your image as a truecolor image,
       then quantize down to 8 bits. */
    int alphaBlendingFlag;
    /* Should the alpha channel of the image be saved? This affects
       PNG at the moment; other future formats may also
       have that capability. JPEG doesn't. */
    int saveAlphaFlag;

    /* There should NEVER BE ACCESSOR MACROS FOR ITEMS BELOW HERE, so this
       part of the structure can be safely changed in new releases. */

    /* 2.0.12: anti-aliased globals. 2.0.26: just a few vestiges after
      switching to the fast, memory-cheap implementation from PHP-gd. */
    int AA;
    int AA_color;
    int AA_dont_blend;

    /* 2.0.12: simple clipping rectangle. These values
      must be checked for safety when set; please use
      gdImageSetClip */
    int cx1;
    int cy1;
    int cx2;
    int cy2;

    /* 2.1.0: allows to specify resolution in dpi */
    unsigned int res_x;
    unsigned int res_y;

    /* Selects quantization method, see gdImageTrueColorToPaletteSetMethod() and
     * gdPaletteQuantizationMethod enum. */
    int paletteQuantizationMethod;
    /* speed/quality trade-off. 1 = best quality, 10 = best speed. 0 =
       method-specific default. Applicable to GD_QUANT_LIQ and
       GD_QUANT_NEUQUANT. */
    int paletteQuantizationSpeed;
    /* Image will remain true-color if conversion to palette cannot achieve
       given quality. Value from 1 to 100, 1 = ugly, 100 = perfect. Applicable
       to GD_QUANT_LIQ.*/
    int paletteQuantizationMinQuality;
    /* Image will use minimum number of palette colors needed to achieve given
       quality. Must be higher than paletteQuantizationMinQuality Value from 1
       to 100, 1 = ugly, 100 = perfect. Applicable to GD_QUANT_LIQ.*/
    int paletteQuantizationMaxQuality;
    gdInterpolationMethod interpolation_id;
    interpolation_method interpolation;
} gdImage;

typedef gdImage *gdImagePtr;

typedef struct gdImageMetadata gdImageMetadata;

#define GD_META_OK 0
#define GD_META_ERR_FORMAT -1
#define GD_META_ERR_PARSE -2
#define GD_META_ERR_NOMEM -3
#define GD_META_ERR_LIMIT -4
#define GD_META_ERR_UNSUPPORTED -5
#define GD_META_ERR_INVALID -6

#define GD_METADATA_DEFAULT_MAX_PROFILE_SIZE ((size_t)64 * 1024 * 1024)
#define GD_METADATA_DEFAULT_MAX_TOTAL_SIZE ((size_t)256 * 1024 * 1024)

BGD_DECLARE(gdImageMetadata *) gdImageMetadataCreate(void);
BGD_DECLARE(void) gdImageMetadataFree(gdImageMetadata *metadata);
BGD_DECLARE(void) gdImageMetadataReset(gdImageMetadata *metadata);
BGD_DECLARE(int)
gdImageMetadataSetLimits(gdImageMetadata *metadata, size_t max_profile_size, size_t max_total_size);
BGD_DECLARE(void)
gdImageMetadataGetLimits(const gdImageMetadata *metadata, size_t *max_profile_size,
                         size_t *max_total_size);
BGD_DECLARE(int)
gdImageMetadataSetProfile(gdImageMetadata *metadata, const char *key, const unsigned char *data,
                          size_t size);
BGD_DECLARE(const unsigned char *)
gdImageMetadataGetProfile(const gdImageMetadata *metadata, const char *key, size_t *size);
BGD_DECLARE(int)
gdImageMetadataRemoveProfile(gdImageMetadata *metadata, const char *key);
BGD_DECLARE(size_t)
gdImageMetadataGetProfileCount(const gdImageMetadata *metadata);
BGD_DECLARE(int)
gdImageMetadataGetProfileAt(const gdImageMetadata *metadata, size_t index, const char **key,
                            const unsigned char **data, size_t *size);

/* Point type for use in polygon drawing. */

/**
 * @brief Defines a point in a 2D coordinate system using floating point values.
  */
typedef struct {
    double x, y; /**< Floating point coordinates. x increases from left to right, y increases from top to bottom */
} gdPointF, *gdPointFPtr;

/*
  Group: Types

  typedef: gdFont

  typedef: gdFontPtr

  A font structure, containing the bitmaps of all characters in a
  font.  Used to declare the characteristics of a font. Text-output
  functions expect these as their second argument, following the
  <gdImagePtr> argument.  <gdFontGetSmall> and <gdFontGetLarge> both
  return one.

  You can provide your own font data by providing such a structure and
  the associated pixel array. You can determine the width and height
  of a single character in a font by examining the w and h members of
  the structure. If you will not be creating your own fonts, you will
  not need to concern yourself with the rest of the components of this
  structure.

  Please see the files gdfontl.c and gdfontl.h for an example of
  the proper declaration of this structure.

  > typedef struct {
  >   // # of characters in font
  >   int nchars;
  >   // First character is numbered... (usually 32 = space)
  >   int offset;
  >   // Character width and height
  >   int w;
  >   int h;
  >   // Font data; array of characters, one row after another.
  >   // Easily included in code, also easily loaded from
  >   // data files.
  >   char *data;
  > } gdFont;

  gdFontPtr is a pointer to gdFont.

*/
typedef struct {
    /* # of characters in font */
    int nchars;
    /* First character is numbered... (usually 32 = space) */
    int offset;
    /* Character width and height */
    int w;
    int h;
    /* Font data; array of characters, one row after another.
       Easily included in code, also easily loaded from
       data files. */
    char *data;
} gdFont;

/* Text functions take these. */
typedef gdFont *gdFontPtr;

typedef void (*gdErrorMethod)(int, const char *, va_list);

BGD_DECLARE(void) gdSetErrorMethod(gdErrorMethod);
BGD_DECLARE(void) gdClearErrorMethod(void);

/**
 * Group: Colors
 *
 * Colors are always of type int which is supposed to be at least 32 bit large.
 *
 * Kinds of colors:
 *   true colors     - ARGB values where the alpha channel is stored as most
 *                     significant, and the blue channel as least significant
 *                     byte. Note that the alpha channel only uses the 7 least
 *                     significant bits.
 *                     Don't rely on the internal representation, though, and
 *                     use <gdTrueColorAlpha> to compose a truecolor value, and
 *                     <gdTrueColorGetAlpha>, <gdTrueColorGetRed>,
 *                     <gdTrueColorGetGreen> and <gdTrueColorGetBlue> to access
 *                     the respective channels.
 *   palette indexes - The index of a color palette entry (0-255).
 *   special colors  - As listed in the following section.
 *
 * Constants: Special Colors
 *   gdStyled        - use the current style, see <gdImageSetStyle>
 *   gdBrushed       - use the current brush, see <gdImageSetBrush>
 *   gdStyledBrushed - use the current style and brush
 *   gdTiled         - use the current tile, see <gdImageSetTile>
 *   gdTransparent   - indicate transparency, what is not the same as the
 *                     transparent color index; used for lines only
 *   gdAntiAliased   - draw anti aliased
 */

/* For backwards compatibility only. Use gdImageSetStyle()
   for MUCH more flexible line drawing. Also see
   gdImageSetBrush(). */
#define gdDashSize 4
#define gdStyled (-2)
#define gdBrushed (-3)
#define gdStyledBrushed (-4)
#define gdTiled (-5)

/* NOT the same as the transparent color index.
        This is used in line styles only. */
#define gdTransparent (-6)

#define gdAntiAliased (-7)

/* Functions to manipulate images. */

/* Creates a palette-based image (up to 256 colors). */
BGD_DECLARE(gdImagePtr) gdImageCreate(int sx, int sy);

/* An alternate name for the above (2.0). */
#define gdImageCreatePalette gdImageCreate

/* Creates a truecolor image (millions of colors). */
BGD_DECLARE(gdImagePtr) gdImageCreateTrueColor(int sx, int sy);

/* Creates an image from various file types. These functions
   return a palette or truecolor image based on the
   nature of the file being loaded. Truecolor PNG
   stays truecolor; palette PNG stays palette-based;
   JPEG is always truecolor. */
/**
 * @defgroup gdCodecs Codecs
 * @brief Image codec support for reading and writing various file formats.
 *
 * GD supports a range of raster codecs for loading and saving images,
 * including JPEG, PNG, GIF, WebP, and others. Each codec is exposed as its
 * own subgroup with format-specific options and functions.
 */

/**
 * @defgroup gdCodecPng PNG
 * @brief PNG image reading and writing support.
 * @ingroup gdCodecs
 *
 * PNG support preserves palette images as palette-based gd images and reads
 * truecolor PNG data as truecolor gd images. PNG output is palette-aware,
 * supports alpha, metadata, compression settings, and libpng filter options.
 *
 * @code{.c}
 *        gdImagePtr im;
 *        int black, white;
 *        FILE *out;
 *
 *        im = gdImageCreate(100, 100);
 *        if (im == NULL) {
 *          fprintf(stderr, "Unable to create image\n");
 *          exit(1);
 *        }
 *
 *        white = gdImageColorAllocate(im, 255, 255, 255);
 *        black = gdImageColorAllocate(im, 0, 0, 0);
 *        gdImageRectangle(im, 0, 0, 99, 99, black);
 *
 *        out = fopen("rect.png", "wb");
 *        if (out == NULL) {
 *           fprintf(stderr, "Unable to open output file\n");
 *           gdImageDestroy(im);
 *           exit(1);
 *        }
 *
 *        gdImagePngEx(im, out, 9);
 *        fclose(out);
 *        gdImageDestroy(im);
 * @endcode
 *
 * @{
 */

/**
 * @brief Create an image from a PNG stdio file.
 *
 * @param fd Pointer to the input FILE stream.
 *
 * @return Returns a gdImagePtr on success, or NULL on failure.
 */
BGD_DECLARE(gdImagePtr) gdImageCreateFromPng(FILE *fd);

/**
 * @brief Create an image from PNG data read through a gdIOCtx.
 *
 * @param in Pointer to the gdIOCtx input context.
 *
 * @return Returns a gdImagePtr on success, or NULL on failure.
 */
BGD_DECLARE(gdImagePtr) gdImageCreateFromPngCtx(gdIOCtxPtr in);

/**
 * @brief Create an image from a PNG memory buffer.
 *
 * @param size Size of the PNG memory buffer in bytes.
 * @param data Pointer to the PNG memory buffer.
 *
 * @return Returns a gdImagePtr on success, or NULL on failure.
 */
BGD_DECLARE(gdImagePtr) gdImageCreateFromPngPtr(int size, void *data);

/**
 * @brief Write an image as PNG data to a stdio file.
 *
 * @param im The image to write.
 * @param out The stdio file to write the PNG data to.
 */
BGD_DECLARE(void) gdImagePng(gdImagePtr im, FILE *out);

/**
 * @brief Write an image as PNG data to a gdIOCtx.
 *
 * @param im The image to write.
 * @param out The gdIOCtx to write the PNG data to.
 */
BGD_DECLARE(void) gdImagePngCtx(gdImagePtr im, gdIOCtxPtr out);

/* 2.0.12: Compression level: 0-9 or -1, where 0 is NO COMPRESSION at all,
   1 is FASTEST but produces larger files, 9 provides the best
   compression (smallest files) but takes a long time to compress, and
   -1 selects the default compiled into the zlib library. */
/**
 * @brief Write an image as PNG data to a stdio file with a compression level.
 *
 * @param im The image to write.
 * @param out The stdio file to write the PNG data to.
 * @param level Compression level: 0 for no compression, 1-9 for zlib levels, or -1 for the default.
 */
BGD_DECLARE(void) gdImagePngEx(gdImagePtr im, FILE *out, int level);

/**
 * @brief Write an image as PNG data to a gdIOCtx with a compression level.
 *
 * @param im The image to write.
 * @param out The gdIOCtx to write the PNG data to.
 * @param level Compression level: 0 for no compression, 1-9 for zlib levels, or -1 for the default.
 */
BGD_DECLARE(void) gdImagePngCtxEx(gdImagePtr im, gdIOCtxPtr out, int level);

/* Best to free this memory with gdFree(), not free() */
/**
 * @brief Write an image as PNG data to a newly allocated memory buffer.
 *
 * @param im The image to write.
 * @param size Pointer to an integer that receives the returned buffer size.
 *
 * @return A pointer to the newly allocated PNG data, or NULL on failure.
 */
BGD_DECLARE(void *) gdImagePngPtr(gdImagePtr im, int *size);

/**
 * @brief Write an image as PNG data to a memory buffer with a compression level.
 *
 * @param im The image to write.
 * @param size Pointer to an integer that receives the returned buffer size.
 * @param level Compression level: 0 for no compression, 1-9 for zlib levels, or -1 for the default.
 *
 * @return A pointer to the newly allocated PNG data, or NULL on failure.
 */
BGD_DECLARE(void *) gdImagePngPtrEx(gdImagePtr im, int *size, int level);

/** Let libpng choose PNG row filters automatically. */
#define GD_PNG_FILTER_AUTO 0U
/** Enable the PNG "None" row filter. */
#define GD_PNG_FILTER_NONE (1U << 0)
/** Enable the PNG "Sub" row filter. */
#define GD_PNG_FILTER_SUB (1U << 1)
/** Enable the PNG "Up" row filter. */
#define GD_PNG_FILTER_UP (1U << 2)
/** Enable the PNG "Average" row filter. */
#define GD_PNG_FILTER_AVERAGE (1U << 3)
/** Enable the PNG "Paeth" row filter. */
#define GD_PNG_FILTER_PAETH (1U << 4)
/** Enable all PNG row filters. */
#define GD_PNG_FILTER_ALL                                                                          \
    (GD_PNG_FILTER_NONE | GD_PNG_FILTER_SUB | GD_PNG_FILTER_UP | GD_PNG_FILTER_AVERAGE |           \
     GD_PNG_FILTER_PAETH)

/**
 * @brief PNG compression strategy values for gdPngWriteOptions.
 */
enum {
    GD_PNG_COMPRESSION_STRATEGY_DEFAULT = 0, /**< Use zlib's default strategy. */
    GD_PNG_COMPRESSION_STRATEGY_FILTERED,    /**< Prefer zlib's filtered-data strategy. */
    GD_PNG_COMPRESSION_STRATEGY_HUFFMAN_ONLY, /**< Use Huffman coding only. */
    GD_PNG_COMPRESSION_STRATEGY_RLE,         /**< Use zlib's run-length encoding strategy. */
    GD_PNG_COMPRESSION_STRATEGY_FIXED        /**< Use zlib's fixed Huffman codes strategy. */
};

/**
 * @brief Options for writing PNG data.
 */
typedef struct {
    int compression_level;           /**< PNG compression level: 0-9, or -1 for the zlib default. */
    unsigned int filters;            /**< Bitmask of GD_PNG_FILTER_* constants. */
    int compression_strategy;        /**< One of the GD_PNG_COMPRESSION_STRATEGY_* constants. */
    const gdImageMetadata *metadata; /**< Optional metadata to embed in the PNG. */
    unsigned int resolution_x;       /**< Horizontal resolution in DPI, or 0 to use the gdImage value. */
    unsigned int resolution_y;       /**< Vertical resolution in DPI, or 0 to use the gdImage value. */
} gdPngWriteOptions;

/**
 * @brief Basic information read from a PNG stream.
 *
 * PNG stores physical pixel density in the pHYs chunk as two raw
 * pixels-per-unit values plus a unit flag. When physical_unit is
 * PNG_RESOLUTION_METER, x_pixels_per_unit and y_pixels_per_unit are pixels
 * per meter and resolution_x/resolution_y contain the converted DPI values.
 * When physical_unit is PNG_RESOLUTION_UNKNOWN, the raw values describe pixel
 * aspect ratio only and resolution_x/resolution_y remain -1.
 */
typedef struct {
    int width;                   /**< Image width in pixels. */
    int height;                  /**< Image height in pixels. */
    int bit_depth;               /**< PNG bit depth from the IHDR chunk. */
    int color_type;              /**< PNG color type from the IHDR chunk. */
    int has_alpha;               /**< Non-zero if the PNG color type includes alpha. */
    int has_transparency;        /**< Non-zero if a tRNS transparency chunk is present. */
    int palette_entries;         /**< Number of palette entries, or -1 if no PLTE chunk was read. */
    int interlace_method;        /**< PNG interlace method from the IHDR chunk. */
    int x_pixels_per_unit;       /**< Raw pHYs horizontal pixels per unit, or -1 if not available. */
    int y_pixels_per_unit;       /**< Raw pHYs vertical pixels per unit, or -1 if not available. */
    int physical_unit;           /**< pHYs unit flag: PNG_RESOLUTION_UNKNOWN, PNG_RESOLUTION_METER, or -1. */
    gdImageMetadata *metadata;   /**< Optional metadata object populated while probing. */
    int decoded_truecolor;       /**< Non-zero if gd decodes this PNG as truecolor. */
    int resolution_x;            /**< Horizontal DPI converted from meter pHYs, or -1 if not available. */
    int resolution_y;            /**< Vertical DPI converted from meter pHYs, or -1 if not available. */
} gdPngInfo;

/**
 * @brief Initialize PNG write options with default values.
 *
 * @param options Pointer to the gdPngWriteOptions structure to initialize.
 */
BGD_DECLARE(void) gdPngWriteOptionsInit(gdPngWriteOptions *options);

/**
 * @brief Initialize a gdPngInfo structure with default values.
 *
 * @param info Pointer to the gdPngInfo structure to initialize.
 */
BGD_DECLARE(void) gdPngInfoInit(gdPngInfo *info);

/**
 * @brief Write an image as PNG data to a stdio file using write options.
 *
 * @param im The image to write.
 * @param out The stdio file to write the PNG data to.
 * @param options Pointer to a gdPngWriteOptions structure, or NULL for defaults.
 *
 * @return Returns 0 on success, or 1 on failure.
 */
BGD_DECLARE(int) gdImagePngWithOptions(gdImagePtr im, FILE *out, const gdPngWriteOptions *options);

/**
 * @brief Write an image as PNG data to a gdIOCtx using write options.
 *
 * @param im The image to write.
 * @param out The gdIOCtx to write the PNG data to.
 * @param options Pointer to a gdPngWriteOptions structure, or NULL for defaults.
 *
 * @return Returns 0 on success, or 1 on failure.
 */
BGD_DECLARE(int)
gdImagePngCtxWithOptions(gdImagePtr im, gdIOCtxPtr out, const gdPngWriteOptions *options);

/**
 * @brief Write an image as PNG data to a memory buffer using write options.
 *
 * @param im The image to write.
 * @param size Pointer to an integer that receives the returned buffer size.
 * @param options Pointer to a gdPngWriteOptions structure, or NULL for defaults.
 *
 * @return A pointer to the newly allocated PNG data, or NULL on failure.
 */
BGD_DECLARE(void *)
gdImagePngPtrWithOptions(gdImagePtr im, int *size, const gdPngWriteOptions *options);

/**
 * @brief Read PNG header information from a stdio file.
 *
 * @param in Pointer to the input FILE stream.
 * @param info Pointer to the gdPngInfo structure to populate.
 *
 * @return Returns 0 on success, or 1 on failure.
 */
BGD_DECLARE(int) gdPngGetInfo(FILE *in, gdPngInfo *info);

/**
 * @brief Read PNG header information from a gdIOCtx.
 *
 * @param in Pointer to the gdIOCtx input context.
 * @param info Pointer to the gdPngInfo structure to populate.
 *
 * @return Returns 0 on success, or 1 on failure.
 */
BGD_DECLARE(int) gdPngGetInfoCtx(gdIOCtxPtr in, gdPngInfo *info);

/**
 * @brief Read PNG header information from a memory buffer.
 *
 * @param size Size of the PNG memory buffer in bytes.
 * @param data Pointer to the PNG memory buffer.
 * @param info Pointer to the gdPngInfo structure to populate.
 *
 * @return Returns 0 on success, or 1 on failure.
 */
BGD_DECLARE(int) gdPngGetInfoPtr(int size, const void *data, gdPngInfo *info);

/**
 * @brief Return a string describing the linked libpng version.
 *
 * @return Returns the linked libpng version string.
 */
BGD_DECLARE(const char *) gdPngGetVersionString(void);
/** @} */

/**
 * @defgroup gdCodecQoi QOI
 * @brief QOI image reading and writing support.
 * @ingroup gdCodecs
 *
 * QOI support reads images as truecolor RGBA gd images with alpha saving
 * enabled. QOI output writes RGBA data for both truecolor and palette images;
 * palette images are expanded through their color table. The colorspace value
 * controls the QOI header colorspace flag and does not transform pixel data.
 *
 * @code{.c}
 *        gdImagePtr im;
 *        int black, white;
 *        FILE *out;
 *
 *        im = gdImageCreateTrueColor(100, 100);
 *        if (im == NULL) {
 *          fprintf(stderr, "Unable to create image\n");
 *          exit(1);
 *        }
 *
 *        white = gdTrueColor(255, 255, 255);
 *        black = gdTrueColor(0, 0, 0);
 *        gdImageFilledRectangle(im, 0, 0, 99, 99, white);
 *        gdImageRectangle(im, 0, 0, 99, 99, black);
 *
 *        out = fopen("rect.qoi", "wb");
 *        if (out == NULL) {
 *           fprintf(stderr, "Unable to open output file\n");
 *           gdImageDestroy(im);
 *           exit(1);
 *        }
 *
 *        gdImageQoi(im, out);
 *        fclose(out);
 *        gdImageDestroy(im);
 * @endcode
 *
 * @{
 */

/**
 * @brief Create an image from a QOI stdio file.
 *
 * @param fd Pointer to the input FILE stream.
 *
 * @return Returns a gdImagePtr on success, or NULL on failure.
 */
BGD_DECLARE(gdImagePtr) gdImageCreateFromQoi(FILE *fd);

/**
 * @brief Create an image from QOI data read through a gdIOCtx.
 *
 * @param in Pointer to the gdIOCtx input context.
 *
 * @return Returns a gdImagePtr on success, or NULL on failure.
 */
BGD_DECLARE(gdImagePtr) gdImageCreateFromQoiCtx(gdIOCtxPtr in);

/**
 * @brief Create an image from a QOI memory buffer.
 *
 * @param size Size of the QOI memory buffer in bytes.
 * @param data Pointer to the QOI memory buffer.
 *
 * @return Returns a gdImagePtr on success, or NULL on failure.
 */
BGD_DECLARE(gdImagePtr) gdImageCreateFromQoiPtr(int size, void *data);

/** 
 * @brief Information read from a QOI data
 */
typedef struct {
    unsigned int width; /**< Image width in pixels. */
    unsigned int height; /**< Image height in pixels. */
    int channels; /**< Number of color channels (3 for RGB, 4 for RGBA). */
    int colorspace; /**< QOI colorspace flag (GD_QOI_SRGB or GD_QOI_LINEAR). */
} gdQoiInfo;

/**
 * @brief Initialize a gdQoiInfo structure to default values.
 * 
 * The default may change in future versions, so it is recommended to call this function before using the structure.
 * Default values update is not considered a breaking change, but it is still recommended to call this function to ensure proper initialization.
 * 
 * @param info Pointer to the gdQoiInfo structure to initialize.
 */
BGD_DECLARE(void) gdQoiInfoInit(gdQoiInfo *info);

/**
 * @brief Read QOI header information from a stdio file.
 * 
 * @param infile Pointer to the input FILE stream.
 * @param info Pointer to the gdQoiInfo structure to populate.
 * 
 * @return Returns 0 on success, or 1 on failure.
 */
BGD_DECLARE(int) gdQoiGetInfo(FILE *infile, gdQoiInfo *info);

/**
 * @brief Read QOI header information from a gdIOCtx.
 * 
 * @param infile Pointer to the gdIOCtx input context.
 * @param info Pointer to the gdQoiInfo structure to populate.
 * 
 * @return Returns 0 on success, or 1 on failure.
 */
BGD_DECLARE(int) gdQoiGetInfoCtx(gdIOCtxPtr infile, gdQoiInfo *info);

/**
 * @brief Read QOI header information from a memory buffer.
 * 
 * @param size Size of the QOI memory buffer in bytes.
 * @param data Pointer to the QOI memory buffer.
 * @param info Pointer to the gdQoiInfo structure to populate.
 * 
 * @return Returns 0 on success, or 1 on failure.
 */
BGD_DECLARE(int) gdQoiGetInfoPtr(int size, const void *data, gdQoiInfo *info);

/**
 * @brief Options for writing QOI data.
 */
typedef struct {
    int colorspace; /**< QOI colorspace flag, either GD_QOI_SRGB or GD_QOI_LINEAR. */
    const gdImageMetadata *metadata; /**< Optional metadata, ignored by QOI. */
} gdQoiWriteOptions;

/**
 * @brief Initialize a gdQoiWriteOptions structure to default values.
 * 
 * The default may change in future versions, so it is recommended to call this function before using the structure.
 * Default values update is not considered a breaking change, but it is still recommended to call this function to ensure proper initialization.
 * 
 * @param options Pointer to the gdQoiWriteOptions structure to initialize.
 */
BGD_DECLARE(void) gdQoiWriteOptionsInit(gdQoiWriteOptions *options);

/**
 * @brief Write an image as QOI data to a stdio file with options.
 *
 * @param im The image to write.
 * @param out The stdio file to write the QOI data to.
 * @param options Pointer to the gdQoiWriteOptions structure specifying write options.
 *
 * @return Returns 0 on success, or 1 on failure.
 */
BGD_DECLARE(int)
gdImageQoiWithOptions(gdImagePtr im, FILE *out, const gdQoiWriteOptions *options);

/** 
 * @brief Write an image as QOI data to a gdIOCtx with options.
 * 
 * @param im The image to write.
 * @param out The gdIOCtx to write the QOI data to.
 * @param options Pointer to the gdQoiWriteOptions structure specifying write options.
 *
 * @return Returns 0 on success, or 1 on failure.
 */
BGD_DECLARE(int)
gdImageQoiCtxWithOptions(gdImagePtr im, gdIOCtxPtr out, const gdQoiWriteOptions *options);

/**
 * @brief Write an image as QOI data to a newly allocated memory buffer with options.
 * 
 * @param im The image to write.
 * @param size Pointer to an integer that receives the returned buffer size.
 * @param options Pointer to the gdQoiWriteOptions structure specifying write options.
 * 
 * @return A pointer to the newly allocated QOI data, or NULL on failure.
 */
BGD_DECLARE(void *)
gdImageQoiPtrWithOptions(gdImagePtr im, int *size, const gdQoiWriteOptions *options);

/**
 * @brief Write an image as QOI data to a newly allocated memory buffer.
 *
 * @param im The image to write.
 * @param size Pointer to an integer that receives the returned buffer size.
 *
 * @return A pointer to the newly allocated QOI data, or NULL on failure.
 */
BGD_DECLARE(void *) gdImageQoiPtr(gdImagePtr im, int *size);

/**
 * @brief Write an image as QOI data to a memory buffer with an explicit colorspace flag.
 *
 * @param im The image to write.
 * @param size Pointer to an integer that receives the returned buffer size.
 * @param colorspace The QOI colorspace flag, either GD_QOI_SRGB or GD_QOI_LINEAR.
 *
 * @return A pointer to the newly allocated QOI data, or NULL on failure.
 */
BGD_DECLARE(void *) gdImageQoiPtrEx(gdImagePtr im, int *size, int colorspace);

/**
 * @brief Write an image as QOI data to a memory buffer.
 *
 * @param im The image to write.
 * @param size Pointer to an integer that receives the returned buffer size.
 * @param metadata Reserved metadata input parameter; QOI metadata is currently ignored.
 *
 * @return A pointer to the newly allocated QOI data, or NULL on failure.
 */
/**
 * @brief Write an image as QOI data to a stdio file.
 *
 * @param im The image to write.
 * @param out The stdio file to write the QOI data to.
 */
BGD_DECLARE(void) gdImageQoi(gdImagePtr im, FILE *out);

/**
 * @brief Write an image as QOI data to a gdIOCtx.
 *
 * @param im The image to write.
 * @param out The gdIOCtx to write the QOI data to.
 */
BGD_DECLARE(void) gdImageQoiCtx(gdImagePtr im, gdIOCtxPtr out);

/**
 * @brief QOI colorspace flags written to the QOI header.
 */
enum {
    GD_QOI_SRGB = 0,  /**< Pixel data is encoded with sRGB transfer characteristics. */
    GD_QOI_LINEAR = 1 /**< Pixel data is encoded with linear transfer characteristics. */
};

/**
 * @brief Write an image as QOI data to a stdio file with an explicit colorspace flag.
 * 
 * @param im The image to write.
 * @param out The stdio file to write the QOI data to.
 */
BGD_DECLARE(void) gdImageQoi(gdImagePtr im, FILE *out);

/**
 * @brief Write an image as QOI data to a gdIOCtx with an explicit colorspace flag.
 * 
 * @param im The image to write.
 * @param out The gdIOCtx to write the QOI data to.
  */
BGD_DECLARE(void) gdImageQoiCtx(gdImagePtr im, gdIOCtxPtr out);

/**
 * @brief Write an image as QOI data to a stdio file with an explicit colorspace flag.
 *
 * @param im The image to write.
 * @param out The stdio file to write the QOI data to.
 * @param colorspace The QOI colorspace flag, either GD_QOI_SRGB or GD_QOI_LINEAR.
 */
BGD_DECLARE(void) gdImageQoiEx(gdImagePtr im, FILE *out, int colorspace);

/**
 * @brief Write an image as QOI data to a gdIOCtx with an explicit colorspace flag.
 *
 * @param im The image to write.
 * @param out The gdIOCtx to write the QOI data to.
 * @param colorspace The QOI colorspace flag, either GD_QOI_SRGB or GD_QOI_LINEAR.
 */
BGD_DECLARE(void)
gdImageQoiCtxEx(gdImagePtr im, gdIOCtxPtr out, int colorspace);

/** @} */

/**
 * @defgroup gdCodecGif GIF
 * @brief GIF image and animation reading and writing support.
 * @ingroup gdCodecs
 *
 * GIF support reads single images as palette-based gd images and writes
 * palette-based GIF data, quantizing truecolor input when needed. Animated GIF
 * support includes a reader for raw frames or composited images and a legacy
 * begin/add/end writer API.
 *
 * @code{.c}
 *        gdImagePtr im;
 *        gdImagePtr prev = NULL;
 *        FILE *out;
 *        int i;
 *
 *        im = gdImageCreate(100, 100);
 *        if (im == NULL) {
 *          fprintf(stderr, "Unable to create image\n");
 *          exit(1);
 *        }
 *
 *        gdImageColorAllocate(im, 255, 255, 255);
 *
 *        out = fopen("anim.gif", "wb");
 *        if (out == NULL) {
 *           fprintf(stderr, "Unable to open output file\n");
 *           gdImageDestroy(im);
 *           exit(1);
 *        }
 *
 *        gdImageGifAnimBegin(im, out, 1, -1);
 *        for (i = 0; i < 20; i++) {
 *          gdImagePtr frame;
 *          int color;
 *
 *          frame = gdImageCreate(100, 100);
 *          if (frame == NULL) {
 *            break;
 *          }
 *          gdImageColorAllocate(frame, 255, 255, 255);
 *          color = gdImageColorAllocate(frame, i * 10, 0, 255 - i * 10);
 *          gdImageFilledRectangle(frame, 10 + i, 10 + i, 40 + i, 40 + i, color);
 *          gdImageGifAnimAdd(frame, out, 1, 0, 0, 10, GD_GIF_DISPOSAL_NONE, prev);
 *          if (prev != NULL) {
 *            gdImageDestroy(prev);
 *          }
 *          prev = frame;
 *        }
 *        if (prev != NULL) {
 *          gdImageDestroy(prev);
 *        }
 *        gdImageGifAnimEnd(out);
 *        fclose(out);
 *        gdImageDestroy(im);
 * @endcode
 *
 * @{
 */

/**
 * @name Single-frame GIF reading and writing
 * @{
 */

/**
 * @brief Create an image from the first frame of a GIF stdio file.
 *
 * @param fd Pointer to the input FILE stream.
 *
 * @return Returns a caller-owned gdImagePtr on success, or NULL on failure.
 */
BGD_DECLARE(gdImagePtr) gdImageCreateFromGif(FILE *fd);

/**
 * @brief Create an image from the first frame of GIF data read through a gdIOCtx.
 *
 * @param in Pointer to the gdIOCtx input context.
 *
 * @return Returns a caller-owned gdImagePtr on success, or NULL on failure.
 */
BGD_DECLARE(gdImagePtr) gdImageCreateFromGifCtx(gdIOCtxPtr in);

/**
 * @brief Create an image from the first frame of a GIF memory buffer.
 *
 * @param size Size of the GIF memory buffer in bytes.
 * @param data Pointer to the GIF memory buffer.
 *
 * @return Returns a caller-owned gdImagePtr on success, or NULL on failure.
 */
BGD_DECLARE(gdImagePtr) gdImageCreateFromGifPtr(int size, void *data);

/**
 * @brief Write an image as GIF data to a gdIOCtx.
 *
 * @param im The image to write.
 * @param out The gdIOCtx to write the GIF data to.
 */
BGD_DECLARE(void) gdImageGifCtx(gdImagePtr im, gdIOCtxPtr out);

/**
 * @brief Write an image as GIF data to a stdio file.
 *
 * @param im The image to write.
 * @param out The stdio file to write the GIF data to.
 */
BGD_DECLARE(void) gdImageGif(gdImagePtr im, FILE *out);

/**
 * @brief Write an image as GIF data to a newly allocated memory buffer.
 *
 * @param im The image to write.
 * @param size Pointer to an integer that receives the returned buffer size.
 *
 * @return A pointer to the newly allocated GIF data, or NULL on failure. Free
 *         the returned buffer with gdFree().
 */
BGD_DECLARE(void *) gdImageGifPtr(gdImagePtr im, int *size);

/** @} */

/**
 * @name Animated GIF reading
 * @{
 */

/**
 * @brief Opaque animated GIF reader handle.
 */
typedef struct gdGifReadStruct *gdGifReadPtr;

/**
 * @brief Basic information read from a GIF stream.
 */
typedef struct {
    char version[4];       /**< GIF version, excluding the terminating NUL. */
    int width;            /**< Logical screen width in pixels. */
    int height;           /**< Logical screen height in pixels. */
    int background_index;      /**< GIF logical screen background color index. */
    int global_color_table;    /**< Non-zero if the GIF has a global color table. */
    int color_resolution;      /**< GIF color resolution in bits per primary color. */
    double pixel_aspect_ratio; /**< GIF pixel aspect ratio, or 1.0 when unspecified. */
    int loop_count;            /**< Netscape loop count, 0 for infinite, or 1 when absent. */
    int loop_count_present;    /**< Non-zero if a Netscape loop count was present. */
} gdGifInfo;

/**
 * @brief Per-frame information read from a GIF animation.
 */
typedef struct {
    int frame_index;       /**< Zero-based frame index. */
    int x;                /**< Frame left offset on the logical screen. */
    int y;                /**< Frame top offset on the logical screen. */
    int width;            /**< Frame width in pixels. */
    int height;           /**< Frame height in pixels. */
    int delay;            /**< Frame delay in hundredths of a second. */
    int disposal;         /**< One of the GD_GIF_DISPOSAL_* constants. */
    int transparent_index; /**< Transparent color index, or -1 if not present. */
    int local_color_table; /**< Non-zero if this frame has a local color table. */
    int interlace;        /**< Non-zero if this frame is interlaced. */
} gdGifFrameInfo;

/**
 * @brief Test whether a seekable GIF stdio file contains more than one frame.
 *
 * @param fd Pointer to the input FILE stream.
 *
 * @return Returns 1 if animated, 0 if readable but not animated, or -1 on error.
 */
BGD_DECLARE(int) gdGifIsAnimated(FILE *fd);

/**
 * @brief Test whether a seekable GIF gdIOCtx contains more than one frame.
 *
 * @param in Pointer to the gdIOCtx input context.
 *
 * @return Returns 1 if animated, 0 if readable but not animated, or -1 on error.
 */
BGD_DECLARE(int) gdGifIsAnimatedCtx(gdIOCtxPtr in);

/**
 * @brief Test whether a GIF memory buffer contains more than one frame.
 *
 * @param size Size of the GIF memory buffer in bytes.
 * @param data Pointer to the GIF memory buffer.
 *
 * @return Returns 1 if animated, 0 if readable but not animated, or -1 on error.
 */
BGD_DECLARE(int) gdGifIsAnimatedPtr(int size, void *data);

/**
 * @brief Open an animated GIF reader from a stdio file.
 *
 * @param fd Pointer to the input FILE stream.
 *
 * @return Returns a gdGifReadPtr on success, or NULL on failure. Close it with
 *         gdGifReadClose().
 */
BGD_DECLARE(gdGifReadPtr) gdGifReadOpen(FILE *fd);

/**
 * @brief Open an animated GIF reader from a gdIOCtx.
 *
 * @param in Pointer to the gdIOCtx input context. The reader does not take
 *        ownership of this context.
 *
 * @return Returns a gdGifReadPtr on success, or NULL on failure. Close it with
 *         gdGifReadClose().
 */
BGD_DECLARE(gdGifReadPtr) gdGifReadOpenCtx(gdIOCtxPtr in);

/**
 * @brief Open an animated GIF reader from a memory buffer.
 *
 * @param size Size of the GIF memory buffer in bytes.
 * @param data Pointer to the GIF memory buffer.
 *
 * @return Returns a gdGifReadPtr on success, or NULL on failure. Close it with
 *         gdGifReadClose().
 */
BGD_DECLARE(gdGifReadPtr) gdGifReadOpenPtr(int size, void *data);

/**
 * @brief Close an animated GIF reader.
 *
 * @param gif The GIF reader to close.
 */
BGD_DECLARE(void) gdGifReadClose(gdGifReadPtr gif);

/**
 * @brief Read logical screen and loop information from a GIF reader.
 *
 * @param gif The GIF reader.
 * @param info Pointer to the gdGifInfo structure to populate.
 *
 * @return Returns 1 on success, or 0 on failure.
 */
BGD_DECLARE(int) gdGifReadGetInfo(gdGifReadPtr gif, gdGifInfo *info);

/**
 * @brief Read logical screen and loop information from a GIF stdio file.
 *
 * The input stream position is restored before returning.
 */
BGD_DECLARE(int) gdGifGetInfo(FILE *file, gdGifInfo *info);

/**
 * @brief Read logical screen and loop information from a seekable gdIOCtx.
 *
 * The input context position is restored before returning.
 */
BGD_DECLARE(int) gdGifGetInfoCtx(gdIOCtxPtr input, gdGifInfo *info);

/**
 * @brief Read logical screen and loop information from a GIF memory buffer.
 */
BGD_DECLARE(int) gdGifGetInfoPtr(int size, const void *data, gdGifInfo *info);

/**
 * @brief Read the next raw GIF frame.
 *
 * @param gif The GIF reader.
 * @param info Pointer to a gdGifFrameInfo structure to populate, or NULL.
 * @param frame Pointer to receive a caller-owned raw frame image, or NULL to
 *        skip receiving the image.
 *
 * @return Returns 1 when a frame is read, 0 at end of stream, or -1 on error.
 */
BGD_DECLARE(int)
gdGifReadNextFrame(gdGifReadPtr gif, gdGifFrameInfo *info, gdImagePtr *frame);

/**
 * @brief Read the next GIF frame composited onto the logical screen.
 *
 * @param gif The GIF reader.
 * @param info Pointer to a gdGifFrameInfo structure to populate, or NULL.
 * @param image Pointer to receive a caller-owned composited image, or NULL to
 *        skip receiving the image.
 *
 * @return Returns 1 when an image is read, 0 at end of stream, or -1 on error.
 */
BGD_DECLARE(int)
gdGifReadNextImage(gdGifReadPtr gif, gdGifFrameInfo *info, gdImagePtr *image);

/** @} */

/**
 * @name GIF animation disposal constants
 * @{
 */

/**
 * @brief GIF frame disposal methods.
 */
enum {
    gdDisposalUnknown,           /**< Unknown disposal method; not recommended for writing. */
    gdDisposalNone,              /**< Preserve previous frame contents. */
    gdDisposalRestoreBackground, /**< Restore the frame area to the background color. */
    gdDisposalRestorePrevious    /**< Restore the frame area to its previous contents. */
};

/** Alias for gdDisposalUnknown. */
#define GD_GIF_DISPOSAL_UNKNOWN gdDisposalUnknown
/** Alias for gdDisposalNone. */
#define GD_GIF_DISPOSAL_NONE gdDisposalNone
/** Alias for gdDisposalRestoreBackground. */
#define GD_GIF_DISPOSAL_RESTORE_BACKGROUND gdDisposalRestoreBackground
/** Alias for gdDisposalRestorePrevious. */
#define GD_GIF_DISPOSAL_RESTORE_PREVIOUS gdDisposalRestorePrevious

/** @} */

/**
 * @name Animated GIF writing
 * @{
 */

/**
 * @brief Begin writing a GIF animation to a stdio file.
 *
 * @param im Reference image used for logical screen size, interlace flag, and
 *        optional global color table.
 * @param outFile The stdio file to write to.
 * @param GlobalCM Global color table flag: 1 to write one, 0 to omit it, or -1
 *        for the default.
 * @param Loops Loop count: 0 for infinite looping, -1 to omit the loop
 *        extension, or a positive finite loop count.
 */
BGD_DECLARE(void)
gdImageGifAnimBegin(gdImagePtr im, FILE *outFile, int GlobalCM, int Loops);

/**
 * @brief Add a frame to a GIF animation written to a stdio file.
 *
 * @param im The frame image to add.
 * @param outFile The stdio file to write to.
 * @param LocalCM Local color table flag: 1 to write one, 0 to use the global
 *        color table, or -1 for the default.
 * @param LeftOfs Frame left offset on the logical screen.
 * @param TopOfs Frame top offset on the logical screen.
 * @param Delay Frame delay in hundredths of a second.
 * @param Disposal One of the GD_GIF_DISPOSAL_* constants.
 * @param previm Previous frame image for built-in optimization, or NULL.
 */
BGD_DECLARE(void)
gdImageGifAnimAdd(gdImagePtr im, FILE *outFile, int LocalCM, int LeftOfs, int TopOfs, int Delay,
                  int Disposal, gdImagePtr previm);

/**
 * @brief Finish writing a GIF animation to a stdio file.
 *
 * @param outFile The stdio file to write to.
 */
BGD_DECLARE(void) gdImageGifAnimEnd(FILE *outFile);

/**
 * @brief Begin writing a GIF animation to a gdIOCtx.
 *
 * @param im Reference image used for logical screen size, interlace flag, and
 *        optional global color table.
 * @param out The gdIOCtx to write to.
 * @param GlobalCM Global color table flag: 1 to write one, 0 to omit it, or -1
 *        for the default.
 * @param Loops Loop count: 0 for infinite looping, -1 to omit the loop
 *        extension, or a positive finite loop count.
 */
BGD_DECLARE(void)
gdImageGifAnimBeginCtx(gdImagePtr im, gdIOCtxPtr out, int GlobalCM, int Loops);

/**
 * @brief Add a frame to a GIF animation written to a gdIOCtx.
 *
 * @param im The frame image to add.
 * @param out The gdIOCtx to write to.
 * @param LocalCM Local color table flag: 1 to write one, 0 to use the global
 *        color table, or -1 for the default.
 * @param LeftOfs Frame left offset on the logical screen.
 * @param TopOfs Frame top offset on the logical screen.
 * @param Delay Frame delay in hundredths of a second.
 * @param Disposal One of the GD_GIF_DISPOSAL_* constants.
 * @param previm Previous frame image for built-in optimization, or NULL.
 */
BGD_DECLARE(void)
gdImageGifAnimAddCtx(gdImagePtr im, gdIOCtxPtr out, int LocalCM, int LeftOfs, int TopOfs, int Delay,
                     int Disposal, gdImagePtr previm);

/**
 * @brief Finish writing a GIF animation to a gdIOCtx.
 *
 * @param out The gdIOCtx to write to.
 */
BGD_DECLARE(void) gdImageGifAnimEndCtx(gdIOCtxPtr out);

/**
 * @brief Begin writing a GIF animation to a newly allocated memory buffer.
 *
 * @param im Reference image used for logical screen size, interlace flag, and
 *        optional global color table.
 * @param size Pointer to an integer that receives the returned buffer size.
 * @param GlobalCM Global color table flag: 1 to write one, 0 to omit it, or -1
 *        for the default.
 * @param Loops Loop count: 0 for infinite looping, -1 to omit the loop
 *        extension, or a positive finite loop count.
 *
 * @return A pointer to the newly allocated GIF animation header data, or NULL
 *         on failure. Free the returned buffer with gdFree().
 */
BGD_DECLARE(void *)
gdImageGifAnimBeginPtr(gdImagePtr im, int *size, int GlobalCM, int Loops);

/**
 * @brief Add a GIF animation frame to a newly allocated memory buffer.
 *
 * @param im The frame image to add.
 * @param size Pointer to an integer that receives the returned buffer size.
 * @param LocalCM Local color table flag: 1 to write one, 0 to use the global
 *        color table, or -1 for the default.
 * @param LeftOfs Frame left offset on the logical screen.
 * @param TopOfs Frame top offset on the logical screen.
 * @param Delay Frame delay in hundredths of a second.
 * @param Disposal One of the GD_GIF_DISPOSAL_* constants.
 * @param previm Previous frame image for built-in optimization, or NULL.
 *
 * @return A pointer to the newly allocated GIF animation frame data, or NULL
 *         on failure. Free the returned buffer with gdFree().
 */
BGD_DECLARE(void *)
gdImageGifAnimAddPtr(gdImagePtr im, int *size, int LocalCM, int LeftOfs, int TopOfs, int Delay,
                     int Disposal, gdImagePtr previm);

/**
 * @brief Finish a GIF animation into a newly allocated memory buffer.
 *
 * @param size Pointer to an integer that receives the returned buffer size.
 *
 * @return A pointer to the newly allocated GIF animation terminator data, or
 *         NULL on failure. Free the returned buffer with gdFree().
 */
BGD_DECLARE(void *) gdImageGifAnimEndPtr(int *size);

/** @} */
/** @} */

/**
 * @defgroup gdCodecWbmp WBMP
 * @brief Wireless Bitmap reading and writing support.
 * @ingroup gdCodecs
 *
 * WBMP support reads Wireless Bitmap Type 0 images into palette-based gd
 * images with white and black colors. WBMP output writes a 1-bit image: pixels
 * whose color matches the foreground color parameter are written as black, and
 * all other pixels are written as white.
 *
 * @code{.c}
 *        gdImagePtr im, roundtrip;
 *        int white, black;
 *        void *data;
 *        int size;
 *
 *        im = gdImageCreate(100, 100);
 *        if (im == NULL) {
 *          exit(1);
 *        }
 *
 *        white = gdImageColorAllocate(im, 255, 255, 255);
 *        black = gdImageColorAllocate(im, 0, 0, 0);
 *        gdImageFilledRectangle(im, 0, 0, 99, 99, white);
 *        gdImageRectangle(im, 20, 20, 79, 79, black);
 *
 *        data = gdImageWBMPPtr(im, &size, black);
 *        if (data == NULL) {
 *          gdImageDestroy(im);
 *          exit(1);
 *        }
 *
 *        roundtrip = gdImageCreateFromWBMPPtr(size, data);
 *        gdFree(data);
 *        gdImageDestroy(roundtrip);
 *        gdImageDestroy(im);
 * @endcode
 *
 * @{
 */

/** @name WBMP Reading */
/** @{ */

/**
 * @brief Create an image from a WBMP stdio file.
 *
 * gdImageCreateFromWBMP() does not close inFile. The returned image is
 * caller-owned and must be destroyed with @ref gdImageDestroy.
 *
 * @param inFile Pointer to the input FILE stream.
 *
 * @return Returns a gdImagePtr on success, or NULL on failure.
 */
BGD_DECLARE(gdImagePtr) gdImageCreateFromWBMP(FILE *inFile);

/**
 * @brief Create an image from WBMP data read through a gdIOCtx.
 *
 * gdImageCreateFromWBMPCtx() does not close infile. The returned image is
 * caller-owned and must be destroyed with @ref gdImageDestroy.
 *
 * @param infile Pointer to the gdIOCtx input context.
 *
 * @return Returns a gdImagePtr on success, or NULL on failure.
 */
BGD_DECLARE(gdImagePtr) gdImageCreateFromWBMPCtx(gdIOCtxPtr infile);

/**
 * @brief Create an image from a WBMP memory buffer.
 *
 * The data buffer is borrowed for the duration of the call. The returned image
 * is caller-owned and must be destroyed with @ref gdImageDestroy.
 *
 * @param size Size of the WBMP memory buffer in bytes.
 * @param data Pointer to the WBMP memory buffer.
 *
 * @return Returns a gdImagePtr on success, or NULL on failure.
 */
BGD_DECLARE(gdImagePtr) gdImageCreateFromWBMPPtr(int size, void *data);

/** @} */
/** @} */

/**
 * @defgroup gdCodecJpeg JPEG
 * @brief JPEG image reading and writing support.
 * @ingroup gdCodecs
 * 
 * GD supports a range of raster codecs for loading and saving images,
 * including JPEG, PNG, GIF, WebP, and others. Each codec is exposed as its
 * own subgroup with format-specific options and functions.
 * 
 * @code{.c}
 *        gdImagePtr im;
 *        int black, white;
 *        FILE *out;

 *        im = gdImageCreate(100, 100);
 *        if (im == NULL) {
 *          fprintf(stderr, "Unable to create image\n");
 *          exit(1);
 *        }
 * 
 *        // Allocate background
 *        white = gdImageColorAllocate(im, 255, 255, 255);
 * 
 *        // Allocate drawing color
 *        black = gdImageColorAllocate(im, 0, 0, 0);
 * 
 *        // Draw rectangle
 *        gdImageRectangle(im, 0, 0, 99, 99, black);
 * 
 *        // Open output file in binary mode
 *        out = fopen("rect.jpg", "wb");
 *        if (out == NULL) {
 *           fprintf(stderr, "Unable to open output file\n");
 *           exit(1);
 *        }
 *        // Write JPEG using default quality
 *        gdImageJpeg(im, out, -1);
 *        // Close file
 *        fclose(out);
 *        // Destroy image
 *        gdImageDestroy(im);
 * @endcode
 *
 * @ingroup gdCodecs
 * @{
 */

/**
 * @brief JPEG color space identifiers reported by gdJpegInfo.
 */
enum {
    GD_JPEG_COLOR_SPACE_UNKNOWN = 0,   /**< Unknown or unsupported JPEG color space. */
    GD_JPEG_COLOR_SPACE_GRAYSCALE = 1, /**< Grayscale JPEG color space. */
    GD_JPEG_COLOR_SPACE_RGB = 2,       /**< RGB JPEG color space. */
    GD_JPEG_COLOR_SPACE_YCBCR = 3,     /**< YCbCr JPEG color space. */
    GD_JPEG_COLOR_SPACE_CMYK = 4,      /**< CMYK JPEG color space. */
    GD_JPEG_COLOR_SPACE_YCCK = 5       /**< YCCK JPEG color space. */
};

/**
 * @brief JPEG density units reported by gdJpegInfo.
 */
enum {
    GD_JPEG_DENSITY_UNIT_NONE = 0, /**< No density unit is specified. */
    GD_JPEG_DENSITY_UNIT_DPI = 1,  /**< Density is measured in dots per inch. */
    GD_JPEG_DENSITY_UNIT_DPCM = 2  /**< Density is measured in dots per centimeter. */
};

/**
 * @brief JPEG DCT method options for gdJpegReadOptions.
 */
enum {
    GD_JPEG_DCT_DEFAULT = 0, /**< Use the JPEG library default DCT method. */
    GD_JPEG_DCT_SLOW = 1,    /**< Use the slow integer DCT method. */
    GD_JPEG_DCT_FAST = 2,    /**< Use the fast integer DCT method. */
    GD_JPEG_DCT_FLOAT = 3    /**< Use the floating-point DCT method. */
};

/**
 * @brief Basic information read from a JPEG header.
 */
typedef struct {
    int width;            /**< Image width in pixels. */
    int height;           /**< Image height in pixels. */
    int bits_per_sample;  /**< Bits per sample reported by the JPEG library. */
    int components;       /**< Number of image components. */
    int color_space;      /**< One of the GD_JPEG_COLOR_SPACE_* constants. */
    int progressive;      /**< Non-zero if the image is progressive. */
    int density_unit;     /**< One of the GD_JPEG_DENSITY_UNIT_* constants. */
    int x_density;        /**< Horizontal density, or -1 if not available. */
    int y_density;        /**< Vertical density, or -1 if not available. */
} gdJpegInfo;

/**
 * @brief Options for reading JPEG data.
 *
 * scale_num / scale_denom: The ratio to scale by.
 */
typedef struct {
    int ignore_warning;       /**< Non-zero to suppress recoverable JPEG warnings. */
    unsigned int scale_num;   /**< Decode scale numerator. When build against libjpeg-turbo, the library handles the scaling internally. With libjpeg, the available closed scaling factors are handled by the library, and GD handles the requested scaling then. */
    unsigned int scale_denom; /**< Decode scale denominator. */
    int dct_method;           /**< One of the GD_JPEG_DCT_* constants. */
} gdJpegReadOptions;

/**
 * @brief Options for writing JPEG data.
 */
typedef struct {
    int quality;                     /**< JPEG quality, or -1 for the JPEG library default. */
    int progressive;                 /**< Controls progressive JPEG output. */
    int force_no_subsampling;        /**< Non-zero to force 4:4:4 chroma sampling. */
    const gdImageMetadata *metadata; /**< Optional metadata to embed in the JPEG. */
} gdJpegWriteOptions;

/**
 * @brief Initialize a gdJpegInfo structure with default values.
 * 
 * @param info Pointer to the gdJpegInfo structure to initialize.
 */
BGD_DECLARE(void) gdJpegInfoInit(gdJpegInfo *info);

/**
 * @brief Initialize JPEG read options with default values.
 * 
 * @param options Pointer to the gdJpegReadOptions structure to initialize.
 */
BGD_DECLARE(void) gdJpegReadOptionsInit(gdJpegReadOptions *options);

/**
 * @brief Initialize JPEG write options with default values.
 * 
 * @param options Pointer to the gdJpegWriteOptions structure to initialize.
 */
BGD_DECLARE(void) gdJpegWriteOptionsInit(gdJpegWriteOptions *options);

/**
 * @brief Read JPEG header information from a stdio file.
 * 
 * @param infile Pointer to the input FILE stream.
 * @param info Pointer to the gdJpegInfo structure to populate with header information.
 * 
 * @return Returns 1 on success, 0 on failure.
 */
BGD_DECLARE(int) gdJpegGetInfo(FILE *infile, gdJpegInfo *info);


/**
 * @brief Read JPEG header information from a gdIOCtx.
 * 
 * @param infile Pointer to the gdIOCtx input context.
 * @param info Pointer to the gdJpegInfo structure to populate with header information.
 * 
 * @return Returns 1 on success, 0 on failure.
 */
BGD_DECLARE(int) gdJpegGetInfoCtx(gdIOCtxPtr infile, gdJpegInfo *info);

/**
 * @brief Read JPEG header information from a memory buffer.
 * 
 * @param size Size of the memory buffer.
 * @param data Pointer to the memory buffer containing JPEG data.
 * @param info Pointer to the gdJpegInfo structure to populate with header information.
 * 
 * @return Returns 1 on success, 0 on failure.
 */
BGD_DECLARE(int) gdJpegGetInfoPtr(int size, const void *data, gdJpegInfo *info);

BGD_DECLARE(int) gdJpegGetMetadata(FILE *infile, gdImageMetadata *metadata);
BGD_DECLARE(int) gdJpegGetMetadataCtx(gdIOCtxPtr infile, gdImageMetadata *metadata);
BGD_DECLARE(int) gdJpegGetMetadataPtr(int size, const void *data, gdImageMetadata *metadata);

/**
 * @brief Create an image from a JPEG stdio file.
 * 
 * @param infile Pointer to the input FILE stream.
 * 
 * @return Returns a gdImagePtr on success, or NULL on failure.
 */
BGD_DECLARE(gdImagePtr) gdImageCreateFromJpeg(FILE *infile);

/**
 * @brief Create an image from a JPEG stdio file, controlling warning handling.
 * 
 * @param infile Pointer to the input FILE stream.
 * @param ignore_warning Non-zero to suppress recoverable JPEG warnings.
 * 
 * @return Returns a gdImagePtr on success, or NULL on failure.
 */
BGD_DECLARE(gdImagePtr)
gdImageCreateFromJpegEx(FILE *infile, int ignore_warning);

/**
 * @brief Create an image from JPEG data read through a gdIOCtx.
 * 
 * @param infile Pointer to the gdIOCtx input context.
 * 
 * @return Returns a gdImagePtr on success, or NULL on failure.
 */
BGD_DECLARE(gdImagePtr) gdImageCreateFromJpegCtx(gdIOCtxPtr infile);

/**
 * @brief Create an image from a JPEG gdIOCtx, controlling warning handling.
 * 
 * @param infile Pointer to the gdIOCtx input context.
 * @param ignore_warning Non-zero to suppress recoverable JPEG warnings.
 * 
 * @return Returns a gdImagePtr on success, or NULL on failure.
 */
BGD_DECLARE(gdImagePtr)
gdImageCreateFromJpegCtxEx(gdIOCtxPtr infile, int ignore_warning);

/**
 * @brief Create an image from a JPEG gdIOCtx and collect metadata.
 * 
 * @param infile Pointer to the gdIOCtx input context.
 * @param metadata Pointer to a gdImageMetadata structure to collect metadata.
 * 
 * @return Returns a gdImagePtr on success, or NULL on failure.
 */

/**
 * @brief Create an image from a JPEG gdIOCtx with warning control and metadata collection.
 * 
 * @param infile Pointer to the gdIOCtx input context.
 * @param ignore_warning Non-zero to suppress recoverable JPEG warnings.
 * @param metadata Pointer to a gdImageMetadata structure to collect metadata.
 * 
 * @return Returns a gdImagePtr on success, or NULL on failure.
 */

/**
 * @brief Create an image from a JPEG gdIOCtx using read options.
 * 
 * @param infile Pointer to the gdIOCtx input context.
 * @param options Pointer to a gdJpegReadOptions structure specifying read options.
 * 
 * @return Returns a gdImagePtr on success, or NULL on failure.
 */
BGD_DECLARE(gdImagePtr)
gdImageCreateFromJpegCtxWithOptions(gdIOCtxPtr infile, const gdJpegReadOptions *options);

/**
 * @brief Create an image from a JPEG memory buffer.
 * 
 * @param size The size of the JPEG memory buffer.
 * @param data Pointer to the JPEG memory buffer.
 * 
 * @return Returns a gdImagePtr on success, or NULL on failure.
 */
BGD_DECLARE(gdImagePtr) gdImageCreateFromJpegPtr(int size, void *data);
/**
 * @brief Create an image from a JPEG memory buffer, controlling warning handling.
 * 
 * @param size The size of the JPEG memory buffer.
 * @param data Pointer to the JPEG memory buffer.
 * @param ignore_warning Non-zero to suppress recoverable JPEG warnings.
 * 
 * @return Returns a gdImagePtr on success, or NULL on failure.
 */
BGD_DECLARE(gdImagePtr)
gdImageCreateFromJpegPtrEx(int size, void *data, int ignore_warning);

/**
 * @brief Create an image from a JPEG memory buffer using read options.
 * 
 * @param size The size of the JPEG memory buffer.
 * @param data Pointer to the JPEG memory buffer.
 * @param options Pointer to a gdJpegReadOptions structure specifying read options.
 * 
 * @return Returns a gdImagePtr on success, or NULL on failure.
 */
BGD_DECLARE(gdImagePtr)
gdImageCreateFromJpegPtrWithOptions(int size, void *data, const gdJpegReadOptions *options);

/** Create a JPEG image from memory using read options and collect metadata. */

/**
 * @brief Create an image from a JPEG memory buffer and collect metadata.
 * 
 * @param size The size of the JPEG memory buffer.
 * @param data Pointer to the JPEG memory buffer.
 * @param metadata Pointer to a gdImageMetadata structure to collect metadata.
 * 
 * @return Returns a gdImagePtr on success, or NULL on failure.
 */

/**
 * @brief Create an image from a JPEG memory buffer with warning control and metadata collection.
 * 
 * @param size The size of the JPEG memory buffer.
 * @param data Pointer to the JPEG memory buffer.
 * @param ignore_warning Non-zero to suppress recoverable JPEG warnings.
 * @param metadata Pointer to a gdImageMetadata structure to collect metadata.
 * 
 * @return Returns a gdImagePtr on success, or NULL on failure.
 */

/**
 * @brief Return a string describing the linked JPEG library version.
 * 
 * @return Returns a string describing the linked JPEG library version.
 */
BGD_DECLARE(const char *) gdJpegGetVersionString();
/** @} */

/**
 * @defgroup gdCodecWebp WebP
 * @brief WebP image reading, writing and animations support.
 * @ingroup gdCodecs
 *
 * WebP support reads still images as truecolor gd images and provides
 * animation readers for raw frame rectangles or coalesced full-canvas images.
 * WebP writers accept truecolor gd images; single-image pointer writers return
 * buffers that must be freed with gdFree(), and animation writers are closed
 * with gdWebpWriteClose() or gdWebpWritePtrFinish().
 *
 * @code{.c}
 *        FILE *in, *out;
 *        gdWebpReadPtr reader;
 *        gdWebpWritePtr writer;
 *        gdWebpInfo info;
 *        gdWebpFrameInfo frameInfo;
 *        gdWebpAnimWriteOptions options;
 *        gdImagePtr image;
 *        int result;
 *
 *        in = fopen("input.webp", "rb");
 *        if (in == NULL) {
 *          fprintf(stderr, "cannot open input.webp\n");
 *          exit(1);
 *        }
 *
 *        reader = gdWebpReadOpen(in, NULL);
 *        fclose(in);
 *        if (reader == NULL || !gdWebpReadGetInfo(reader, &info)) {
 *          fprintf(stderr, "cannot read WebP\n");
 *          if (reader != NULL) {
 *            gdWebpReadClose(reader);
 *          }
 *          exit(1);
 *        }
 *
 *        gdWebpAnimWriteOptionsInit(&options);
 *        options.canvasWidth = info.width;
 *        options.canvasHeight = info.height;
 *        options.loopCount = info.loopCount;
 *        options.backgroundColor = info.backgroundColor;
 *        options.quality = gdWebpLossless;
 *
 *        out = fopen("output.webp", "wb");
 *        if (out == NULL) {
 *          gdWebpReadClose(reader);
 *          exit(1);
 *        }
 *        writer = gdWebpWriteOpen(out, &options);
 *        if (writer == NULL) {
 *          fclose(out);
 *          gdWebpReadClose(reader);
 *          exit(1);
 *        }
 *
 *        while ((result = gdWebpReadNextImage(reader, &frameInfo, &image)) == 1) {
 *          if (!gdWebpWriteAddImage(writer, image, frameInfo.duration)) {
 *            gdImageDestroy(image);
 *            gdWebpWriteClose(writer);
 *            fclose(out);
 *            gdWebpReadClose(reader);
 *            exit(1);
 *          }
 *          gdImageDestroy(image);
 *        }
 *
 *        gdWebpWriteClose(writer);
 *        fclose(out);
 *        gdWebpReadClose(reader);
 * @endcode
 *
 * @{
 */

/** @name Single-Image Reading */
/** @{ */

/**
 * @brief Create a truecolor image from a WebP stdio file.
 *
 * gdImageCreateFromWebp() does not close inFile. The returned image is
 * caller-owned and must be destroyed with @ref gdImageDestroy.
 *
 * @param inFile Pointer to the input FILE stream.
 *
 * @return Returns a gdImagePtr on success, or NULL on failure.
 */
BGD_DECLARE(gdImagePtr) gdImageCreateFromWebp(FILE *inFile);

/**
 * @brief Create a truecolor image from a WebP memory buffer.
 *
 * The data buffer is borrowed for the duration of the call. The returned image
 * is caller-owned and must be destroyed with @ref gdImageDestroy.
 *
 * @param size Size of the WebP memory buffer in bytes.
 * @param data Pointer to the WebP memory buffer.
 *
 * @return Returns a gdImagePtr on success, or NULL on failure.
 */
BGD_DECLARE(gdImagePtr) gdImageCreateFromWebpPtr(int size, void *data);

/**
 * @brief Create a truecolor image from WebP data read through a gdIOCtx.
 *
 * gdImageCreateFromWebpCtx() does not close infile. The returned image is
 * caller-owned and must be destroyed with @ref gdImageDestroy.
 *
 * @param infile Pointer to the gdIOCtx input context.
 *
 * @return Returns a gdImagePtr on success, or NULL on failure.
 */
BGD_DECLARE(gdImagePtr) gdImageCreateFromWebpCtx(gdIOCtxPtr infile);

/** @} */

/** @name WebP Types And Constants */
/** @{ */

/**
 * @brief Opaque WebP animation reader handle.
 *
 * Handles returned by gdWebpReadOpen(), gdWebpReadOpenCtx(), or
 * gdWebpReadOpenPtr() must be closed with gdWebpReadClose().
 */
typedef struct gdWebpRead *gdWebpReadPtr;

/**
 * @brief Opaque WebP animation writer handle.
 *
 * Handles returned by gdWebpWriteOpen() or gdWebpWriteOpenCtx() must be closed
 * with gdWebpWriteClose(). Handles returned by gdWebpWriteOpenPtr() must be
 * finished with gdWebpWritePtrFinish().
 */
typedef struct gdWebpWrite *gdWebpWritePtr;

/**
 * @brief WebP container information.
 */
typedef struct {
    int width;           /**< Canvas width in pixels. */
    int height;          /**< Canvas height in pixels. */
    int frame_count;     /**< Number of frames in the WebP container. */
    int loop_count;      /**< Animation loop count, or 0 for infinite looping. */
    int background_color; /**< Canvas background color as stored in the WebP container. */
    int format_flags;    /**< WebP container feature flags reported by libwebp. */
    int is_animation;    /**< Non-zero if the WebP container is animated. */
} gdWebpInfo;

/**
 * @brief WebP animation frame information.
 */
typedef struct {
    int frame_index; /**< Zero-based frame index. */
    int x;          /**< Frame rectangle X offset in pixels. */
    int y;          /**< Frame rectangle Y offset in pixels. */
    int width;      /**< Frame rectangle width in pixels. */
    int height;     /**< Frame rectangle height in pixels. */
    int duration;   /**< Frame duration in milliseconds. */
    int timestamp;  /**< Frame start timestamp in milliseconds. */
    int dispose;    /**< Disposal method, gdWebpDisposeNone or gdWebpDisposeBackground. */
    int blend;      /**< Blend method, gdWebpBlendAlpha or gdWebpBlendNone. */
    int has_alpha;  /**< Non-zero if the frame has alpha. */
    int complete;   /**< Non-zero if the frame data is complete. */
} gdWebpFrameInfo;

/**
 * @brief WebP multi-image/animation reader options.
 */
typedef struct {
    int coalesced;      /**< Non-zero to read full-canvas images, zero to read raw frame rectangles. */
} gdWebpReadOptions;

/**
 * @brief WebP still-image writer options.
 */
typedef struct {
    int quality;                     /**< Encoding quality, -1 for default, 0-100 for lossy, or gdWebpLossless. */
    const gdImageMetadata *metadata; /**< Optional metadata to embed in the WebP container. */
} gdWebpWriteOptions;

/**
 * @brief WebP animation writer options.
 */
typedef struct {
    int canvas_width;     /**< Canvas width in pixels, or 0 to use the first image width. */
    int canvas_height;    /**< Canvas height in pixels, or 0 to use the first image height. */
    int loop_count;       /**< Animation loop count, or 0 for infinite looping. */
    int background_color; /**< Canvas background color to store in the WebP container. */
    int quality;         /**< Encoding quality, -1 for default, 0-100 for lossy, or gdWebpLossless. */
    int lossless;        /**< Non-zero to force lossless encoding. */
    int method;          /**< Compression method, or a negative value to use libwebp default. */
    int minimize_size;    /**< Non-zero to enable libwebp minimized-size animation encoding. */
    int kmin;            /**< Minimum distance between key frames, or 0 for libwebp default. */
    int kmax;            /**< Maximum distance between key frames, or 0 for libwebp default. */
    int allow_mixed;      /**< Non-zero to allow mixed lossy and lossless frames. */
} gdWebpAnimWriteOptions;

/**
 * @brief Initialize WebP multi-image/animation read options with gd defaults.
 *
 * The default reader mode is coalesced, so gdWebpReadNextImage() returns
 * full-canvas rendered images. Set gdWebpReadOptions::coalesced to zero before
 * opening the reader to read raw frame rectangles with gdWebpReadNextFrame().
 *
 * @param options Pointer to the read options structure to initialize.
 */
BGD_DECLARE(void) gdWebpReadOptionsInit(gdWebpReadOptions *options);

/**
 * @brief Initialize WebP still-image write options with gd defaults.
 *
 * The default writer uses libwebp's default quality and writes no metadata.
 *
 * @param options Pointer to the write options structure to initialize.
 */
BGD_DECLARE(void) gdWebpWriteOptionsInit(gdWebpWriteOptions *options);

/**
 * @brief Initialize WebP multi-image/animation write options with gd defaults.
 *
 * The default writer infers the canvas size from the first frame, writes lossy
 * WebP with libwebp's default animation settings, and uses loopCount 0 for
 * infinite looping.
 *
 * @param options Pointer to the write options structure to initialize.
 */
BGD_DECLARE(void) gdWebpAnimWriteOptionsInit(gdWebpAnimWriteOptions *options);

/**
 * @brief WebP frame disposal methods.
 */
enum {
    gdWebpDisposeNone,      /**< Do not dispose the frame after display. */
    gdWebpDisposeBackground /**< Clear the frame rectangle to the background after display. */
};

/**
 * @brief WebP frame blend methods.
 */
enum {
    gdWebpBlendAlpha, /**< Blend the frame using alpha compositing. */
    gdWebpBlendNone   /**< Replace the frame rectangle without alpha blending. */
};

/** @} */

/** @name WebP Multi-Image/Animation Reading */
/** @{ */

/**
 * @brief Test whether a WebP stdio file contains animation.
 *
 * The stream position is restored before returning when possible. gdWebpIsAnimated()
 * does not close fd.
 *
 * @param fd Pointer to the input FILE stream.
 *
 * @return Returns 1 for animated WebP, 0 for still WebP, or -1 on error.
 */
BGD_DECLARE(int) gdWebpIsAnimated(FILE *fd);

/**
 * @brief Test whether a seekable gdIOCtx contains animated WebP data.
 *
 * The context position is restored before returning when possible.
 * gdWebpIsAnimatedCtx() does not close in.
 *
 * @param in Pointer to the gdIOCtx input context.
 *
 * @return Returns 1 for animated WebP, 0 for still WebP, or -1 on error.
 */
BGD_DECLARE(int) gdWebpIsAnimatedCtx(gdIOCtxPtr in);

/**
 * @brief Test whether a WebP memory buffer contains animation.
 *
 * The data buffer is borrowed for the duration of the call.
 *
 * @param size Size of the WebP memory buffer in bytes.
 * @param data Pointer to the WebP memory buffer.
 *
 * @return Returns 1 for animated WebP, 0 for still WebP, or -1 on error.
 */
BGD_DECLARE(int) gdWebpIsAnimatedPtr(int size, void *data);

/**
 * @brief Open a WebP animation reader from a stdio file.
 *
 * gdWebpReadOpen() reads the WebP data into the reader and does not close fd.
 * Pass NULL for options to use gd defaults. The returned handle must be closed
 * with gdWebpReadClose().
 *
 * @param fd Pointer to the input FILE stream.
 * @param options Pointer to read options, or NULL for defaults.
 *
 * @return Returns a WebP reader handle on success, or NULL on failure.
 */
BGD_DECLARE(gdWebpReadPtr) gdWebpReadOpen(FILE *fd, const gdWebpReadOptions *options);

/**
 * @brief Open a WebP animation reader from a gdIOCtx.
 *
 * gdWebpReadOpenCtx() reads the WebP data into the reader and does not close
 * in. Pass NULL for options to use gd defaults. The returned handle must be
 * closed with gdWebpReadClose().
 *
 * @param in Pointer to the gdIOCtx input context.
 * @param options Pointer to read options, or NULL for defaults.
 *
 * @return Returns a WebP reader handle on success, or NULL on failure.
 */
BGD_DECLARE(gdWebpReadPtr) gdWebpReadOpenCtx(gdIOCtxPtr in, const gdWebpReadOptions *options);

/**
 * @brief Open a WebP animation reader from a memory buffer.
 *
 * The data buffer is borrowed for the duration of the call. The returned
 * handle owns its copy of the WebP data and must be closed with
 * gdWebpReadClose(). Pass NULL for options to use gd defaults.
 *
 * @param size Size of the WebP memory buffer in bytes.
 * @param data Pointer to the WebP memory buffer.
 * @param options Pointer to read options, or NULL for defaults.
 *
 * @return Returns a WebP reader handle on success, or NULL on failure.
 */
BGD_DECLARE(gdWebpReadPtr)
gdWebpReadOpenPtr(int size, void *data, const gdWebpReadOptions *options);

/**
 * @brief Close a WebP animation reader.
 *
 * @param webp WebP reader handle to close, or NULL.
 */
BGD_DECLARE(void) gdWebpReadClose(gdWebpReadPtr webp);

/**
 * @brief Get WebP container information from a WebP reader.
 *
 * @param webp WebP reader handle.
 * @param info Pointer to a gdWebpInfo structure to receive container information.
 *
 * @return Returns 1 on success, or 0 on failure.
 */
BGD_DECLARE(int) gdWebpReadGetInfo(gdWebpReadPtr webp, gdWebpInfo *info);

/**
 * @brief Extract opaque EXIF, XMP, and ICC metadata from a WebP reader.
 *
 * @param webp The WebP reader.
 * @param metadata Metadata object to populate.
 * @return GD_META_OK on success, or a GD_META_ERR_* value on failure.
 */
BGD_DECLARE(int) gdWebpReadGetMetadata(gdWebpReadPtr webp, gdImageMetadata *metadata);

/**
 * @brief Read the next raw WebP animation frame rectangle.
 *
 * When frame is not NULL and the function returns 1, *frame receives a
 * caller-owned truecolor image that must be destroyed with @ref gdImageDestroy.
 * Passing NULL for frame advances the reader without returning the image.
 *
 * @param webp WebP reader handle opened with raw-frame mode.
 * @param info Pointer to a gdWebpFrameInfo structure to receive frame information, or NULL.
 * @param frame Pointer to receive the caller-owned frame image, or NULL.
 *
 * @return Returns 1 when a frame is read, 0 at end of animation, or -1 on error.
 */
BGD_DECLARE(int)
gdWebpReadNextFrame(gdWebpReadPtr webp, gdWebpFrameInfo *info, gdImagePtr *frame);

/**
 * @brief Read the next coalesced WebP animation image.
 *
 * When image is not NULL and the function returns 1, *image receives a
 * caller-owned truecolor full-canvas image that must be destroyed with
 * @ref gdImageDestroy. Passing NULL for image advances the reader without
 * returning the image.
 *
 * @param webp WebP reader handle.
 * @param info Pointer to a gdWebpFrameInfo structure to receive frame information, or NULL.
 * @param image Pointer to receive the caller-owned full-canvas image, or NULL.
 *
 * @return Returns 1 when an image is read, 0 at end of animation, or -1 on error.
 */
BGD_DECLARE(int)
gdWebpReadNextImage(gdWebpReadPtr webp, gdWebpFrameInfo *info, gdImagePtr *image);

/** @} */

/** @name WebP Multi-Image/Animation Writing */
/** @{ */

/**
 * @brief Open a WebP animation writer for a stdio file.
 *
 * gdWebpWriteOpen() does not close outFile. The returned handle must be closed
 * with gdWebpWriteClose(), which assembles and writes the animation.
 *
 * @param outFile Pointer to the output FILE stream.
 * @param options Pointer to write options, or NULL for defaults.
 *
 * @return Returns a WebP writer handle on success, or NULL on failure.
 */
BGD_DECLARE(gdWebpWritePtr)
gdWebpWriteOpen(FILE *outFile, const gdWebpAnimWriteOptions *options);

/**
 * @brief Open a WebP animation writer for a gdIOCtx.
 *
 * The output context is borrowed and is not closed by gdWebpWriteClose().
 * The returned handle must be closed with gdWebpWriteClose(), which assembles
 * and writes the animation.
 *
 * @param out Pointer to the gdIOCtx output context.
 * @param options Pointer to write options, or NULL for defaults.
 *
 * @return Returns a WebP writer handle on success, or NULL on failure.
 */
BGD_DECLARE(gdWebpWritePtr)
gdWebpWriteOpenCtx(gdIOCtxPtr out, const gdWebpAnimWriteOptions *options);

/**
 * @brief Open a WebP animation writer that returns a memory buffer.
 *
 * The returned handle must be finished with gdWebpWritePtrFinish().
 *
 * @param options Pointer to write options, or NULL for defaults.
 *
 * @return Returns a WebP memory writer handle on success, or NULL on failure.
 */
BGD_DECLARE(gdWebpWritePtr)
gdWebpWriteOpenPtr(const gdWebpAnimWriteOptions *options);

/**
 * @brief Add an image to a WebP animation writer.
 *
 * The image is borrowed for the duration of the call and remains owned by the
 * caller. All frames must match the writer canvas size.
 *
 * @param webp WebP writer handle.
 * @param image Image to add as the next frame.
 * @param durationMs Frame duration in milliseconds.
 *
 * @return Returns 1 on success, or 0 on failure.
 */
BGD_DECLARE(int)
gdWebpWriteAddImage(gdWebpWritePtr webp, gdImagePtr image, int durationMs);

/**
 * @brief Finish, write, and close a WebP animation writer.
 *
 * Use this for handles returned by gdWebpWriteOpen() or gdWebpWriteOpenCtx().
 * For memory writers returned by gdWebpWriteOpenPtr(), use gdWebpWritePtrFinish().
 *
 * @param webp WebP writer handle to finish and close, or NULL.
 */
BGD_DECLARE(void) gdWebpWriteClose(gdWebpWritePtr webp);

/**
 * @brief Finish a WebP memory writer and return the encoded buffer.
 *
 * This closes webp whether encoding succeeds or fails. The returned buffer is
 * caller-owned and must be freed with gdFree().
 *
 * @param webp WebP memory writer handle returned by gdWebpWriteOpenPtr().
 * @param size Pointer to an integer that receives the returned buffer size.
 *
 * @return Returns a pointer to the newly allocated WebP buffer, or NULL on failure.
 */
BGD_DECLARE(void *) gdWebpWritePtrFinish(gdWebpWritePtr webp, int *size);

/** @} */
/** @} */

/**
 * @defgroup gdCodecJxl JPEG XL
 * @brief JPEG XL image reading, writing, and animation support.
 * @ingroup gdCodecs
 *
 * JPEG XL support reads still images as truecolor gd images and writes still
 * images with either lossy distance settings or lossless encoding. Animation
 * readers can return coalesced full-canvas images or raw frame rectangles, and
 * animation writers accept full-canvas truecolor frames.
 *
 * @code{.c}
 *        gdImagePtr first, second, image;
 *        gdJxlAnimWriteOptions write_options;
 *        gdJxlWritePtr writer;
 *        gdJxlReadPtr reader;
 *        void *data;
 *        int size, delay_ms, result;
 *
 *        first = gdImageCreateTrueColor(32, 24);
 *        second = gdImageCreateTrueColor(32, 24);
 *        if (first == NULL || second == NULL) {
 *          exit(1);
 *        }
 *
 *        gdImageFilledRectangle(first, 0, 0, 31, 23, gdTrueColor(255, 0, 0));
 *        gdImageFilledRectangle(second, 0, 0, 31, 23, gdTrueColor(0, 0, 255));
 *
 *        gdJxlAnimWriteOptionsInit(&write_options);
 *        write_options.lossless = 1;
 *        write_options.loopCount = 0;
 *        writer = gdJxlWriteOpenPtr(&write_options);
 *        if (writer == NULL) {
 *          gdImageDestroy(first);
 *          gdImageDestroy(second);
 *          exit(1);
 *        }
 *        gdJxlWriteAddImage(writer, first, 120);
 *        gdJxlWriteAddImage(writer, second, 80);
 *        data = gdJxlWritePtrFinish(writer, &size);
 *
 *        reader = gdJxlReadOpenPtr(size, data, NULL);
 *        while ((result = gdJxlReadNextImage(reader, &delay_ms, &image)) == 1) {
 *          gdImageDestroy(image);
 *        }
 *        gdJxlReadClose(reader);
 *        gdFree(data);
 *        gdImageDestroy(first);
 *        gdImageDestroy(second);
 * @endcode
 *
 * @{
 */

/** @name JPEG XL Single-Image Reading */
/** @{ */

/**
 * @brief Create a truecolor image from a JPEG XL stdio file.
 *
 * gdImageCreateFromJxl() does not close inFile. The returned image is
 * caller-owned and must be destroyed with @ref gdImageDestroy.
 *
 * @param inFile Pointer to the input FILE stream.
 *
 * @return Returns a gdImagePtr on success, or NULL on failure.
 */
BGD_DECLARE(gdImagePtr) gdImageCreateFromJxl(FILE *inFile);

/**
 * @brief Create a truecolor image from a JPEG XL memory buffer.
 *
 * The data buffer is borrowed for the duration of the call. The returned image
 * is caller-owned and must be destroyed with @ref gdImageDestroy.
 *
 * @param size Size of the JPEG XL memory buffer in bytes.
 * @param data Pointer to the JPEG XL memory buffer.
 *
 * @return Returns a gdImagePtr on success, or NULL on failure.
 */
BGD_DECLARE(gdImagePtr) gdImageCreateFromJxlPtr(int size, void *data);

/**
 * @brief Create a truecolor image from JPEG XL data read through a gdIOCtx.
 *
 * gdImageCreateFromJxlCtx() does not close infile. The returned image is
 * caller-owned and must be destroyed with @ref gdImageDestroy.
 *
 * @param infile Pointer to the gdIOCtx input context.
 *
 * @return Returns a gdImagePtr on success, or NULL on failure.
 */
BGD_DECLARE(gdImagePtr) gdImageCreateFromJxlCtx(gdIOCtxPtr infile);

/** @} */

/** @name JPEG XL Single-Image Writing */
/** @{ */

/**
 * @brief Write an image as JPEG XL data to a stdio file.
 *
 * gdImageJxl() does not close outFile. The image is borrowed for the duration
 * of the call; palette images may be converted to truecolor internally.
 *
 * @param im The image to write.
 * @param outFile Pointer to the output FILE stream.
 */
BGD_DECLARE(void) gdImageJxl(gdImagePtr im, FILE *outFile);

/**
 * @brief Write an image as JPEG XL data to a stdio file with encoder settings.
 *
 * gdImageJxlEx() does not close outFile. The image is borrowed for the
 * duration of the call; palette images may be converted to truecolor
 * internally.
 *
 * @param im The image to write.
 * @param outFile Pointer to the output FILE stream.
 * @param lossless Non-zero to use lossless JPEG XL encoding.
 * @param distance Lossy encoding distance when lossless is zero.
 * @param effort Encoder effort setting.
 */
BGD_DECLARE(void)
gdImageJxlEx(gdImagePtr im, FILE *outFile, int lossless, float distance, int effort);

/**
 * @brief Write an image as JPEG XL data to a newly allocated memory buffer.
 *
 * The image is borrowed for the duration of the call; palette images may be
 * converted to truecolor internally. The returned buffer is caller-owned and
 * must be freed with gdFree().
 *
 * @param im The image to write.
 * @param size Pointer to an integer that receives the returned buffer size.
 *
 * @return Returns a pointer to the newly allocated JPEG XL buffer, or NULL on failure.
 */
BGD_DECLARE(void *) gdImageJxlPtr(gdImagePtr im, int *size);

/**
 * @brief Write an image as JPEG XL data to a newly allocated memory buffer with encoder settings.
 *
 * The image is borrowed for the duration of the call; palette images may be
 * converted to truecolor internally. The returned buffer is caller-owned and
 * must be freed with gdFree().
 *
 * @param im The image to write.
 * @param size Pointer to an integer that receives the returned buffer size.
 * @param lossless Non-zero to use lossless JPEG XL encoding.
 * @param distance Lossy encoding distance when lossless is zero.
 * @param effort Encoder effort setting.
 *
 * @return Returns a pointer to the newly allocated JPEG XL buffer, or NULL on failure.
 */
BGD_DECLARE(void *)
gdImageJxlPtrEx(gdImagePtr im, int *size, int lossless, float distance, int effort);

/**
 * @brief Write an image as JPEG XL data to a gdIOCtx.
 *
 * gdImageJxlCtx() does not close outfile. The image is borrowed for the
 * duration of the call; palette images may be converted to truecolor
 * internally.
 *
 * @param im The image to write.
 * @param outfile Pointer to the gdIOCtx output context.
 */
BGD_DECLARE(void) gdImageJxlCtx(gdImagePtr im, gdIOCtxPtr outfile);

/**
 * @brief Write an image as JPEG XL data to a gdIOCtx with encoder settings.
 *
 * gdImageJxlCtxEx() does not close outfile. The image is borrowed for the
 * duration of the call; palette images may be converted to truecolor
 * internally.
 *
 * @param im The image to write.
 * @param outfile Pointer to the gdIOCtx output context.
 * @param lossless Non-zero to use lossless JPEG XL encoding.
 * @param distance Lossy encoding distance when lossless is zero.
 * @param effort Encoder effort setting.
 */
BGD_DECLARE(void)
gdImageJxlCtxEx(gdImagePtr im, gdIOCtxPtr outfile, int lossless, float distance, int effort);

/**
 * @brief JPEG XL still-image writer options.
 */
typedef struct {
    int lossless;                         /**< Non-zero for lossless encoding. */
    float distance;                       /**< Lossy distance, from 0 through 25. */
    int effort;                           /**< Encoder effort, from 1 through 9. */
    const gdImageMetadata *metadata;      /**< Optional EXIF/XMP metadata. */
} gdJxlWriteOptions;

/**
 * @brief Initialize JPEG XL still-image write options with gd defaults.
 * 
 * @param options Pointer to the write options structure to initialize.
 */
BGD_DECLARE(void) gdJxlWriteOptionsInit(gdJxlWriteOptions *options);

/**
 * @brief Write an image as JPEG XL data to a stdio file with write options.
 *
 * @param im The image to write.
 * @param outFile Pointer to the output FILE stream.
 * @param options Pointer to the JPEG XL write options.
 *
 * @return Returns non-zero on success, or zero on failure.
 */
BGD_DECLARE(int)
gdImageJxlWithOptions(gdImagePtr im, FILE *outFile, const gdJxlWriteOptions *options);

/**
 * @brief Write an image as JPEG XL data to a gdIOCtx with write options.
 *
 * @param im The image to write.
 * @param outfile Pointer to the gdIOCtx output context.
 * @param options Pointer to the JPEG XL write options.
 *
 * @return Returns non-zero on success, or zero on failure.
 */
BGD_DECLARE(int)
gdImageJxlCtxWithOptions(gdImagePtr im, gdIOCtxPtr outfile, const gdJxlWriteOptions *options);

/**
 * @brief Write an image as JPEG XL data to a newly allocated memory buffer with write options.
 * 
 * The image is borrowed for the duration of the call; palette images may be
 * converted to truecolor internally. The returned buffer is caller-owned and
 * must be freed with gdFree().
 * 
 * @param im The image to write.
 * @param size Pointer to an integer that receives the returned buffer size.
 * @param options Pointer to the JPEG XL write options.
 *
 * @return Returns a pointer to the newly allocated memory buffer, or NULL on failure.
 */
BGD_DECLARE(void *)
gdImageJxlPtrWithOptions(gdImagePtr im, int *size, const gdJxlWriteOptions *options);

/** @} */

/** @name JPEG XL File Information And Animation Types */
/** @{ */

/**
 * @brief Opaque JPEG XL multi-image/animation reader handle.
 *
 * Handles returned by @ref gdJxlReadOpen, @ref gdJxlReadOpenCtx(), or
 * @ref gdJxlReadOpenPtr must be closed with @ref gdJxlReadClose.
 */
typedef struct gdJxlRead *gdJxlReadPtr;

/**
 * @brief Opaque JPEG XL multi-image/animation writer handle.
 *
 * Handles returned by @ref gdJxlWriteOpen or @ref gdJxlWriteOpenCtx must be closed
 * with @ref gdJxlWriteClose. Handles returned by @ref gdJxlWriteOpenPtr must be
 * finished with @ref gdJxlWritePtrFinish.
 */
typedef struct gdJxlWrite *gdJxlWritePtr;

/**
 * @brief JPEG XL top-level file information.
 *
 * This describes the complete JPEG XL file, including whether it contains
 * animation. It is not an animation-only structure.
 */
typedef struct {
    int width;      /**< Canvas width in pixels. */
    int height;     /**< Canvas height in pixels. */
    int animated;   /**< Non-zero if the JPEG XL stream is animated. */
    int loop_count; /**< Animation loop count, or 0 for infinite looping. */
} gdJxlInfo;

/**
 * @brief JPEG XL raw frame information.
 */
typedef struct {
    int delay_ms;   /**< Frame duration in milliseconds. */
    int x_offset;   /**< Frame rectangle X offset in pixels. */
    int y_offset;   /**< Frame rectangle Y offset in pixels. */
    int width;      /**< Frame rectangle width in pixels. */
    int height;     /**< Frame rectangle height in pixels. */
    int blend_mode; /**< Frame blend mode, one of the gdJxlBlend* constants. */
    int is_last;    /**< Non-zero if this is the final frame. */
} gdJxlFrameInfo;

/**
 * @brief JPEG XL raw frame blend modes.
 */
enum {
    gdJxlBlendReplace, /**< Replace the frame rectangle with the new frame. */
    gdJxlBlendAdd,     /**< Add the new frame to the existing canvas. */
    gdJxlBlendBlend,   /**< Blend the new frame over the existing canvas. */
    gdJxlBlendMuladd,  /**< Multiply then add the new frame with the existing canvas. */
    gdJxlBlendMul      /**< Multiply the new frame with the existing canvas. */
};

/**
 * @brief JPEG XL multi-image/animation reader options.
 */
typedef struct {
    int coalesced;      /**< Non-zero to read full-canvas images, zero to read raw frame rectangles. */
} gdJxlReadOptions;

/**
 * @brief JPEG XL animation writer options.
 */
typedef struct {
    int canvas_width;    /**< Canvas width in pixels, or 0 to use the first image width. */
    int canvas_height;   /**< Canvas height in pixels, or 0 to use the first image height. */
    int lossless;       /**< Non-zero to use lossless JPEG XL encoding. */
    float distance;     /**< Lossy encoding distance when lossless is zero. */
    int effort;         /**< Encoder effort setting. */
    int loop_count;      /**< Animation loop count, or 0 for infinite looping. */
} gdJxlAnimWriteOptions;

/**
 * @brief Initialize JPEG XL multi-image/animation read options with gd defaults.
 *
 * The default reader mode is coalesced, so @ref gdJxlReadNextImage returns
 * full-canvas rendered images. Set @ref gdJxlReadOptions::coalesced to zero before
 * opening the reader to read raw frame rectangles with @ref gdJxlReadNextFrame.
 *
 * @param options Pointer to the read options structure to initialize.
 */
BGD_DECLARE(void) gdJxlReadOptionsInit(gdJxlReadOptions *options);

/**
 * @brief Initialize JPEG XL multi-image/animation write options with gd defaults.
 *
 * The default writer infers the canvas size from the first frame, writes lossy
 * JPEG XL with distance 1.0 and effort 7, and uses loopCount 0 for infinite
 * looping.
 *
 * @param options Pointer to the write options structure to initialize.
 */
BGD_DECLARE(void) gdJxlAnimWriteOptionsInit(gdJxlAnimWriteOptions *options);

/** @} */

/** @name JPEG XL Multi-Image/Animation Reading */
/** @{ */

/**
 * @brief Open a JPEG XL multi-image/animation reader from a stdio file.
 *
 * gdJxlReadOpen() reads the JPEG XL data into the reader and does not close
 * inFile. Pass NULL for options to use gd defaults. The returned handle must be
 * closed with gdJxlReadClose().
 *
 * @param inFile Pointer to the input FILE stream.
 * @param options Pointer to read options, or NULL for defaults.
 *
 * @return Returns a JPEG XL reader handle on success, or NULL on failure.
 */
BGD_DECLARE(gdJxlReadPtr) gdJxlReadOpen(FILE *inFile, const gdJxlReadOptions *options);

/**
 * @brief Open a JPEG XL multi-image/animation reader from a gdIOCtx.
 *
 * gdJxlReadOpenCtx() reads the JPEG XL data into the reader and does not close
 * inCtx. Pass NULL for options to use gd defaults. The returned handle must be
 * closed with gdJxlReadClose().
 *
 * @param inCtx Pointer to the gdIOCtx input context.
 * @param options Pointer to read options, or NULL for defaults.
 *
 * @return Returns a JPEG XL reader handle on success, or NULL on failure.
 */
BGD_DECLARE(gdJxlReadPtr) gdJxlReadOpenCtx(gdIOCtxPtr inCtx, const gdJxlReadOptions *options);

/**
 * @brief Open a JPEG XL multi-image/animation reader from a memory buffer.
 *
 * The data buffer is borrowed for the duration of the call. Pass NULL for
 * options to use gd defaults. The returned handle owns its copy of the JPEG XL
 * data and must be closed with @ref gdJxlReadClose.
 *
 * @param size Size of the JPEG XL memory buffer in bytes.
 * @param data Pointer to the JPEG XL memory buffer.
 * @param options Pointer to read options, or NULL for defaults.
 *
 * @return Returns a JPEG XL reader handle on success, or NULL on failure.
 */
BGD_DECLARE(gdJxlReadPtr)
gdJxlReadOpenPtr(int size, void *data, const gdJxlReadOptions *options);

/**
 * @brief Get JPEG XL image or animation information from a reader.
 *
 * The returned gdJxlInfo describes the canvas size and animation loop count
 * reported by the JPEG XL stream.
 *
 * @param reader JPEG XL reader handle.
 * @param info Pointer to a gdJxlInfo structure to receive image information.
 *
 * @return Returns 1 on success, or 0 on failure.
 */
BGD_DECLARE(int) gdJxlReadGetInfo(gdJxlReadPtr reader, gdJxlInfo *info);

/**
 * @brief Extract supported still-image metadata from a JPEG XL reader.
 *
 * The caller owns the metadata object. Animation metadata is not exposed.
 */
BGD_DECLARE(int) gdJxlReadGetMetadata(gdJxlReadPtr reader, gdImageMetadata *metadata);

/**
 * @brief Read the next coalesced JPEG XL image.
 *
 * This function is used with coalesced readers. When image is not NULL and the
 * function returns 1, *image receives a caller-owned full-canvas truecolor image
 * that must be destroyed with @ref gdImageDestroy. Passing NULL for image advances
 * the reader without returning the decoded image.
 *
 * @param reader JPEG XL reader handle opened with coalesced mode.
 * @param delay_ms Pointer to receive the frame duration in milliseconds, or NULL.
 * @param image Pointer to receive the caller-owned image, or NULL.
 *
 * @return Returns 1 when an image is read, 0 at end of stream, or -1 on error.
 */
BGD_DECLARE(int) gdJxlReadNextImage(gdJxlReadPtr reader, int *delay_ms, gdImagePtr *image);

/**
 * @brief Read the next raw JPEG XL frame rectangle.
 *
 * This function is used with non-coalesced readers. When frame is not NULL and
 * the function returns 1, *frame receives a caller-owned truecolor frame
 * rectangle that must be destroyed with @ref gdImageDestroy. Passing NULL for frame
 * advances the reader without returning the decoded image.
 *
 * @param reader JPEG XL reader handle opened with raw-frame mode.
 * @param info Pointer to receive raw frame information.
 * @param frame Pointer to receive the caller-owned frame image, or NULL.
 *
 * @return Returns 1 when a frame is read, 0 at end of stream, or -1 on error.
 */
BGD_DECLARE(int) gdJxlReadNextFrame(gdJxlReadPtr reader, gdJxlFrameInfo *info, gdImagePtr *frame);

/**
 * @brief Close a JPEG XL multi-image reader.
 *
 * @param reader JPEG XL reader handle to close, or NULL.
 */
BGD_DECLARE(void) gdJxlReadClose(gdJxlReadPtr reader);

/** @} */

/** @name JPEG XL Multi-Image/Animation Writing */
/** @{ */

/**
 * @brief Open a JPEG XL multi-image/animation writer for a stdio file.
 *
 * @ref gdJxlWriteOpen does not close outFile. Pass NULL for options to use gd defaults.
 * The returned handle must be closed with @ref gdJxlWriteClose.
 *
 * @param outFile Pointer to the output FILE stream.
 * @param options Pointer to write options, or NULL for defaults.
 *
 * @return Returns a JPEG XL writer handle on success, or NULL on failure.
 */
BGD_DECLARE(gdJxlWritePtr) gdJxlWriteOpen(FILE *outFile, const gdJxlAnimWriteOptions *options);

/**
 * @brief Open a JPEG XL multi-image/animation writer for a gdIOCtx.
 *
 * The output context is borrowed and is not closed by @ref gdJxlWriteClose. Pass
 * NULL for options to use gd defaults. The returned handle must be closed with
 * @ref gdJxlWriteClose.
 *
 * @param outCtx Pointer to the gdIOCtx output context.
 * @param options Pointer to write options, or NULL for defaults.
 *
 * @return Returns a JPEG XL writer handle on success, or NULL on failure.
 */
BGD_DECLARE(gdJxlWritePtr) gdJxlWriteOpenCtx(gdIOCtxPtr outCtx, const gdJxlAnimWriteOptions *options);

/**
 * @brief Open a JPEG XL multi-image/animation writer that returns a memory buffer.
 *
 * Pass NULL for options to use gd defaults. The returned handle must be finished
 * with @ref gdJxlWritePtrFinish.
 *
 * @param options Pointer to write options, or NULL for defaults.
 *
 * @return Returns a JPEG XL memory writer handle on success, or NULL on failure.
 */
BGD_DECLARE(gdJxlWritePtr) gdJxlWriteOpenPtr(const gdJxlAnimWriteOptions *options);

/**
 * @brief Add a full-canvas image to a JPEG XL multi-image/animation writer.
 *
 * The image is borrowed for the duration of the call and remains owned by the
 * caller. The image must be truecolor. All images must match the resolved
 * canvas size; when the writer canvas is zero, the first image sets it.
 *
 * @param writer JPEG XL writer handle.
 * @param image Image to add as the next frame.
 * @param delay_ms Frame duration in milliseconds.
 *
 * @return Returns 1 on success, or 0 on failure.
 */
BGD_DECLARE(int) gdJxlWriteAddImage(gdJxlWritePtr writer, gdImagePtr image, int delay_ms);

/**
 * @brief Add a full-canvas image to a JPEG XL multi-image/animation writer.
 *
 * Use this for handles returned by gdJxlWriteOpen() or gdJxlWriteOpenCtx(). For
 * memory writers returned by @ref gdJxlWriteOpenPtr, use @ref gdJxlWritePtrFinish.
 *
 * @param writer JPEG XL writer handle to finish and close, or NULL.
 */
BGD_DECLARE(void) gdJxlWriteClose(gdJxlWritePtr writer);

/**
 * @brief Finish a JPEG XL multi-image/animation memory writer and return the encoded buffer.
 *
 * This closes writer whether encoding succeeds or fails. The returned buffer is
 * caller-owned and must be freed with gdFree().
 *
 * @param writer JPEG XL memory writer handle returned by @ref gdJxlWriteOpenPtr.
 * @param size Pointer to an integer that receives the returned buffer size.
 *
 * @return Returns a pointer to the newly allocated JPEG XL buffer, or NULL on failure.
 */
BGD_DECLARE(void *) gdJxlWritePtrFinish(gdJxlWritePtr writer, int *size);

/** @} */
/** @} */

/**
 * @defgroup gdCodecHeif HEIF
 * @brief Read and write High Efficiency Image File Format images.
 * @ingroup gdCodecs
 *
 * HEIF support reads HEIF-family files from stdio streams, memory buffers, or
 * gd IO contexts and returns truecolor images. The reader accepts AVIF, MIF1,
 * HEIC, and HEIX brands and decodes the primary image. HEIF writing accepts
 * truecolor images and can write to stdio streams, memory buffers, or gd IO
 * contexts with explicit codec, quality, lossless, and chroma-subsampling
 * options.
 *
 * @code{.c}
 * FILE *in;
 * gdImagePtr im;
 * gdHeifWriteOptions options;
 * void *data;
 * int size;
 *
 * in = fopen("input.heic", "rb");
 * if (in == NULL) {
 *     return 1;
 * }
 *
 * im = gdImageCreateFromHeif(in);
 * fclose(in);
 * if (im == NULL) {
 *     return 1;
 * }
 *
 * gdHeifWriteOptionsInit(&options);
 * options.quality = 90;
 * options.codec = GD_HEIF_CODEC_HEVC;
 * options.chroma = GD_HEIF_CHROMA_444;
 *
 * data = gdImageHeifPtrWithOptions(im, &size, &options);
 * if (data != NULL) {
 *     gdFree(data);
 * }
 *
 * gdImageDestroy(im);
 * @endcode
 *
 * @{
 */

/** @name HEIF Constants and Options */
/** @{ */

/** @brief HEIF coding formats for gdHeifWriteOptions::codec. */
typedef enum {
    GD_HEIF_CODEC_UNKNOWN = 0, /**< Unknown or unspecified HEIF codec. */
    GD_HEIF_CODEC_HEVC,        /**< HEVC/H.265 HEIF codec. */
    GD_HEIF_CODEC_AV1 = 4,     /**< AV1 HEIF codec. */
} gdHeifCodec;

/** @brief HEIF chroma-subsampling string used by gdHeifWriteOptions::chroma. */
typedef const char *gdHeifChroma;

/** Use 4:2:0 chroma subsampling for HEIF output. */
#define GD_HEIF_CHROMA_420 "420"
/** Use 4:2:2 chroma subsampling for HEIF output. */
#define GD_HEIF_CHROMA_422 "422"
/** Use 4:4:4 chroma subsampling for HEIF output. */
#define GD_HEIF_CHROMA_444 "444"

/** @brief HEIF decoder options used by gdImageCreateFromHeifPtrWithOptions(). */
typedef struct {
    int ignore_transformations; /**< Nonzero to ignore HEIF image transformations while decoding. */
} gdHeifReadOptions;

/** @brief HEIF encoder options used by gdImageHeifPtrWithOptions(). */
typedef struct {
    int quality;        /**< Lossy quality from 0 to 100, -1 for the default, or 200 for lossless. */
    int lossless;       /**< Nonzero to request lossless encoding. */
    gdHeifCodec codec;  /**< HEIF codec to use for output. */
    gdHeifChroma chroma; /**< Chroma-subsampling string for output. */
    const gdImageMetadata *metadata; /**< Optional metadata to embed in the HEIF file. */
} gdHeifWriteOptions;

/**
 * @brief Information extracted from a HEIF image and its container.
 *
 * Info reports facts available in the input independently of whether GD can
 * decode or write every feature. Width, height, alpha, and bit depth describe
 * the primary image. top_level_image_count and is_animation describe the
 * container when available; they do not add a frame/page decoding API.
 * metadata is caller-owned and is populated with canonical exif, xmp, and
 * iptc profiles. HEIF's ICC color profile is intentionally not exposed as
 * metadata.
 */
typedef struct {
    int width;
    int height;
    int top_level_image_count;
    int has_alpha;
    int bit_depth;
    int is_animation;
    gdImageMetadata *metadata;
} gdHeifInfo;

/**
 * @brief Initialize HEIF information with gd defaults.
 *
 * Set info->metadata to a caller-owned metadata object after initialization
 * when metadata should be collected.
 */
BGD_DECLARE(void) gdHeifInfoInit(gdHeifInfo *info);

/**
 * @brief Read HEIF information from a stdio stream.
 *
 * The input stream is borrowed and remains open. Returns GD_META_OK on
 * success; otherwise returns a GD_META_* error code.
 */
BGD_DECLARE(int) gdHeifGetInfo(FILE *inFile, gdHeifInfo *info);

/** @brief Read HEIF information from a gdIOCtx without closing it. */
BGD_DECLARE(int) gdHeifGetInfoCtx(gdIOCtxPtr in, gdHeifInfo *info);

/** @brief Read HEIF information from an in-memory buffer without taking ownership. */
BGD_DECLARE(int) gdHeifGetInfoPtr(int size, const void *data, gdHeifInfo *info);


/**
 * @brief Initialize HEIF read options with gd defaults.
 *
 * Call this before changing selected gdHeifReadOptions fields and passing the
 * structure to gdImageCreateFromHeifPtrWithOptions().
 *
 * @param options Pointer to the read options structure to initialize.
 */
BGD_DECLARE(void) gdHeifReadOptionsInit(gdHeifReadOptions *options);

/**
 * @brief Initialize HEIF write options with gd defaults.
 *
 * Call this before changing selected gdHeifWriteOptions fields and passing the
 * structure to gdImageHeifPtrWithOptions().
 *
 * @param options Pointer to the write options structure to initialize.
 */
BGD_DECLARE(void) gdHeifWriteOptionsInit(gdHeifWriteOptions *options);

/** @} */

/** @name HEIF Reading */
/** @{ */

/**
 * @brief Create a truecolor image from HEIF data in a stdio stream.
 *
 * gdImageCreateFromHeif() does not close inFile. It decodes the primary image
 * and returns a new truecolor image. On success, the returned image is owned by
 * the caller and must be destroyed with @ref gdImageDestroy.
 *
 * @param inFile Pointer to the input FILE stream.
 * @return A newly allocated truecolor image, or NULL on error.
 */
BGD_DECLARE(gdImagePtr) gdImageCreateFromHeif(FILE *inFile);

/**
 * @brief Create a truecolor image from HEIF data in memory.
 *
 * gdImageCreateFromHeifPtr() reads size bytes from data without taking
 * ownership of the buffer. It decodes the primary image and returns a new
 * truecolor image. On success, the returned image is owned by the caller and
 * must be destroyed with @ref gdImageDestroy.
 *
 * @param size Size of data in bytes.
 * @param data Pointer to the HEIF data.
 * @return A newly allocated truecolor image, or NULL on error.
 */
BGD_DECLARE(gdImagePtr) gdImageCreateFromHeifPtr(int size, void *data);

/**
 * @brief Create a truecolor image from HEIF data in memory using read options.
 *
 * gdImageCreateFromHeifPtrWithOptions() reads size bytes from data without
 * taking ownership of the buffer. It borrows options for the duration of the
 * call; pass NULL for gd defaults. On success, the returned image is owned by
 * the caller and must be destroyed with @ref gdImageDestroy.
 *
 * @param size Size of data in bytes.
 * @param data Pointer to the HEIF data.
 * @param options HEIF read options, or NULL for defaults.
 * @return A newly allocated truecolor image, or NULL on error.
 */
BGD_DECLARE(gdImagePtr)
gdImageCreateFromHeifPtrWithOptions(int size, void *data, const gdHeifReadOptions *options);

/**
 * @brief Create a truecolor image from HEIF data in an IO context.
 *
 * gdImageCreateFromHeifCtx() reads from infile without closing it. It decodes
 * the primary image and returns a new truecolor image. On success, the returned
 * image is owned by the caller and must be destroyed with @ref gdImageDestroy.
 *
 * @param infile The input IO context.
 * @return A newly allocated truecolor image, or NULL on error.
 */
BGD_DECLARE(gdImagePtr) gdImageCreateFromHeifCtx(gdIOCtxPtr infile);

/** @} */

/** @name HEIF Writing */
/** @{ */

/**
 * @brief Write a truecolor image as HEIF data to a stdio stream.
 *
 * gdImageHeifEx() writes im with explicit quality, codec, and chroma settings.
 * Quality may be 0 to 100 for lossy output, -1 for gd's default lossy quality,
 * or 200 for lossless output. The image is borrowed for the duration of the
 * call, and outFile is not closed.
 *
 * @param im The truecolor image to write.
 * @param outFile Pointer to the output FILE stream.
 * @param quality Lossy quality from 0 to 100, -1 for the default, or 200 for lossless.
 * @param codec HEIF codec to use for output.
 * @param chroma Chroma-subsampling string for output.
 */
BGD_DECLARE(void)
gdImageHeifEx(gdImagePtr im, FILE *outFile, int quality, gdHeifCodec codec, gdHeifChroma chroma);

/**
 * @brief Write a truecolor image as HEIF data to a stdio stream.
 *
 * gdImageHeif() uses gd's default HEIF write settings: default lossy quality,
 * HEVC codec, and 4:4:4 chroma subsampling. The image is borrowed for the
 * duration of the call, and outFile is not closed.
 *
 * @param im The truecolor image to write.
 * @param outFile Pointer to the output FILE stream.
 */
BGD_DECLARE(void) gdImageHeif(gdImagePtr im, FILE *outFile);

/**
 * @brief Encode a truecolor image as HEIF data in memory.
 *
 * gdImageHeifPtr() uses gd's default HEIF write settings. The image is borrowed
 * for the duration of the call. On success, the returned buffer is owned by the
 * caller and must be freed with gdFree().
 *
 * @param im The truecolor image to encode.
 * @param size Pointer that receives the encoded buffer size in bytes.
 * @return A newly allocated HEIF data buffer, or NULL on error.
 */
BGD_DECLARE(void *) gdImageHeifPtr(gdImagePtr im, int *size);

/**
 * @brief Encode a truecolor image as HEIF data in memory.
 *
 * gdImageHeifPtrEx() writes im with explicit quality, codec, and chroma
 * settings. Quality may be 0 to 100 for lossy output, -1 for gd's default
 * lossy quality, or 200 for lossless output. The image is borrowed for the
 * duration of the call. On success, the returned buffer is owned by the caller
 * and must be freed with gdFree().
 *
 * @param im The truecolor image to encode.
 * @param size Pointer that receives the encoded buffer size in bytes.
 * @param quality Lossy quality from 0 to 100, -1 for the default, or 200 for lossless.
 * @param codec HEIF codec to use for output.
 * @param chroma Chroma-subsampling string for output.
 * @return A newly allocated HEIF data buffer, or NULL on error.
 */
BGD_DECLARE(void *)
gdImageHeifPtrEx(gdImagePtr im, int *size, int quality, gdHeifCodec codec, gdHeifChroma chroma);

/**
 * @brief Encode a truecolor image as HEIF data in memory using write options.
 *
 * gdImageHeifPtrWithOptions() borrows im and options for the duration of the
 * call. Pass NULL for options to use gd defaults. On success, the returned
 * buffer is owned by the caller and must be freed with gdFree().
 *
 * @param im The truecolor image to encode.
 * @param size Pointer that receives the encoded buffer size in bytes.
 * @param options HEIF encoder options, or NULL for defaults.
 * @return A newly allocated HEIF data buffer, or NULL on error.
 */
BGD_DECLARE(void *)
gdImageHeifPtrWithOptions(gdImagePtr im, int *size, const gdHeifWriteOptions *options);

/**
 * @brief Write HEIF data to a stdio stream using write options.
 *
 * The stream is borrowed and remains open. Returns 0 on success and nonzero
 * on failure. Pass NULL for options to use gd defaults.
 */
BGD_DECLARE(int)
gdImageHeifWithOptions(gdImagePtr im, FILE *outFile, const gdHeifWriteOptions *options);

/**
 * @brief Write HEIF data to a gdIOCtx using write options.
 *
 * The context is borrowed and remains open. Returns 0 on success and nonzero
 * on failure. Pass NULL for options to use gd defaults.
 */
BGD_DECLARE(int)
gdImageHeifCtxWithOptions(gdImagePtr im, gdIOCtxPtr outfile, const gdHeifWriteOptions *options);

/**
 * @brief Write a truecolor image as HEIF data to an IO context.
 *
 * gdImageHeifCtx() writes im with explicit quality, codec, and chroma settings.
 * Quality may be 0 to 100 for lossy output, -1 for gd's default lossy quality,
 * or 200 for lossless output. The image is borrowed for the duration of the
 * call, and outfile is not closed.
 *
 * @param im The truecolor image to write.
 * @param outfile The output IO context.
 * @param quality Lossy quality from 0 to 100, -1 for the default, or 200 for lossless.
 * @param codec HEIF codec to use for output.
 * @param chroma Chroma-subsampling string for output.
 */
BGD_DECLARE(void)
gdImageHeifCtx(gdImagePtr im, gdIOCtxPtr outfile, int quality, gdHeifCodec codec,
               gdHeifChroma chroma);

/** @} */

/** @} */

/**
 * @defgroup gdCodecAvif AVIF
 * @brief Read and write AV1 Image File Format images.
 * @ingroup gdCodecs
 *
 * AVIF support reads AVIF data from stdio streams, memory buffers, or gd IO
 * contexts and returns truecolor images. If the AVIF input contains an image
 * sequence, gd reads the first image and ignores subsequent frames. AVIF writing
 * accepts truecolor images and can write to stdio streams, memory buffers, or
 * gd IO contexts with quality, speed, lossless, and chroma-subsampling options.
 *
 * @code{.c}
 * FILE *in;
 * gdImagePtr im;
 * gdAvifWriteOptions options;
 * void *data;
 * int size;
 *
 * in = fopen("input.avif", "rb");
 * if (in == NULL) {
 *     return 1;
 * }
 *
 * im = gdImageCreateFromAvif(in);
 * fclose(in);
 * if (im == NULL) {
 *     return 1;
 * }
 *
 * gdAvifWriteOptionsInit(&options);
 * options.quality = 90;
 * options.speed = 6;
 * options.chroma_subsampling = GD_AVIF_CHROMA_SUBSAMPLING_YUV444;
 *
 * data = gdImageAvifPtrWithOptions(im, &size, &options);
 * if (data != NULL) {
 *     gdFree(data);
 * }
 *
 * gdImageDestroy(im);
 * @endcode
 *
 * @{
 */

/** @name AVIF Reading */
/** @{ */

/**
 * @brief Create a truecolor image from AVIF data in a stdio stream.
 *
 * gdImageCreateFromAvif() does not close inFile. If the AVIF contains an image
 * sequence, only the first image is decoded. On success, the returned image is
 * owned by the caller and must be destroyed with @ref gdImageDestroy.
 *
 * @param inFile Pointer to the input FILE stream.
 * @return A newly allocated truecolor image, or NULL on error.
 */
BGD_DECLARE(gdImagePtr) gdImageCreateFromAvif(FILE *inFile);

/**
 * @brief Create a truecolor image from AVIF data in memory.
 *
 * gdImageCreateFromAvifPtr() reads size bytes from data without taking
 * ownership of the buffer. If the AVIF contains an image sequence, only the
 * first image is decoded. On success, the returned image is owned by the caller
 * and must be destroyed with @ref gdImageDestroy.
 *
 * @param size Size of data in bytes.
 * @param data Pointer to the AVIF data.
 * @return A newly allocated truecolor image, or NULL on error.
 */
BGD_DECLARE(gdImagePtr) gdImageCreateFromAvifPtr(int size, void *data);

/**
 * @brief Create a truecolor image from AVIF data in an IO context.
 *
 * gdImageCreateFromAvifCtx() reads from infile without closing it. If the AVIF
 * contains an image sequence, only the first image is decoded. On success, the
 * returned image is owned by the caller and must be destroyed with
 * @ref gdImageDestroy.
 *
 * @param infile The input IO context.
 * @return A newly allocated truecolor image, or NULL on error.
 */
BGD_DECLARE(gdImagePtr) gdImageCreateFromAvifCtx(gdIOCtxPtr infile);

/** @} */

/** @name AVIF Write Options */
/** @{ */

/** AVIF chroma subsampling modes for gdAvifWriteOptions::chroma_subsampling. */
enum {
    GD_AVIF_CHROMA_SUBSAMPLING_AUTO = 0,  /**< Choose subsampling automatically from the quality settings. */
    GD_AVIF_CHROMA_SUBSAMPLING_YUV420 = 1, /**< Use 4:2:0 chroma subsampling. */
    GD_AVIF_CHROMA_SUBSAMPLING_YUV444 = 2  /**< Use 4:4:4 chroma subsampling. */
};

/** AVIF YUV pixel formats reported by gdAvifInfo::yuv_format. */
enum {
    GD_AVIF_PIXEL_FORMAT_NONE = 0,
    GD_AVIF_PIXEL_FORMAT_YUV444 = 1,
    GD_AVIF_PIXEL_FORMAT_YUV422 = 2,
    GD_AVIF_PIXEL_FORMAT_YUV420 = 3,
    GD_AVIF_PIXEL_FORMAT_YUV400 = 4
};

/** @brief AVIF encoder options used by gdImageAvifPtrWithOptions(). */
typedef struct {
    int quality;            /**< Compression quality from 0 to 100, or -1 for the default. */
    int speed;              /**< Encoder speed; lower values are slower and may improve compression. */
    int lossless;           /**< Nonzero to request lossless encoding. */
    int chroma_subsampling; /**< One of the GD_AVIF_CHROMA_SUBSAMPLING_* values. */
    const gdImageMetadata *metadata; /**< Optional EXIF/XMP metadata to embed. */
} gdAvifWriteOptions;

/**
 * @brief Information extracted from the AVIF image and container.
 *
 * The fields describe facts available from the input, independently of the
 * features currently decoded or written by GD. Dimensions, alpha, bit depth,
 * and YUV format describe the primary image. Sequence fields describe the
 * AVIF input when present. metadata is caller-owned and contains canonical
 * EXIF and XMP profiles; ICC is not part of the public metadata path.
 */
typedef struct {
    int width; /**< Image width in pixels. */
    int height; /**< Image height in pixels. */
    int is_animation; /**< Nonzero if the image is an animation. */
    int is_progressive; /**< Nonzero if the image is progressive. */
    int frame_count; /**< Number of frames in the animation. */
    double duration; /**< Duration of the animation in seconds. */
    int has_alpha; /**< Nonzero if the image has an alpha channel. */
    int bit_depth; /**< Bit depth of the image. */
    int yuv_format; /**< One of the GD_AVIF_PIXEL_FORMAT_* values. */
    gdImageMetadata *metadata; /**< Pointer to the image metadata. */
} gdAvifInfo;

/** @brief Initialize AVIF information and clear the metadata pointer.
 * 
 * @param info Pointer to the gdAvifInfo structure to initialize.
 */
BGD_DECLARE(void) gdAvifInfoInit(gdAvifInfo *info);

/** @brief Read AVIF information from a stdio stream without closing it. *
 * 
 * @param inFile Pointer to the input FILE stream.
 * @param info Pointer to the gdAvifInfo structure to populate.
 * 
 * @return Returns 1 on success, or 0 on failure.
 */
BGD_DECLARE(int) gdAvifGetInfo(FILE *inFile, gdAvifInfo *info);

/** @brief Read AVIF information from a gdIOCtx without closing it.
 * 
 * @param in Pointer to the gdIOCtx input context.
 * @param info Pointer to the gdAvifInfo structure to populate.
 * 
 * @return Returns 1 on success, or 0 on failure.
  */
BGD_DECLARE(int) gdAvifGetInfoCtx(gdIOCtxPtr in, gdAvifInfo *info);

/** @brief Read AVIF information from memory without taking ownership. 
 * 
 * @param size Size of the AVIF memory buffer in bytes.
 * @param data Pointer to the AVIF memory buffer.
 * @param info Pointer to the gdAvifInfo structure to populate.
 * 
 * @return Returns 1 on success, or 0 on failure.
*/
BGD_DECLARE(int) gdAvifGetInfoPtr(int size, const void *data, gdAvifInfo *info);

/**
 * @brief Initialize AVIF write options with gd defaults.
 *
 * Call this before changing selected gdAvifWriteOptions fields and passing the
 * structure to gdImageAvifPtrWithOptions().
 *
 * @param options Pointer to the options structure to initialize.
 */
BGD_DECLARE(void) gdAvifWriteOptionsInit(gdAvifWriteOptions *options);

/** @} */

/** @name AVIF Writing */
/** @{ */

/**
 * @brief Write a truecolor image as AVIF data to a stdio stream.
 *
 * gdImageAvif() uses gd's default AVIF quality and speed settings. The image is
 * borrowed for the duration of the call, and outFile is not closed.
 *
 * @param im The truecolor image to write.
 * @param outFile Pointer to the output FILE stream.
 */
BGD_DECLARE(void) gdImageAvif(gdImagePtr im, FILE *outFile);

/**
 * @brief Write a truecolor image as AVIF data to a stdio stream.
 *
 * gdImageAvifEx() writes im with explicit quality and speed settings. Quality
 * values range from 0 to 100, with higher values improving quality and 100
 * requesting lossless output; -1 selects the default. Speed is passed to the
 * AVIF encoder and clamped to its supported range. The image is borrowed for
 * the duration of the call, and outFile is not closed.
 *
 * @param im The truecolor image to write.
 * @param outFile Pointer to the output FILE stream.
 * @param quality Compression quality from 0 to 100, or -1 for the default.
 * @param speed Encoder speed; lower values are slower and may improve compression.
 */
BGD_DECLARE(void)
gdImageAvifEx(gdImagePtr im, FILE *outFile, int quality, int speed);

/**
 * @brief Encode a truecolor image as AVIF data in memory.
 *
 * gdImageAvifPtr() uses gd's default AVIF quality and speed settings. The image
 * is borrowed for the duration of the call. On success, the returned buffer is
 * owned by the caller and must be freed with gdFree().
 *
 * @param im The truecolor image to encode.
 * @param size Pointer that receives the encoded buffer size in bytes.
 * @return A newly allocated AVIF data buffer, or NULL on error.
 */
BGD_DECLARE(void *) gdImageAvifPtr(gdImagePtr im, int *size);

/**
 * @brief Encode a truecolor image as AVIF data in memory.
 *
 * gdImageAvifPtrEx() writes im with explicit quality and speed settings.
 * Quality values range from 0 to 100, with higher values improving quality and
 * 100 requesting lossless output; -1 selects the default. Speed is passed to the
 * AVIF encoder and clamped to its supported range. The image is borrowed for the
 * duration of the call. On success, the returned buffer is owned by the caller
 * and must be freed with gdFree().
 *
 * @param im The truecolor image to encode.
 * @param size Pointer that receives the encoded buffer size in bytes.
 * @param quality Compression quality from 0 to 100, or -1 for the default.
 * @param speed Encoder speed; lower values are slower and may improve compression.
 * @return A newly allocated AVIF data buffer, or NULL on error.
 */
BGD_DECLARE(void *)
gdImageAvifPtrEx(gdImagePtr im, int *size, int quality, int speed);

/**
 * @brief Encode a truecolor image as AVIF data in memory using write options.
 *
 * gdImageAvifPtrWithOptions() borrows im and options for the duration of the
 * call. Pass NULL for options to use gd defaults. On success, the returned
 * buffer is owned by the caller and must be freed with gdFree().
 *
 * @param im The truecolor image to encode.
 * @param size Pointer that receives the encoded buffer size in bytes.
 * @param options AVIF encoder options, or NULL for defaults.
 * @return A newly allocated AVIF data buffer, or NULL on error.
 */
BGD_DECLARE(void *)
gdImageAvifPtrWithOptions(gdImagePtr im, int *size, const gdAvifWriteOptions *options);

/** @brief Write AVIF data to a stdio stream using write options. */
BGD_DECLARE(int)
gdImageAvifWithOptions(gdImagePtr im, FILE *outFile, const gdAvifWriteOptions *options);

/** @brief Write AVIF data to a gdIOCtx using write options. */
BGD_DECLARE(int)
gdImageAvifCtxWithOptions(gdImagePtr im, gdIOCtxPtr outfile, const gdAvifWriteOptions *options);

/**
 * @brief Write a truecolor image as AVIF data to an IO context.
 *
 * gdImageAvifCtx() writes im with explicit quality and speed settings. Quality
 * values range from 0 to 100, with higher values improving quality and 100
 * requesting lossless output; -1 selects the default. Speed is passed to the
 * AVIF encoder and clamped to its supported range. The image is borrowed for
 * the duration of the call, and outfile is not closed.
 *
 * @param im The truecolor image to write.
 * @param outfile The output IO context.
 * @param quality Compression quality from 0 to 100, or -1 for the default.
 * @param speed Encoder speed; lower values are slower and may improve compression.
 */
BGD_DECLARE(void)
gdImageAvifCtx(gdImagePtr im, gdIOCtxPtr outfile, int quality, int speed);

/** @} */

/** @} */

/**
 * @defgroup gdCodecTiff TIFF
 * @brief TIFF image reading, writing, and multi-page support.
 * @ingroup gdCodecs
 *
 * TIFF support reads single images through the gdImageCreateFromTiff*()
 * functions and reads multi-page TIFF files through gdTiffRead*() readers.
 * The multi-page reader returns one caller-owned gd image per page. TIFF
 * writers can write one or more truecolor images using explicit bit-depth,
 * colorspace, compression, alpha, and resolution options.
 *
 * @code{.c}
 *        gdTiffReadPtr reader;
 *        gdTiffInfo info;
 *        gdTiffPageInfo page;
 *        gdImagePtr image;
 *        FILE *in;
 *
 *        in = fopen("input.tif", "rb");
 *        if (in == NULL) {
 *          fprintf(stderr, "cannot open input.tif\n");
 *          exit(1);
 *        }
 *
 *        reader = gdTiffReadOpen(in, NULL);
 *        fclose(in);
 *        if (reader == NULL) {
 *          fprintf(stderr, "cannot read TIFF\n");
 *          exit(1);
 *        }
 *
 *        if (gdTiffReadGetInfo(reader, &info)) {
 *          printf("pages: %d, first page: %dx%d\n",
 *                 info.pageCount, info.width, info.height);
 *        }
 *
 *        while (gdTiffReadNextImage(reader, &page, &image) == 1) {
 *          printf("page %d: %dx%d\n", page.pageIndex, page.width, page.height);
 *          gdImageDestroy(image);
 *        }
 *
 *        gdTiffReadClose(reader);
 * @endcode
 *
 * @{
 */

/** @name TIFF Single-Image Reading */
/** @{ */

/**
 * @brief Create an image from a TIFF stdio file.
 *
 * gdImageCreateFromTiff() does not close inFile. The returned image is
 * caller-owned and must be destroyed with @ref gdImageDestroy.
 *
 * @param inFile Pointer to the input FILE stream.
 *
 * @return Returns a gdImagePtr on success, or NULL on failure.
 */
BGD_DECLARE(gdImagePtr) gdImageCreateFromTiff(FILE *inFile);

/**
 * @brief Create an image from TIFF data read through a gdIOCtx.
 *
 * gdImageCreateFromTiffCtx() does not close infile. The returned image is
 * caller-owned and must be destroyed with @ref gdImageDestroy.
 *
 * @param infile Pointer to the gdIOCtx input context.
 *
 * @return Returns a gdImagePtr on success, or NULL on failure.
 */
BGD_DECLARE(gdImagePtr) gdImageCreateFromTiffCtx(gdIOCtxPtr infile);

/**
 * @brief Create an image from a TIFF memory buffer.
 *
 * The data buffer is borrowed for the duration of the call. The returned image
 * is caller-owned and must be destroyed with @ref gdImageDestroy.
 *
 * @param size Size of the TIFF memory buffer in bytes.
 * @param data Pointer to the TIFF memory buffer.
 *
 * @return Returns a gdImagePtr on success, or NULL on failure.
 */
BGD_DECLARE(gdImagePtr) gdImageCreateFromTiffPtr(int size, void *data);

/** @} */

/** @name TIFF Reading Types */
/** @{ */

/**
 * @brief Opaque TIFF multi-page reader handle.
 *
 * Handles returned by gdTiffReadOpen(), gdTiffReadOpenCtx(), or
 * gdTiffReadOpenPtr() must be closed with gdTiffReadClose().
 */
typedef struct gdTiffReadStruct *gdTiffReadPtr;

/**
 * @brief Options for reading TIFF data with the gdTiffRead*() API.
 *
 * Initialize with gdTiffReadOptionsInit() before setting fields.
 */
typedef struct {
    int notused;  /**< Reserved for future read options; currently unused. */
} gdTiffReadOptions;

/**
 * @brief TIFF file information from the first page and container.
 */
typedef struct {
    int width;              /**< First page width in pixels. */
    int height;             /**< First page height in pixels. */
    int page_count;         /**< Number of TIFF directories/pages in the file. */
    int bits_per_sample;    /**< First page bits per sample. */
    int samples_per_pixel;  /**< First page samples per pixel. */
    int compression;        /**< First page compression tag value, usually a GD_TIFF_COMPRESSION_* enum value. */
    int photometric;        /**< First page photometric tag value, usually a GD_TIFF_PHOTOMETRIC_* enum value. */
    float x_resolution;     /**< First page horizontal resolution. */
    float y_resolution;     /**< First page vertical resolution. */
    int resolution_unit;    /**< First page resolution unit, one of the GD_TIFF_RESUNIT_* enum values. */
} gdTiffInfo;

/**
 * @brief TIFF page information returned while reading pages.
 */
typedef struct {
    int page_index;         /**< Zero-based page index. */
    int width;              /**< Page width in pixels. */
    int height;             /**< Page height in pixels. */
    int bits_per_sample;    /**< Page bits per sample. */
    int samples_per_pixel;  /**< Page samples per pixel. */
    int compression;        /**< Page compression tag value, usually a GD_TIFF_COMPRESSION_* enum value. */
    int photometric;        /**< Page photometric tag value, usually a GD_TIFF_PHOTOMETRIC_* enum value. */
    int planar;             /**< Page planar configuration, one of the GD_TIFF_PLANARCONFIG_* enum values. */
    int has_alpha;          /**< Non-zero if the page has extra alpha samples. */
    int is_tiled;           /**< Non-zero if the page is stored as TIFF tiles. */
    float x_resolution;     /**< Page horizontal resolution. */
    float y_resolution;     /**< Page vertical resolution. */
    int resolution_unit;    /**< Page resolution unit, one of the GD_TIFF_RESUNIT_* enum values. */
} gdTiffPageInfo;

/** @} */

/** @name TIFF Multi-Page Reading */
/** @{ */

/**
 * @brief Test whether a TIFF stdio file has more than one page.
 *
 * gdTiffIsMultiPage() reads from fd and attempts to restore its stream
 * position before returning. It does not close fd.
 *
 * @param fd Pointer to the input FILE stream.
 *
 * @return Returns 1 for multi-page TIFF, 0 for single-page TIFF, or -1 on error.
 */
BGD_DECLARE(int) gdTiffIsMultiPage(FILE *fd);

/**
 * @brief Test whether TIFF data read through a seekable gdIOCtx has more than one page.
 *
 * gdTiffIsMultiPageCtx() reads from in and attempts to restore its position
 * before returning. It does not close in.
 *
 * @param in Pointer to the gdIOCtx input context.
 *
 * @return Returns 1 for multi-page TIFF, 0 for single-page TIFF, or -1 on error.
 */
BGD_DECLARE(int) gdTiffIsMultiPageCtx(gdIOCtxPtr in);

/**
 * @brief Test whether a TIFF memory buffer has more than one page.
 *
 * The data buffer is borrowed for the duration of the call.
 *
 * @param size Size of the TIFF memory buffer in bytes.
 * @param data Pointer to the TIFF memory buffer.
 *
 * @return Returns 1 for multi-page TIFF, 0 for single-page TIFF, or -1 on error.
 */
BGD_DECLARE(int) gdTiffIsMultiPagePtr(int size, void *data);

/**
 * @brief Initialize TIFF read options with defaults.
 *
 * @param options Pointer to the options structure to initialize.
 */
BGD_DECLARE(void) gdTiffReadOptionsInit(gdTiffReadOptions *options);

/**
 * @brief Open a TIFF multi-page reader from a stdio file.
 *
 * gdTiffReadOpen() reads the TIFF data into the reader and does not close fd.
 * The returned handle must be closed with gdTiffReadClose().
 *
 * @param fd Pointer to the input FILE stream.
 * @param options Pointer to read options, or NULL for defaults.
 *
 * @return Returns a TIFF reader handle on success, or NULL on failure.
 */
BGD_DECLARE(gdTiffReadPtr) gdTiffReadOpen(FILE *fd, const gdTiffReadOptions *options);

/**
 * @brief Open a TIFF multi-page reader from a gdIOCtx.
 *
 * gdTiffReadOpenCtx() reads the TIFF data into the reader and does not close
 * in. The returned handle must be closed with gdTiffReadClose().
 *
 * @param in Pointer to the gdIOCtx input context.
 * @param options Pointer to read options, or NULL for defaults.
 *
 * @return Returns a TIFF reader handle on success, or NULL on failure.
 */
BGD_DECLARE(gdTiffReadPtr) gdTiffReadOpenCtx(gdIOCtxPtr in, const gdTiffReadOptions *options);

/**
 * @brief Open a TIFF multi-page reader from a memory buffer.
 *
 * The data buffer is borrowed for the duration of the call. The returned
 * reader owns its copy of the TIFF data and must be closed with
 * gdTiffReadClose().
 *
 * @param size Size of the TIFF memory buffer in bytes.
 * @param data Pointer to the TIFF memory buffer.
 * @param options Pointer to read options, or NULL for defaults.
 *
 * @return Returns a TIFF reader handle on success, or NULL on failure.
 */
BGD_DECLARE(gdTiffReadPtr) gdTiffReadOpenPtr(int size, void *data, const gdTiffReadOptions *options);

/**
 * @brief Close a TIFF multi-page reader.
 *
 * @param tiff TIFF reader handle to close, or NULL.
 */
BGD_DECLARE(void) gdTiffReadClose(gdTiffReadPtr tiff);

/**
 * @brief Get TIFF file information from a reader.
 *
 * @param tiff TIFF reader handle.
 * @param info Pointer to a gdTiffInfo structure to receive file information.
 *
 * @return Returns 1 on success, or 0 on failure.
 */
BGD_DECLARE(int) gdTiffReadGetInfo(gdTiffReadPtr tiff, gdTiffInfo *info);

/** Collect opaque metadata from the first TIFF directory. */
BGD_DECLARE(int) gdTiffReadGetMetadata(gdTiffReadPtr tiff, gdImageMetadata *metadata);

/**
 * @brief Read the next TIFF page image.
 *
 * When image is not NULL and the function returns 1, *image receives a
 * caller-owned gd image that must be destroyed with @ref gdImageDestroy. Passing
 * NULL for image advances the reader without returning the decoded image.
 *
 * @param tiff TIFF reader handle.
 * @param info Pointer to a gdTiffPageInfo structure to receive page information, or NULL.
 * @param image Pointer to receive the caller-owned page image, or NULL.
 *
 * @return Returns 1 when a page is read, 0 at end of file, or -1 on error.
 */
BGD_DECLARE(int)
gdTiffReadNextImage(gdTiffReadPtr tiff, gdTiffPageInfo *info, gdImagePtr *image);

/** @} */

/** @name TIFF Constants */
/** @{ */

/**
 * @brief TIFF writer color spaces.
 */
typedef enum {
    GD_TIFF_RGB = 1,     /**< Write RGB TIFF data. */
    GD_TIFF_RGBA = 2,    /**< Write RGBA TIFF data with an alpha extra sample. */
    GD_TIFF_GRAY = 3,    /**< Write grayscale TIFF data. */
    GD_TIFF_BILEVEL = 4  /**< Write 1-bit bilevel TIFF data. */
} gdTiffColorSpace;

/**
 * @brief TIFF compression tag values.
 */
typedef enum {
    GD_TIFF_COMPRESSION_NONE = 1,            /**< No TIFF compression. */
    GD_TIFF_COMPRESSION_CCITT_RLE = 2,       /**< CCITT modified Huffman run-length encoding compression. */
    GD_TIFF_COMPRESSION_CCITT_FAX3 = 3,      /**< CCITT Group 3 fax compression. */
    GD_TIFF_COMPRESSION_CCITT_FAX4 = 4,      /**< CCITT Group 4 fax compression. */
    GD_TIFF_COMPRESSION_LZW = 5,             /**< LZW compression. */
    GD_TIFF_COMPRESSION_JPEG = 7,            /**< JPEG compression. */
    GD_TIFF_COMPRESSION_ADOBE_DEFLATE = 8,   /**< Adobe-style Deflate compression. */
    GD_TIFF_COMPRESSION_DEFLATE = 32946,     /**< Deflate compression. */
    GD_TIFF_COMPRESSION_PACKBITS = 32773     /**< PackBits compression. */
} gdTiffCompression;

/**
 * @brief TIFF photometric interpretation tag values.
 */
typedef enum {
    GD_TIFF_PHOTOMETRIC_MINISWHITE = 0,          /**< White is the minimum sample value. */
    GD_TIFF_PHOTOMETRIC_MINISBLACK = 1,          /**< Black is the minimum sample value. */
    GD_TIFF_PHOTOMETRIC_RGB = 2,                 /**< RGB photometric interpretation. */
    GD_TIFF_PHOTOMETRIC_PALETTE = 3,             /**< Palette color photometric interpretation. */
    GD_TIFF_PHOTOMETRIC_TRANSPARENCY_MASK = 4,   /**< Transparency mask photometric interpretation. */
    GD_TIFF_PHOTOMETRIC_SEPARATED = 5,           /**< Separated photometric interpretation. */
    GD_TIFF_PHOTOMETRIC_YCBCR = 6,               /**< YCbCr photometric interpretation. */
    GD_TIFF_PHOTOMETRIC_CIELAB = 8               /**< CIE L*a*b* photometric interpretation. */
} gdTiffPhotometric;

/**
 * @brief TIFF planar configuration tag values.
 */
typedef enum {
    GD_TIFF_PLANARCONFIG_CONTIG = 1,    /**< Store samples for each pixel contiguously. */
    GD_TIFF_PLANARCONFIG_SEPARATE = 2   /**< Store samples in separate planes. */
} gdTiffPlanarConfig;

/**
 * @brief TIFF resolution unit tag values.
 */
typedef enum {
    GD_TIFF_RESUNIT_NONE = 1,       /**< Resolution values have no absolute unit. */
    GD_TIFF_RESUNIT_INCH = 2,       /**< Resolution values are pixels per inch. */
    GD_TIFF_RESUNIT_CENTIMETER = 3  /**< Resolution values are pixels per centimeter. */
} gdTiffResolutionUnit;

/**
 * @brief TIFF alpha sample types.
 */
typedef enum {
    GD_TIFF_ALPHA_UNASSOCIATED = 1,  /**< TIFF alpha samples are unassociated with color samples. */
    GD_TIFF_ALPHA_ASSOCIATED = 2     /**< TIFF alpha samples are premultiplied into color samples. */
} gdTiffAlphaType;

/** @} */

/** @name TIFF Writing Types */
/** @{ */

/**
 * @brief Options for writing TIFF data with the gdTiffWrite*() API.
 *
 * Initialize with gdTiffWriteOptionsInit() before setting fields. NULL options
 * use defaults: 8-bit RGBA, Adobe Deflate compression, inch resolution units,
 * 72x72 resolution, and unassociated alpha.
 */
typedef struct {
    int bitDepth;        /**< Bits per sample: 1, 8, or 16. */
    gdTiffColorSpace colorspace;       /**< Output colorspace. */
    gdTiffCompression compression;     /**< TIFF compression. */
    int jpegQuality;     /**< JPEG compression quality when compression is GD_TIFF_COMPRESSION_JPEG. */
    int minIsWhite;      /**< Non-zero to use white as the minimum sample value for gray or bilevel output. */
    gdTiffResolutionUnit resolutionUnit;  /**< Resolution unit. */
    float xResolution;   /**< Horizontal resolution to store in the TIFF file. */
    float yResolution;   /**< Vertical resolution to store in the TIFF file. */
    gdTiffAlphaType alphaType;         /**< Alpha sample type. */
    const gdImageMetadata *metadata;   /**< Opaque TIFF tag metadata. */
} gdTiffWriteOptions;

/**
 * @brief Opaque TIFF writer handle.
 *
 * Handles returned by gdTiffWriteOpen() or gdTiffWriteOpenCtx() must be closed
 * with gdTiffWriteClose(). Handles returned by gdTiffWriteOpenPtr() must be
 * finished with gdTiffWritePtrFinish().
 */
typedef struct gdTiffWriteStruct *gdTiffWritePtr;

/** @} */

/** @name TIFF Multi-Page Writing */
/** @{ */

/**
 * @brief Initialize TIFF write options with defaults.
 *
 * @param options Pointer to the options structure to initialize.
 */
BGD_DECLARE(void) gdTiffWriteOptionsInit(gdTiffWriteOptions *options);

/**
 * @brief Open a TIFF writer for a stdio file.
 *
 * gdTiffWriteOpen() does not close outFile. The returned handle must be
 * closed with gdTiffWriteClose().
 *
 * @param outFile Pointer to the output FILE stream.
 * @param options Pointer to write options, or NULL for defaults.
 *
 * @return Returns a TIFF writer handle on success, or NULL on failure.
 */
BGD_DECLARE(gdTiffWritePtr)
gdTiffWriteOpen(FILE *outFile, const gdTiffWriteOptions *options);

/**
 * @brief Open a TIFF writer for a gdIOCtx.
 *
 * The output context is borrowed and is not closed by gdTiffWriteClose(). The
 * returned handle must be closed with gdTiffWriteClose().
 *
 * @param out Pointer to the gdIOCtx output context.
 * @param options Pointer to write options, or NULL for defaults.
 *
 * @return Returns a TIFF writer handle on success, or NULL on failure.
 */
BGD_DECLARE(gdTiffWritePtr)
gdTiffWriteOpenCtx(gdIOCtxPtr out, const gdTiffWriteOptions *options);

/**
 * @brief Open a TIFF writer that returns a memory buffer.
 *
 * The returned handle must be finished with gdTiffWritePtrFinish().
 *
 * @param options Pointer to write options, or NULL for defaults.
 *
 * @return Returns a TIFF memory writer handle on success, or NULL on failure.
 */
BGD_DECLARE(gdTiffWritePtr)
gdTiffWriteOpenPtr(const gdTiffWriteOptions *options);

/**
 * @brief Add an image as the next TIFF page.
 *
 * The image is borrowed for the duration of the call and remains owned by the
 * caller. This writer API accepts truecolor images only.
 *
 * @param write TIFF writer handle.
 * @param image Image to add as the next page.
 *
 * @return Returns 1 on success, or 0 on failure.
 */
BGD_DECLARE(int) gdTiffWriteAddImage(gdTiffWritePtr write, gdImagePtr image);

/**
 * @brief Close a file or gdIOCtx TIFF writer.
 *
 * Use this for handles returned by gdTiffWriteOpen() or gdTiffWriteOpenCtx().
 * For memory writers returned by gdTiffWriteOpenPtr(), use
 * gdTiffWritePtrFinish().
 *
 * @param write TIFF writer handle to close, or NULL.
 */
BGD_DECLARE(void) gdTiffWriteClose(gdTiffWritePtr write);

/**
 * @brief Finish a TIFF memory writer and return the encoded buffer.
 *
 * This closes write whether encoding succeeds or fails. The returned buffer is
 * caller-owned and must be freed with gdFree().
 *
 * @param write TIFF memory writer handle returned by gdTiffWriteOpenPtr().
 * @param size Pointer to an integer that receives the returned buffer size.
 *
 * @return Returns a pointer to the newly allocated TIFF buffer, or NULL on failure.
 */
BGD_DECLARE(void *) gdTiffWritePtrFinish(gdTiffWritePtr write, int *size);

/** @} */

/** @name TIFF Single-Image Writing */
/** @{ */

/**
 * @brief Write an image as TIFF data to a stdio file.
 *
 * gdImageTiff() does not close outFile. The image is borrowed for the duration
 * of the call.
 *
 * @param im The image to write.
 * @param outFile Pointer to the output FILE stream.
 */
BGD_DECLARE(void) gdImageTiff(gdImagePtr im, FILE *outFile);

/**
 * @brief Write an image as TIFF data to a newly allocated memory buffer.
 *
 * The image is borrowed for the duration of the call. The returned buffer is
 * caller-owned and must be freed with gdFree().
 *
 * @param im The image to write.
 * @param size Pointer to an integer that receives the returned buffer size.
 *
 * @return Returns a pointer to the newly allocated TIFF buffer, or NULL on failure.
 */
BGD_DECLARE(void *) gdImageTiffPtr(gdImagePtr im, int *size);

/**
 * @brief Write an image as TIFF data to a gdIOCtx.
 *
 * gdImageTiffCtx() does not close out. The image is borrowed for the duration
 * of the call.
 *
 * @param image The image to write.
 * @param out Pointer to the gdIOCtx output context.
 */
BGD_DECLARE(void) gdImageTiffCtx(gdImagePtr image, gdIOCtxPtr out);

/** @} */
/** @} */

/**
 * @defgroup gdCodecTga TGA
 * @brief Read Truevision TGA images.
 * @ingroup gdCodecs
 *
 * TGA support is read-only. The reader accepts stdio streams, gdIOCtx streams,
 * and caller-provided memory buffers, and returns a new truecolor gd image.
 * The returned image is owned by the caller and must be destroyed with
 * gdImageDestroy().
 *
 * GD reads uncompressed and RLE-compressed color-mapped, truecolor, and
 * grayscale TGA images. Supported inputs include 8-bit indexed data with
 * 15-, 16-, 24-, or 32-bit color map entries, 16- and 24-bit truecolor data,
 * 32-bit truecolor data with 8 alpha bits, and 8-bit grayscale data. Image
 * origin flags are applied so the returned gd image has the expected
 * orientation. When decoded alpha is present, alpha saving is enabled on the
 * returned image.
 *
 * @code{.c}
 * FILE *in;
 * FILE *out;
 * gdImagePtr im;
 *
 * in = fopen("input.tga", "rb");
 * if (in == NULL) {
 *     return 1;
 * }
 *
 * im = gdImageCreateFromTga(in);
 * fclose(in);
 * if (im == NULL) {
 *     return 1;
 * }
 *
 * out = fopen("output.png", "wb");
 * if (out == NULL) {
 *     gdImageDestroy(im);
 *     return 1;
 * }
 *
 * gdImagePng(im, out);
 * fclose(out);
 * gdImageDestroy(im);
 * @endcode
 *
 * @{
 */

/** @name TGA Reading */
/** @{ */

/**
 * @brief Create an image from TGA data in a stdio stream.
 *
 * gdImageCreateFromTga() borrows fp for the duration of the call and does not
 * close it. On success, the returned image is owned by the caller and must be
 * destroyed with gdImageDestroy().
 *
 * @param fp Pointer to the input FILE stream.
 * @return A newly allocated truecolor image, or NULL on error.
 */
BGD_DECLARE(gdImagePtr) gdImageCreateFromTga(FILE *fp);

/**
 * @brief Create an image from TGA data in a gdIOCtx.
 *
 * gdImageCreateFromTgaCtx() borrows ctx for the duration of the call and does
 * not close it. On success, the returned image is owned by the caller and must
 * be destroyed with gdImageDestroy().
 *
 * @param ctx Pointer to the gdIOCtx input context.
 * @return A newly allocated truecolor image, or NULL on error.
 */
BGD_DECLARE(gdImagePtr) gdImageCreateFromTgaCtx(gdIOCtxPtr ctx);

/**
 * @brief Create an image from a TGA memory buffer.
 *
 * gdImageCreateFromTgaPtr() borrows data for the duration of the call. The
 * caller retains ownership of the input buffer. On success, the returned image
 * is owned by the caller and must be destroyed with gdImageDestroy().
 *
 * @param size Size of the TGA memory buffer in bytes.
 * @param data Pointer to the TGA memory buffer.
 * @return A newly allocated truecolor image, or NULL on error.
 */
BGD_DECLARE(gdImagePtr) gdImageCreateFromTgaPtr(int size, void *data);

/** @} */

/** @} */

/**
 * @defgroup gdCodecBmp BMP
 * @brief Read and write Microsoft Windows bitmap images.
 * @ingroup gdCodecs
 *
 * BMP support reads stdio files, gdIOCtx streams, or caller-provided memory
 * buffers into gd images. Indexed BMP inputs are returned as palette images
 * when possible, while direct-color BMP inputs are returned as truecolor
 * images. The returned image is owned by the caller and must be destroyed with
 * @ref gdImageDestroy.
 *
 * BMP output can use the legacy automatic-bit-depth APIs or the extended APIs
 * that select a specific BMP bit depth, compression mode, and writer flags.
 * The writer supports 1, 4, 8, 16, 24, and 32 bits per pixel. RLE4 is valid
 * only for 4 bpp output, and RLE8 is valid only for 8 bpp output. BMP output
 * handles and FILE streams are borrowed and are not closed by gd.
 *
 * @code{.c}
 * gdImagePtr im, roundtrip;
 * void *data;
 * int size;
 *
 * im = gdImageCreateTrueColor(32, 32);
 * gdImageFilledRectangle(im, 0, 0, 31, 31, 0x336699);
 *
 * data = gdImageBmpPtrEx(im, &size, 24, GD_BMP_COMPRESS_NONE,
 *                        GD_BMP_FLAG_NONE);
 * if (data != NULL) {
 *        roundtrip = gdImageCreateFromBmpPtr(size, data);
 *        gdFree(data);
 *        if (roundtrip != NULL) {
 *                gdImageDestroy(roundtrip);
 *        }
 * }
 *
 * gdImageDestroy(im);
 * @endcode
 * @{
 */

/**
 * @brief Create an image from a BMP stdio file.
 *
 * gdImageCreateFromBmp() reads from the current position of inFile and does
 * not close it. On success, the returned image is owned by the caller and must
 * be destroyed with @ref gdImageDestroy.
 *
 * @param inFile Pointer to the BMP FILE stream to read.
 * @return A newly allocated image, or NULL on error.
 */
BGD_DECLARE(gdImagePtr) gdImageCreateFromBmp(FILE *inFile);

/**
 * @brief Create an image from a BMP memory buffer.
 *
 * gdImageCreateFromBmpPtr() borrows data for the duration of the call. The
 * caller retains ownership of the input buffer. On success, the returned image
 * is owned by the caller and must be destroyed with @ref gdImageDestroy.
 *
 * @param size Size of the BMP memory buffer in bytes.
 * @param data Pointer to the BMP memory buffer.
 * @return A newly allocated image, or NULL on error.
 */
BGD_DECLARE(gdImagePtr) gdImageCreateFromBmpPtr(int size, void *data);

/**
 * @brief Create an image from BMP data read through a gdIOCtx.
 *
 * gdImageCreateFromBmpCtx() reads from infile and does not close it. On
 * success, the returned image is owned by the caller and must be destroyed with
 * @ref gdImageDestroy.
 *
 * @param infile Pointer to the gdIOCtx input context.
 * @return A newly allocated image, or NULL on error.
 */
BGD_DECLARE(gdImagePtr) gdImageCreateFromBmpCtx(gdIOCtxPtr infile);

/** Descriptive facts read from a BMP file header. */
typedef struct {
	int header_type; /**< Type of the BMP header. */
	int width; /**< Image width in pixels. */
	int height; /**< Image height in pixels. */
	int top_down; /**< Nonzero if the image is stored top-down. */
	int planes; /**< Number of color planes. */
	int bits_per_pixel; /**< Number of bits per pixel. */
	int compression; /**< Compression method used. */
	int image_size; /**< Size of the pixel data in bytes. */
	int horizontal_resolution; /**< Horizontal resolution in pixels per meter. */
	int vertical_resolution; /**< Vertical resolution in pixels per meter. */
	int colors_used; /**< Number of colors used in the palette. */
	int important_colors; /**< Number of important colors. */
	int palette_type; /**< Type of the palette. */
	int palette_entries; /**< Number of entries in the palette. */
	unsigned int red_mask; /**< Red channel mask. */
	unsigned int green_mask; /**< Green channel mask. */
	unsigned int blue_mask; /**< Blue channel mask. */
	unsigned int alpha_mask; /**< Alpha channel mask. */
} gdBmpInfo;

/**
 * @brief Initialize a gdBmpInfo structure to default values.
 * 
 * @param info Pointer to the gdBmpInfo structure to initialize.
 */
BGD_DECLARE(void) gdBmpInfoInit(gdBmpInfo *info);

/**
 * @brief Read BMP file information from a stdio file.
 * 
 * @param infile Pointer to the BMP FILE stream to read.
 * @param info Pointer to a gdBmpInfo structure to receive the BMP information.
 * 
 * @return Returns 1 on success, or 0 on failure.
 */
BGD_DECLARE(int) gdBmpGetInfo(FILE *infile, gdBmpInfo *info);

/**
 * @brief Read BMP file information from a gdIOCtx.
 * 
 * @param infile Pointer to the gdIOCtx input context.
 * @param info Pointer to a gdBmpInfo structure to receive the BMP information.
 * 
 * @return Returns 1 on success, or 0 on failure.
 */
BGD_DECLARE(int) gdBmpGetInfoCtx(gdIOCtxPtr infile, gdBmpInfo *info);

/**
 * @brief Read BMP file information from a memory buffer.
 * 
 * @param size Size of the BMP memory buffer in bytes.
 * @param data Pointer to the BMP memory buffer.
 * @param info Pointer to a gdBmpInfo structure to receive the BMP information.
 * 
 * @return Returns 1 on success, or 0 on failure.
 */
BGD_DECLARE(int) gdBmpGetInfoPtr(int size, const void *data, gdBmpInfo *info);

/**
 * @brief Write an image as BMP data to a newly allocated memory buffer.
 *
 * gdImageBmpPtr() uses automatic BMP bit-depth selection. A zero compression
 * value writes uncompressed BMP data; a nonzero value requests legacy RLE
 * output when the automatically selected BMP bit depth supports it. The image
 * is borrowed for the duration of the call. On success, the returned buffer
 * must be freed with gdFree().
 *
 * @param im The image to write.
 * @param size Output location for the returned buffer size in bytes.
 * @param compression Legacy compression selector; zero disables RLE, nonzero
 *        requests RLE when supported by the selected output bit depth.
 * @return A newly allocated BMP buffer, or NULL on error.
 */
BGD_DECLARE(void *) gdImageBmpPtr(gdImagePtr im, int *size, int compression);

/**
 * @brief Write an image as BMP data to a stdio file.
 *
 * gdImageBmp() uses automatic BMP bit-depth selection. A zero compression
 * value writes uncompressed BMP data; a nonzero value requests legacy RLE
 * output when the automatically selected BMP bit depth supports it. The image
 * and outFile are borrowed for the duration of the call, and outFile is not
 * closed by gd.
 *
 * @param im The image to write.
 * @param outFile Pointer to the output FILE stream.
 * @param compression Legacy compression selector; zero disables RLE, nonzero
 *        requests RLE when supported by the selected output bit depth.
 */
BGD_DECLARE(void) gdImageBmp(gdImagePtr im, FILE *outFile, int compression);

/**
 * @brief Write an image as BMP data to a gdIOCtx.
 *
 * gdImageBmpCtx() uses automatic BMP bit-depth selection. A zero compression
 * value writes uncompressed BMP data; a nonzero value requests legacy RLE
 * output when the automatically selected BMP bit depth supports it. The image
 * and out context are borrowed for the duration of the call, and out is not
 * closed by gd.
 *
 * @param im The image to write.
 * @param out Pointer to the gdIOCtx output context.
 * @param compression Legacy compression selector; zero disables RLE, nonzero
 *        requests RLE when supported by the selected output bit depth.
 */
BGD_DECLARE(void) gdImageBmpCtx(gdImagePtr im, gdIOCtxPtr out, int compression);

/** @brief Write uncompressed BMP pixel data. */
#define GD_BMP_COMPRESS_NONE 0 
/** @brief Write BI_RLE8 compressed pixel data; valid only for 8 bpp output. */
#define GD_BMP_COMPRESS_RLE8 1
/** @brief Write BI_RLE4 compressed pixel data; valid only for 4 bpp output. */
#define GD_BMP_COMPRESS_RLE4 2

/** @brief Use default BMP writer behavior. */
#define GD_BMP_FLAG_NONE 0
/** @brief Force output to use a BITMAPV4HEADER. */
#define GD_BMP_FLAG_FORCE_V4HDR (1 << 0)
/** @brief Allow lossy truecolor-to-indexed conversion for 1, 4, or 8 bpp output. */
#define GD_BMP_FLAG_QUANTIZE (1 << 1)
/** @brief Use RGB555 bit masks instead of RGB565 for 16 bpp output. */
#define GD_BMP_FLAG_RGB555 (1 << 2)

/**
 * @brief Structured BMP writer options.
 */
typedef struct {
	int bits_per_pixel; /**< Requested output bit depth, or 0 for automatic selection. */
	int compression; /**< One of GD_BMP_COMPRESS_* values. */
	int flags; /**< Bitwise OR of GD_BMP_FLAG_* values. */
	const gdImageMetadata *metadata; /**< Reserved and ignored for BMP. */
} gdBmpWriteOptions;

/**
 * @brief Initialize a gdBmpWriteOptions structure to default values.
 * 
 * Updates or changes to the default values are not guaranteed to be compatible with future versions of gd. Callers should not assume that the default values will remain the same across versions.
 * This is not considered part of the API contract and may change without notice. Callers should always explicitly set the fields they care about after calling this function.
 */
BGD_DECLARE(void) gdBmpWriteOptionsInit(gdBmpWriteOptions *options);

/**
 * @brief Write an image as BMP data to a stdio file with explicit options.
 * 
 * @param im The image to write.
 * @param outFile Pointer to the output FILE stream.
 * @param options Pointer to a gdBmpWriteOptions structure specifying output options.
 * 
 * @return 0 on success, or a nonzero error code on failure.
 * 
 * @see gdBmpWriteOptionsInit gdBmpWriteOptions
 */
BGD_DECLARE(int) gdImageBmpWithOptions(gdImagePtr im, FILE *outFile, const gdBmpWriteOptions *options);

/**
 * @brief Write an image as BMP data to a gdIOCtx with explicit options.
 * 
 * @param im The image to write.
 * @param out Pointer to the gdIOCtx output context.
 * @param options Pointer to a gdBmpWriteOptions structure specifying output options.
 * 
 * @return 0 on success, or a nonzero error code on failure.
 * @see gdBmpWriteOptionsInit gdBmpWriteOptions
 */
BGD_DECLARE(int) gdImageBmpCtxWithOptions(gdImagePtr im, gdIOCtxPtr out, const gdBmpWriteOptions *options);

/**
 * @brief Write an image as BMP data to a newly allocated memory buffer with explicit options.
 * 
 * @param im The image to write.
 * @param size Output location for the returned buffer size in bytes.
 * @param options Pointer to a gdBmpWriteOptions structure specifying output options.
 * 
 * @return A newly allocated BMP buffer, or NULL on error. The caller is responsible for freeing the buffer with gdFree().
 * 
 * @see gdBmpWriteOptionsInit gdBmpWriteOptions
 */
BGD_DECLARE(void *) gdImageBmpPtrWithOptions(gdImagePtr im, int *size, const gdBmpWriteOptions *options);

/**
 * @brief Write an image as BMP data to a newly allocated memory buffer.
 *
 * gdImageBmpPtrEx() writes BMP output with explicit control over output bit
 * depth, compression, and writer flags. Pass bpp as 0 for automatic selection,
 * or as one of 1, 4, 8, 16, 24, or 32. Explicit indexed output from a
 * truecolor image is lossy and fails unless GD_BMP_FLAG_QUANTIZE is set. The
 * image is borrowed for the duration of the call. On success, the returned
 * buffer must be freed with gdFree().
 *
 * @param im The image to write.
 * @param size Output location for the returned buffer size in bytes.
 * @param bpp Requested output bit depth, or 0 for automatic selection.
 * @param compression One of GD_BMP_COMPRESS_NONE, GD_BMP_COMPRESS_RLE8, or
 *        GD_BMP_COMPRESS_RLE4.
 * @param flags Bitwise OR of GD_BMP_FLAG_* values.
 * @return A newly allocated BMP buffer, or NULL on error.
 */
BGD_DECLARE(void *)
gdImageBmpPtrEx(gdImagePtr im, int *size, int bpp, int compression, int flags);

/**
 * @brief Write an image as BMP data to a stdio file.
 *
 * gdImageBmpEx() writes BMP output with explicit control over output bit
 * depth, compression, and writer flags. Pass bpp as 0 for automatic selection,
 * or as one of 1, 4, 8, 16, 24, or 32. RLE4 is valid only for 4 bpp output and
 * RLE8 is valid only for 8 bpp output. The image and outFile are borrowed for
 * the duration of the call, and outFile is not closed by gd.
 *
 * @param im The image to write.
 * @param outFile Pointer to the output FILE stream.
 * @param bpp Requested output bit depth, or 0 for automatic selection.
 * @param compression One of GD_BMP_COMPRESS_NONE, GD_BMP_COMPRESS_RLE8, or
 *        GD_BMP_COMPRESS_RLE4.
 * @param flags Bitwise OR of GD_BMP_FLAG_* values.
 */
BGD_DECLARE(void)
gdImageBmpEx(gdImagePtr im, FILE *outFile, int bpp, int compression, int flags);

/**
 * @brief Write an image as BMP data to a gdIOCtx.
 *
 * gdImageBmpCtxEx() writes BMP output with explicit control over output bit
 * depth, compression, and writer flags. Pass bpp as 0 for automatic selection,
 * or as one of 1, 4, 8, 16, 24, or 32. For 16 bpp output, RGB565 masks are
 * used by default and GD_BMP_FLAG_RGB555 selects RGB555 masks. The image and
 * out context are borrowed for the duration of the call, and out is not closed
 * by gd.
 *
 * @param im The image to write.
 * @param out Pointer to the gdIOCtx output context.
 * @param bpp Requested output bit depth, or 0 for automatic selection.
 * @param compression One of GD_BMP_COMPRESS_NONE, GD_BMP_COMPRESS_RLE8, or
 *        GD_BMP_COMPRESS_RLE4.
 * @param flags Bitwise OR of GD_BMP_FLAG_* values.
 */
BGD_DECLARE(void)
gdImageBmpCtxEx(gdImagePtr im, gdIOCtxPtr out, int bpp, int compression, int flags);

/** @} */

/**
 * @defgroup gdCodecUhdr UltraHDR
 * @brief Read, transform, and write UltraHDR JPEG images.
 * @ingroup gdCodecs
 *
 * UltraHDR stores an SDR base image plus a gain map that allows HDR
 * reconstruction. gd's normal gdImage representation cannot safely represent
 * that gain map because it is not an ordinary 8-bit or palette bitmap; it may
 * be a floating-point-style image with metadata that must stay aligned with the
 * SDR base image. For that reason, UltraHDR support uses a separate opaque
 * gdUhdrImage handle and performs UltraHDR-aware operations through libultrahdr.
 *
 * Do not convert a gdUhdrImagePtr to gdImagePtr and expect to write it back as
 * UltraHDR. gdUhdrImageGetSdr() intentionally returns only a standard SDR
 * gdImagePtr view. That image can be inspected or saved as ordinary JPEG/PNG,
 * but it no longer carries the gain map needed to recreate a valid UltraHDR
 * image. Future GD versions may add internal image formats that can represent
 * the gain map directly; until then, use gdUhdrImageResize(),
 * gdUhdrImageCrop(), gdUhdrImageRotate(), and gdUhdrImageMirror() to queue
 * supported UltraHDR-preserving transformations.
 *
 * @code{.c}
 * gdUhdrImagePtr im;
 * gdImagePtr sdr;
 * gdUhdrError err;
 * int rc;
 *
 * im = gdUhdrImageCreateFromFile("input.jpg", GD_UHDR_FORMAT_JPEG, &err);
 * if (im == NULL) {
 *     return 1;
 * }
 *
 * rc = gdUhdrImageResize(im, 640, 360, &err);
 * if (rc != GD_UHDR_SUCCESS) {
 *     gdUhdrImageDestroy(im);
 *     return 1;
 * }
 *
 * rc = gdUhdrImageFile(im, "output.jpg", GD_UHDR_FORMAT_JPEG, 90, &err);
 * if (rc != GD_UHDR_SUCCESS) {
 *     gdUhdrImageDestroy(im);
 *     return 1;
 * }
 *
 * sdr = gdUhdrImageGetSdr(im, &err);
 * if (sdr != NULL) {
 *     gdImageDestroy(sdr);
 * }
 *
 * gdUhdrImageDestroy(im);
 * @endcode
 *
 * @{
 */

/** @name UltraHDR Status Codes */
/** @{ */

#define GD_UHDR_SUCCESS 0        /**< Operation succeeded. */
#define GD_UHDR_NOT_AVAILABLE -1 /**< libgd was built without UltraHDR support. */
#define GD_UHDR_E_INVALID -2     /**< Invalid argument or state. */
#define GD_UHDR_E_UNSUPPORTED -3 /**< Unsupported format or operation. */
#define GD_UHDR_E_ENCODE -4      /**< Encode failure. */
#define GD_UHDR_E_DECODE -5      /**< Decode failure. */
/** @} */

/** @name UltraHDR Transform Constants */
/** @{ */

/** Mirror an UltraHDR image horizontally. */
#define GD_UHDR_MIRROR_HORIZONTAL 0
/** Mirror an UltraHDR image vertically. */
#define GD_UHDR_MIRROR_VERTICAL 1

/** @} */

/** @name UltraHDR Types */
/** @{ */

/** @brief UltraHDR container format selector. */
typedef enum {
    GD_UHDR_FORMAT_JPEG = 0, /**< UltraHDR JPEG, currently supported. */
    GD_UHDR_FORMAT_WEBP = 1, /**< Reserved for future WebP-based UltraHDR support. */
    GD_UHDR_FORMAT_HEIF = 2  /**< Reserved for future HEIF-based UltraHDR support. */
} gdUhdrFormat;

/** @brief Opaque UltraHDR image handle. */
typedef struct gdUhdrImageStruct gdUhdrImage;

/** @brief Pointer to an opaque UltraHDR image handle. */
typedef gdUhdrImage *gdUhdrImagePtr;

/** @brief Structured error details for UltraHDR APIs. */
typedef struct {
    int code;           /**< libgd UltraHDR status code, one of the GD_UHDR_* values. */
    int provider_code;  /**< Underlying libultrahdr provider error code, if any. */
    char message[128];  /**< Optional human-readable error detail. */
} gdUhdrError;

/** @brief Pointer to a gdUhdrError structure. */
typedef gdUhdrError *gdUhdrErrorPtr;

/** @} */

/** @name UltraHDR Reading */
/** @{ */

/**
 * @brief Create an UltraHDR image handle from a file path.
 *
 * gdUhdrImageCreateFromFile() reads filename and validates that it is an
 * UltraHDR image in the selected format. Currently only GD_UHDR_FORMAT_JPEG is
 * supported. The returned handle is owned by the caller and must be destroyed
 * with gdUhdrImageDestroy(). If err is not NULL, it receives status details.
 *
 * @param filename Path to the UltraHDR input file.
 * @param format Input format, currently GD_UHDR_FORMAT_JPEG.
 * @param err Optional pointer to receive detailed error information.
 * @return A new UltraHDR image handle, or NULL on error.
 */
BGD_DECLARE(gdUhdrImagePtr)
gdUhdrImageCreateFromFile(const char *filename, int format, gdUhdrErrorPtr err);

/**
 * @brief Create an UltraHDR image handle from an IO context.
 *
 * gdUhdrImageCreateFromCtx() reads all data from ctx but does not close it.
 * Currently only GD_UHDR_FORMAT_JPEG is supported. The returned handle is owned
 * by the caller and must be destroyed with gdUhdrImageDestroy(). If err is not
 * NULL, it receives status details.
 *
 * @param ctx Input IO context.
 * @param format Input format, currently GD_UHDR_FORMAT_JPEG.
 * @param err Optional pointer to receive detailed error information.
 * @return A new UltraHDR image handle, or NULL on error.
 */
BGD_DECLARE(gdUhdrImagePtr)
gdUhdrImageCreateFromCtx(gdIOCtxPtr ctx, int format, gdUhdrErrorPtr err);

/**
 * @brief Create an UltraHDR image handle from memory.
 *
 * gdUhdrImageCreateFromPtr() reads size bytes from data without taking
 * ownership of the buffer. Currently only GD_UHDR_FORMAT_JPEG is supported. The
 * returned handle is owned by the caller and must be destroyed with
 * gdUhdrImageDestroy(). If err is not NULL, it receives status details.
 *
 * @param size Size of data in bytes.
 * @param data Pointer to UltraHDR data.
 * @param format Input format, currently GD_UHDR_FORMAT_JPEG.
 * @param err Optional pointer to receive detailed error information.
 * @return A new UltraHDR image handle, or NULL on error.
 */
BGD_DECLARE(gdUhdrImagePtr)
gdUhdrImageCreateFromPtr(int size, void *data, int format, gdUhdrErrorPtr err);

/**
 * @brief Destroy an UltraHDR image handle.
 *
 * Use this for handles returned by gdUhdrImageCreateFromFile(),
 * gdUhdrImageCreateFromCtx(), or gdUhdrImageCreateFromPtr(). Passing NULL is
 * allowed.
 *
 * @param im UltraHDR image handle to destroy.
 */
BGD_DECLARE(void) gdUhdrImageDestroy(gdUhdrImagePtr im);

/** @} */

/** @} */

/*
  Group: Types

  typedef: gdSource

  typedef: gdSourcePtr

        *Note:* This interface is *obsolete* and kept only for
        *compatibility.  Use <gdIOCtx> instead.

        Represents a source from which a PNG can be read. Programmers who
        do not wish to read PNGs from a file can provide their own
        alternate input mechanism, using the @ref gdImageCreateFromPngSource
        function. See the documentation of that function for an example of
        the proper use of this type.

        > typedef struct {
        >         int (*source) (void *context, char *buffer, int len);
        >         void *context;
        > } gdSource, *gdSourcePtr;

        The source function must return -1 on error, otherwise the number
        of bytes fetched. 0 is EOF, not an error!

   'context' will be passed to your source function.

*/
/** @deprecated in favor of gdIOCtx */
typedef struct {
    int (*source)(void *context, char *buffer, int len);
    void *context;
} gdSource, *gdSourcePtr;

/** @deprecated in favor of gdImageCreateFromPngCtx */
BGD_DECLARE(gdImagePtr) gdImageCreateFromPngSource(gdSourcePtr in);

/** @deprecated for completeness with Sink 2.x APIs, will be removed in 3.0 with all Sink APIs */
BGD_DECLARE(gdImagePtr) gdImageCreateFromQoiSource(gdSourcePtr in);

/**
 * @defgroup gdCodecGd GD
 * @brief Read and write libgd's native .gd image format.
 * @ingroup gdCodecs
 *
 * The GD format is libgd's own historical image dump format. It is obsolete
 * for interchange and should generally be used only for development, testing,
 * or compatibility with existing .gd assets. For compressed or portable image
 * exchange, prefer formats such as PNG, JPEG, WebP, or AVIF.
 *
 * gd can read GD 1.x palette .gd files and GD 2.x palette or truecolor .gd
 * files. The writer always emits the GD 2.x .gd format, not the related GD2
 * chunked format documented separately by the GD2 APIs. Returned gdImagePtr
 * images are owned by the caller and must be destroyed with @ref gdImageDestroy.
 *
 * @code{.c}
 * gdImagePtr im, roundtrip;
 * void *data;
 * int size;
 *
 * im = gdImageCreate(100, 100);
 * gdImageColorAllocate(im, 255, 255, 255);
 * gdImageColorAllocate(im, 0, 0, 0);
 * gdImageLine(im, 0, 0, 99, 99, 1);
 *
 * data = gdImageGdPtr(im, &size);
 * if (data != NULL) {
 *        roundtrip = gdImageCreateFromGdPtr(size, data);
 *        gdFree(data);
 *        if (roundtrip != NULL) {
 *                gdImageDestroy(roundtrip);
 *        }
 * }
 *
 * gdImageDestroy(im);
 * @endcode
 *
 * @{
 */

/** @name GD Reading */
/** @{ */

/**
 * @brief Create an image from a GD stdio file.
 *
 * gdImageCreateFromGd() reads from the current position of in and does not
 * close it. On success, the returned image is owned by the caller and must be
 * destroyed with @ref gdImageDestroy.
 *
 * @param in Pointer to the GD FILE stream to read.
 * @return A newly allocated image, or NULL on error.
 */
BGD_DECLARE(gdImagePtr) gdImageCreateFromGd(FILE *in);

/**
 * @brief Create an image from GD data read through a gdIOCtx.
 *
 * gdImageCreateFromGdCtx() reads from in and does not close it. On success,
 * the returned image is owned by the caller and must be destroyed with
 * @ref gdImageDestroy.
 *
 * @param in Pointer to the gdIOCtx input context.
 * @return A newly allocated image, or NULL on error.
 */
BGD_DECLARE(gdImagePtr) gdImageCreateFromGdCtx(gdIOCtxPtr in);

/**
 * @brief Create an image from a GD memory buffer.
 *
 * gdImageCreateFromGdPtr() borrows data for the duration of the call. The
 * caller retains ownership of the input buffer. On success, the returned image
 * is owned by the caller and must be destroyed with @ref gdImageDestroy.
 *
 * @param size Size of the GD memory buffer in bytes.
 * @param data Pointer to the GD memory buffer.
 * @return A newly allocated image, or NULL on error.
 */
BGD_DECLARE(gdImagePtr) gdImageCreateFromGdPtr(int size, void *data);

/** @} */

/** @name GD Writing */
/** @{ */

/**
 * @brief Write an image as GD data to a newly allocated memory buffer.
 *
 * gdImageGdPtr() writes the image in GD 2.x .gd format. The image is borrowed
 * for the duration of the call. On success, the returned buffer is owned by the
 * caller and must be freed with gdFree().
 *
 * @param im The image to write.
 * @param size Output location for the returned buffer size in bytes.
 * @return A newly allocated GD buffer, or NULL on error.
 */
BGD_DECLARE(void *) gdImageGdPtr(gdImagePtr im, int *size);

/**
 * @brief Write an image as GD data to a stdio file.
 *
 * gdImageGd() writes the image in GD 2.x .gd format. The image and out stream
 * are borrowed for the duration of the call, and out is not closed by gd.
 *
 * @param im The image to write.
 * @param out Pointer to the output FILE stream.
 */
BGD_DECLARE(void) gdImageGd(gdImagePtr im, FILE *out);

/** @} */

/** @} */

/**
 * @defgroup gdCodecGd2 GD2
 * @brief Read and write libgd's chunked native .gd2 image format.
 * @ingroup gdCodecs
 * @deprecated GD and GD2 formats are in favor of more suitable and future proof like QOI or WebP for lossless usage or similar.
 *
 * GD2 is libgd's historical chunked image dump format. It is obsolete for
 * general interchange and should generally be used only for development,
 * testing, or compatibility with existing .gd2 assets. Unlike the simpler
 * @ref gdCodecGd format, GD2 stores image data in chunks and can read a
 * rectangular region without decoding the entire image. Compressed GD2 support
 * requires libz; when GD2 support is not available the functions fail and
 * report an error through gd's error mechanism.
 *
 * GD2 readers accept palette and truecolor GD2 files. The whole-image readers
 * return the full image, while the part readers return a newly allocated image
 * containing the requested rectangle. Returned gdImagePtr images are owned by
 * the caller and must be destroyed with @ref gdImageDestroy. The writer emits GD2
 * data using a public format selector of GD2_FMT_RAW or GD2_FMT_COMPRESSED;
 * truecolor images are written with the corresponding internal truecolor GD2
 * format automatically.
 *
 * @code{.c}
 * gdImagePtr im, roundtrip;
 * void *data;
 * int size;
 *
 * im = gdImageCreate(100, 100);
 * gdImageColorAllocate(im, 255, 255, 255);
 * gdImageColorAllocate(im, 0, 0, 0);
 * gdImageLine(im, 0, 0, 99, 99, 1);
 *
 * data = gdImageGd2Ptr(im, GD2_CHUNKSIZE, GD2_FMT_COMPRESSED, &size);
 * if (data != NULL) {
 *        roundtrip = gdImageCreateFromGd2PartPtr(size, data, 0, 0, 50, 50);
 *        gdFree(data);
 *        if (roundtrip != NULL) {
 *                gdImageDestroy(roundtrip);
 *        }
 * }
 *
 * gdImageDestroy(im);
 * @endcode
 *
 * @{
 */

/** @name GD2 Reading */
/** @{ */

/**
 * @brief Create an image from a GD2 stdio file.
 *
 * gdImageCreateFromGd2() reads from the current position of in and does not
 * close it. On success, the returned image is owned by the caller and must be
 * destroyed with @ref gdImageDestroy.
 *
 * @param in Pointer to the GD2 FILE stream to read.
 * @return A newly allocated image, or NULL on error.
 */
BGD_DECLARE(gdImagePtr) gdImageCreateFromGd2(FILE *in);

/**
 * @brief Create an image from GD2 data read through a gdIOCtx.
 *
 * gdImageCreateFromGd2Ctx() reads from in and does not close it. On success,
 * the returned image is owned by the caller and must be destroyed with
 * @ref gdImageDestroy.
 *
 * @param in Pointer to the gdIOCtx input context.
 * @return A newly allocated image, or NULL on error.
 */
BGD_DECLARE(gdImagePtr) gdImageCreateFromGd2Ctx(gdIOCtxPtr in);

/**
 * @brief Create an image from a GD2 memory buffer.
 *
 * gdImageCreateFromGd2Ptr() borrows data for the duration of the call. The
 * caller retains ownership of the input buffer. On success, the returned image
 * is owned by the caller and must be destroyed with @ref gdImageDestroy.
 *
 * @param size Size of the GD2 memory buffer in bytes.
 * @param data Pointer to the GD2 memory buffer.
 * @return A newly allocated image, or NULL on error.
 */
BGD_DECLARE(gdImagePtr) gdImageCreateFromGd2Ptr(int size, void *data);

/**
 * @brief Create an image from a rectangular region of a GD2 stdio file.
 *
 * gdImageCreateFromGd2Part() reads the region beginning at srcx, srcy with
 * dimensions w by h. The input stream is borrowed and is not closed by gd. On
 * success, the returned image is owned by the caller and must be destroyed with
 * @ref gdImageDestroy.
 *
 * @param in Pointer to the GD2 FILE stream to read.
 * @param srcx Left coordinate of the source rectangle.
 * @param srcy Top coordinate of the source rectangle.
 * @param w Width of the source rectangle in pixels.
 * @param h Height of the source rectangle in pixels.
 * @return A newly allocated image containing the requested region, or NULL on error.
 */
BGD_DECLARE(gdImagePtr)
gdImageCreateFromGd2Part(FILE *in, int srcx, int srcy, int w, int h);

/**
 * @brief Create an image from a rectangular GD2 region read through a gdIOCtx.
 *
 * gdImageCreateFromGd2PartCtx() reads the region beginning at srcx, srcy with
 * dimensions w by h. The input context is borrowed and is not closed by gd. On
 * success, the returned image is owned by the caller and must be destroyed with
 * @ref gdImageDestroy.
 *
 * @param in Pointer to the gdIOCtx input context.
 * @param srcx Left coordinate of the source rectangle.
 * @param srcy Top coordinate of the source rectangle.
 * @param w Width of the source rectangle in pixels.
 * @param h Height of the source rectangle in pixels.
 * @return A newly allocated image containing the requested region, or NULL on error.
 */
BGD_DECLARE(gdImagePtr)
gdImageCreateFromGd2PartCtx(gdIOCtxPtr in, int srcx, int srcy, int w, int h);

/**
 * @brief Create an image from a rectangular region of a GD2 memory buffer.
 *
 * gdImageCreateFromGd2PartPtr() borrows data for the duration of the call. The
 * caller retains ownership of the input buffer. On success, the returned image
 * is owned by the caller and must be destroyed with @ref gdImageDestroy.
 *
 * @param size Size of the GD2 memory buffer in bytes.
 * @param data Pointer to the GD2 memory buffer.
 * @param srcx Left coordinate of the source rectangle.
 * @param srcy Top coordinate of the source rectangle.
 * @param w Width of the source rectangle in pixels.
 * @param h Height of the source rectangle in pixels.
 * @return A newly allocated image containing the requested region, or NULL on error.
 */
BGD_DECLARE(gdImagePtr)
gdImageCreateFromGd2PartPtr(int size, void *data, int srcx, int srcy, int w, int h);

/** @} */

/** @} */

/**
 * @defgroup gdCodecXbm XBM
 * @brief Read and write X11 bitmap images.
 * @ingroup gdCodecs
 *
 * XBM support reads X11 bitmap data from an open stdio stream and writes XBM
 * text to a gd IO context. XBM images are 1-bit images stored as C source-style
 * data, and gd maps them to palette images when reading. The reader returns a
 * new image owned by the caller; the writer borrows both the image and output
 * context for the duration of the call.
 *
 * @code{.c}
 * FILE *in;
 * gdImagePtr im;
 * gdIOCtxPtr out;
 *
 * in = fopen("icon.xbm", "rb");
 * if (in == NULL) {
 *     return 1;
 * }
 *
 * im = gdImageCreateFromXbm(in);
 * fclose(in);
 * if (im == NULL) {
 *     return 1;
 * }
 *
 * out = gdNewFileCtx(stdout);
 * if (out == NULL) {
 *     gdImageDestroy(im);
 *     return 1;
 * }
 *
 * gdImageXbmCtx(im, "icon.xbm", 1, out);
 * out->gd_free(out);
 * gdImageDestroy(im);
 * @endcode
 *
 * @{
 */

/** @name XBM Reading */
/** @{ */

/**
 * @brief Create an image from XBM data in a stdio stream.
 *
 * gdImageCreateFromXbm() rewinds in before reading and does not close it. X11
 * XBM data with char arrays and X10 XBM data with short arrays are supported.
 * On success, the returned image is owned by the caller and must be destroyed
 * with @ref gdImageDestroy.
 *
 * @param in Pointer to the input FILE stream.
 * @return A newly allocated image, or NULL on error.
 */
BGD_DECLARE(gdImagePtr) gdImageCreateFromXbm(FILE *in);

/** @} */

/** @name XBM Writing */
/** @{ */

/**
 * @brief Write an image to an IO context in X11 bitmap format.
 *
 * gdImageXbmCtx() does not close out. The image and output context are borrowed
 * for the duration of the call. Pixels whose color index equals fg are written
 * as set bits; all other pixels are written as unset bits.
 *
 * @param image The image to write.
 * @param file_name Prefix for the generated XBM C identifiers. Path components,
 *                  a trailing .xbm extension, and unsupported identifier
 *                  characters are normalized before writing.
 * @param fg Foreground color index to write as set bits.
 * @param out The output IO context.
 */
BGD_DECLARE(void)
gdImageXbmCtx(gdImagePtr image, char *file_name, int fg, gdIOCtxPtr out);

/** @} */

/** @} */

/**
 * @defgroup gdCodecXpm XPM
 * @brief Read and write X PixMap images.
 * @ingroup gdCodecs
 */
 /** 
 * @brief Read X PixMap images.
 * @ingroup gdCodecs
 *
 * XPM support reads X PixMap files through libXpm and returns palette images.
 * Unlike most gd image readers, the XPM API takes a filename string rather than
 * a FILE pointer, memory buffer, or gd IO context. The returned image is owned
 * by the caller. gd does not provide an XPM writer.
 *
 * @code{.c}
 * gdImagePtr im;
 *
 * im = gdImageCreateFromXpm("icon.xpm");
 * if (im == NULL) {
 *     return 1;
 * }
 * 
 * gdImageDestroy(im);
 * @endcode
*/

/** @name XPM Reading */
/** @{ */

/**
 * @brief Create a palette image from an XPM file.
 *
 * gdImageCreateFromXpm() reads filename directly through libXpm. The input is a
 * filename, not an open FILE stream. On success, the returned image is owned by
 * the caller and must be destroyed with @ref gdImageDestroy.
 *
 * @param filename Path to the XPM file to read.
 * @return A newly allocated palette image, or NULL on error.
 */
BGD_DECLARE(gdImagePtr) gdImageCreateFromXpm(char *filename);

/** @} */

/** @} */

/**
 * @brief Write an image as WBMP data to a stdio file.
 * @ingroup gdCodecWbmp
 *
 * gdImageWBMP() does not close out. The image is borrowed for the duration of
 * the call. Pixels whose color equals fg are written as black; all other
 * pixels are written as white.
 *
 * @param image The image to write.
 * @param fg Foreground color value to write as black.
 * @param out Pointer to the output FILE stream.
 */
BGD_DECLARE(void) gdImageWBMP(gdImagePtr image, int fg, FILE *out);

/**
 * @brief Write an image as WBMP data to a gdIOCtx.
 * @ingroup gdCodecWbmp
 *
 * gdImageWBMPCtx() does not close out. The image is borrowed for the duration
 * of the call. Pixels whose color equals fg are written as black; all other
 * pixels are written as white.
 *
 * @param image The image to write.
 * @param fg Foreground color value to write as black.
 * @param out Pointer to the gdIOCtx output context.
 */
BGD_DECLARE(void) gdImageWBMPCtx(gdImagePtr image, int fg, gdIOCtxPtr out);

/**
 * @addtogroup gdCodecUhdr
 * @{
 */

/** @name UltraHDR Availability and Inspection */
/** @{ */

/**
 * @brief Return whether UltraHDR support is available in this build.
 * @ingroup gdCodecUhdr
 *
 * This reports whether libgd was built with libultrahdr support.
 *
 * @return 1 when UltraHDR support is available, or 0 otherwise.
 */
BGD_DECLARE(int) gdUhdrIsAvailable(void);

/**
 * @brief Return the UltraHDR image width.
 * @ingroup gdCodecUhdr
 *
 * @param im UltraHDR image handle.
 * @return Image width in pixels, or 0 for NULL.
 */
BGD_DECLARE(int) gdUhdrImageWidth(gdUhdrImagePtr im);

/**
 * @brief Return the UltraHDR image height.
 * @ingroup gdCodecUhdr
 *
 * @param im UltraHDR image handle.
 * @return Image height in pixels, or 0 for NULL.
 */
BGD_DECLARE(int) gdUhdrImageHeight(gdUhdrImagePtr im);

/**
 * @brief Return whether the UltraHDR image has a gain map.
 * @ingroup gdCodecUhdr
 *
 * @param im UltraHDR image handle.
 * @return 1 if im has a gain map, or 0 otherwise.
 */
BGD_DECLARE(int) gdUhdrImageHasGainMap(gdUhdrImagePtr im);

/** @} */

/** @name UltraHDR Transform Queue */
/** @{ */

/**
 * @brief Queue an UltraHDR-preserving resize operation.
 * @ingroup gdCodecUhdr
 *
 * The operation is recorded on im and applied when the image is written. The
 * SDR base image and gain map are transformed together so the output can remain
 * a valid UltraHDR image.
 *
 * @param im UltraHDR image handle.
 * @param width Output width in pixels.
 * @param height Output height in pixels.
 * @param err Optional pointer to receive detailed error information.
 * @return GD_UHDR_SUCCESS on success, or another GD_UHDR_* status code on error.
 */
BGD_DECLARE(int)
gdUhdrImageResize(gdUhdrImagePtr im, int width, int height, gdUhdrErrorPtr err);

/**
 * @brief Queue an UltraHDR-preserving crop operation.
 * @ingroup gdCodecUhdr
 *
 * The operation is recorded on im and applied when the image is written. The
 * SDR base image and gain map are cropped together so the output can remain a
 * valid UltraHDR image.
 *
 * @param im UltraHDR image handle.
 * @param left Left edge of the crop rectangle in pixels.
 * @param top Top edge of the crop rectangle in pixels.
 * @param width Crop width in pixels.
 * @param height Crop height in pixels.
 * @param err Optional pointer to receive detailed error information.
 * @return GD_UHDR_SUCCESS on success, or another GD_UHDR_* status code on error.
 */
BGD_DECLARE(int)
gdUhdrImageCrop(gdUhdrImagePtr im, int left, int top, int width, int height, gdUhdrErrorPtr err);

/**
 * @brief Queue an UltraHDR-preserving right-angle rotation.
 * @ingroup gdCodecUhdr
 *
 * The operation is recorded on im and applied when the image is written. The
 * SDR base image and gain map are rotated together so the output can remain a
 * valid UltraHDR image. Supported angles are 0, 90, 180, and 270 degrees.
 *
 * @param im UltraHDR image handle.
 * @param degrees Rotation angle in degrees.
 * @param err Optional pointer to receive detailed error information.
 * @return GD_UHDR_SUCCESS on success, or another GD_UHDR_* status code on error.
 */
BGD_DECLARE(int)
gdUhdrImageRotate(gdUhdrImagePtr im, int degrees, gdUhdrErrorPtr err);

/**
 * @brief Queue an UltraHDR-preserving mirror operation.
 * @ingroup gdCodecUhdr
 *
 * The operation is recorded on im and applied when the image is written. The
 * SDR base image and gain map are mirrored together so the output can remain a
 * valid UltraHDR image.
 *
 * @param im UltraHDR image handle.
 * @param axis Mirror axis, GD_UHDR_MIRROR_HORIZONTAL or GD_UHDR_MIRROR_VERTICAL.
 * @param err Optional pointer to receive detailed error information.
 * @return GD_UHDR_SUCCESS on success, or another GD_UHDR_* status code on error.
 */
BGD_DECLARE(int)
gdUhdrImageMirror(gdUhdrImagePtr im, int axis, gdUhdrErrorPtr err);

/** @} */

/** @name UltraHDR Writing */
/** @{ */

/**
 * @brief Write an UltraHDR image to a file path.
 * @ingroup gdCodecUhdr
 *
 * Currently only GD_UHDR_FORMAT_JPEG is supported. If no transform operations
 * were queued, gd writes the original compressed UltraHDR data through. If
 * transforms were queued, gd uses libultrahdr to decode the base image and gain
 * map, applies the queued operations to both, and re-encodes an UltraHDR JPEG.
 *
 * @param im UltraHDR image handle to write.
 * @param filename Output file path.
 * @param format Output format, currently GD_UHDR_FORMAT_JPEG.
 * @param quality JPEG quality from 1 to 95.
 * @param err Optional pointer to receive detailed error information.
 * @return GD_UHDR_SUCCESS on success, or another GD_UHDR_* status code on error.
 */
BGD_DECLARE(int)
gdUhdrImageFile(gdUhdrImagePtr im, const char *filename, int format, int quality,
                gdUhdrErrorPtr err);

/**
 * @brief Write an UltraHDR image to an IO context.
 * @ingroup gdCodecUhdr
 *
 * gdUhdrImageCtx() does not close ctx. Currently only GD_UHDR_FORMAT_JPEG is
 * supported. If no transform operations were queued, gd writes the original
 * compressed UltraHDR data through. If transforms were queued, gd uses
 * libultrahdr to produce a new UltraHDR JPEG with the gain map preserved.
 *
 * @param im UltraHDR image handle to write.
 * @param ctx Output IO context.
 * @param format Output format, currently GD_UHDR_FORMAT_JPEG.
 * @param quality JPEG quality from 1 to 95.
 * @param err Optional pointer to receive detailed error information.
 * @return GD_UHDR_SUCCESS on success, or another GD_UHDR_* status code on error.
 */
BGD_DECLARE(int)
gdUhdrImageCtx(gdUhdrImagePtr im, gdIOCtxPtr ctx, int format, int quality, gdUhdrErrorPtr err);

/**
 * @brief Write an UltraHDR image to a newly allocated memory buffer.
 * @ingroup gdCodecUhdr
 *
 * Currently only GD_UHDR_FORMAT_JPEG is supported. On success, the returned
 * buffer is owned by the caller and must be freed with gdFree().
 *
 * @param im UltraHDR image handle to write.
 * @param size Pointer that receives the encoded buffer size in bytes.
 * @param format Output format, currently GD_UHDR_FORMAT_JPEG.
 * @param quality JPEG quality from 1 to 95.
 * @param err Optional pointer to receive detailed error information.
 * @return A newly allocated UltraHDR data buffer, or NULL on error.
 */
BGD_DECLARE(void *)
gdUhdrImageWritePtr(gdUhdrImagePtr im, int *size, int format, int quality, gdUhdrErrorPtr err);

/** @} */

/** @name UltraHDR SDR Extraction */
/** @{ */

/**
 * @brief Decode the SDR view of an UltraHDR image as a gdImage.
 * @ingroup gdCodecUhdr
 *
 * The returned gdImagePtr is caller-owned and must be destroyed with
 * @ref gdImageDestroy. It is an SDR image only: it does not contain the UltraHDR
 * gain map and cannot be used to recreate an UltraHDR image. Use the
 * gdUhdrImage* transform and write APIs when the gain map must be preserved.
 *
 * @param im UltraHDR image handle to decode.
 * @param err Optional pointer to receive detailed error information.
 * @return A newly allocated truecolor SDR image, or NULL on error.
 */
BGD_DECLARE(gdImagePtr)
gdUhdrImageGetSdr(gdUhdrImagePtr im, gdUhdrErrorPtr err);

/** @} */

/** @} */

/**
 * @defgroup gdImageFileIO Image File Convenience APIs
 * @brief Convenience APIs for reading and writing images by filename or signature.
 *
 * These APIs are helpers around the format-specific codec functions. For
 * reading existing files, @ref gdImageReadFile is preferred over
 * @ref gdImageCreateFromFile because it detects image type from binary
 * signatures rather than trusting the filename extension.
 *
 * @{
 */

/**
 * @brief Create an image from a file using the filename extension.
 *
 * gdImageCreateFromFile() chooses the reader from the filename extension using
 * the same extension table as @ref gdSupportsFileType. The returned image is
 * caller-owned and must be destroyed with gdImageDestroy().
 *
 * @note For new code, prefer @ref gdImageReadFile when reading existing files.
 * gdImageReadFile() checks binary signatures instead of trusting the filename
 * extension, which is safer and more reliable when files are mislabeled or
 * supplied by users.
 *
 * @param filename Path to the input image file.
 * @return A newly allocated image, or NULL on error or unsupported extension.
 */
BGD_DECLARE(gdImagePtr) gdImageCreateFromFile(const char *filename);

/**
 * @brief Read an image file by probing its binary signature.
 *
 * gdImageReadFile() opens filename, reads the first bytes of the file, detects
 * the image format by known binary signatures, and dispatches to the matching
 * codec reader. It falls back to the filename-based XPM reader and FILE-based
 * XBM reader for those formats because they do not have gdIOCtx readers.
 *
 * Supported detected formats depend on the codecs compiled into gd and include
 * PNG, JPEG, GIF, BMP, TIFF, WebP, AVIF, HEIC, JXL, GD, GD2, QOI, XPM, and XBM.
 * The returned image is caller-owned and must be destroyed with
 * gdImageDestroy().
 *
 * @param filename Path to the input image file.
 * @return A newly allocated image, or NULL on error, unknown format, disabled
 *         codec, or decode failure.
 */
BGD_DECLARE(gdImagePtr) gdImageReadFile(const char *filename);

/**
 * @brief Read an image from a gdIOCtx by probing its binary signature.
 *
 * gdImageReadCtx() borrows ctx for the duration of the call and does not close
 * it. The input context must support seeking because gd reads a probe buffer
 * and then seeks back to the start before dispatching to the codec reader.
 * Formats without gdIOCtx readers, currently XPM and XBM, are not supported by
 * this function; use @ref gdImageReadFile for those file formats.
 *
 * The returned image is caller-owned and must be destroyed with
 * gdImageDestroy().
 *
 * @param ctx Pointer to the input gdIOCtx.
 * @return A newly allocated image, or NULL on error, unknown format, disabled
 *         codec, unsupported context reader, or decode failure.
 */
BGD_DECLARE(gdImagePtr) gdImageReadCtx(gdIOCtxPtr ctx);

/**
 * @brief Status values for extended automatic image readers.
 */
typedef enum {
	gdImageReadStatusOk = 0, /**< The image was read successfully. */
	gdImageReadStatusUnrecognized, /**< No known binary signature matched the input. */
	gdImageReadStatusUnsupportedFormat, /**< The signature matched a format that cannot be read through the requested API. */
	gdImageReadStatusCodecUnavailable, /**< The matching codec is not available in this gd build. */
	gdImageReadStatusDecodeFailed /**< The matching codec was available but failed to decode the image. */
} gdImageReadStatus;

/** Restrict extended automatic reading to codec APIs that match the input source. */
#define GD_IMAGE_READ_RESTRICT_CODEC_API 1

/**
 * @brief Read an image from a gdIOCtx with extended status information.
 *
 * gdImageReadCtxEx() is the extended form of @ref gdImageReadCtx. It accepts
 * option flags and can report a gdImageReadStatus value and detected format
 * name to the caller. The input context is borrowed for the duration of the
 * call and is not closed.
 *
 * @param ctx Pointer to the input gdIOCtx.
 * @param flags Bitmask of GD_IMAGE_READ_* flags.
 * @param status Optional pointer that receives the read status.
 * @param format_name Optional pointer that receives the detected format name.
 * @return A newly allocated image, or NULL on error.
 */
BGD_DECLARE(gdImagePtr) gdImageReadCtxEx(gdIOCtxPtr ctx, int flags, gdImageReadStatus *status, const char **format_name);

/**
 * @brief Write an image to a file in the format indicated by the filename.
 * 
 * File type is determined by the extension of the file name. See @ref gdSupportsFileType for an overview of the parsing.
 * This is appropriate for writing, where the filename normally chooses the
 * intended output format.
 * 
 * For file types that require extra arguments, gdImageFile() attempts to use sane defaults:
 * - @ref gdImageGd2 - chunk size = 0, compression is enabled.
 * - @ref gdImageJpeg - quality = -1 (i.e. the reasonable default)
 * - @ref gdImageWBMP - foreground is the darkest available color
 * 
 * Everything else is called with the two-argument function and so will use the default values.
 * @ref gdImageFile has some rudimentary error detection and will return GD_FALSE (0) if a detectable error occurred.
 * However, the image loaders do not normally return their error status so a result of GD_TRUE (1) does **not** mean the file was saved successfully.
 * 
 * @param im The image to save.
 * @param filename The path to the file to which the image is saved.
 * @return GD_TRUE on apparent success, or GD_FALSE if the filename extension
 *         is unsupported or the output file could not be opened.
 */
BGD_DECLARE(int) gdImageFile(gdImagePtr im, const char *filename);

/**
 * @brief Test if a given file type is supported by GD.
 * 
 * gdSupportsFileType() tests the filename extension using the same extension
 * table as @ref gdImageCreateFromFile and @ref gdImageFile. The file does not
 * need to exist. If writing is nonzero, the function returns true only when
 * gdImageFile() can write that extension; otherwise it returns true when
 * gdImageCreateFromFile() can read that extension.
 *
 * Extension parsing has the same limitations as the extension-based helpers.
 * Use @ref gdImageReadFile when reading an actual file and reliable format
 * detection matters.
 *
 * @param filename Filename whose extension should be tested.
 * @param writing Nonzero to test write support; zero to test read support.
 * @return GD_TRUE (1) if the file type is supported, GD_FALSE (0) if not.
 */
BGD_DECLARE(int) gdSupportsFileType(const char *filename, int writing);

/** @} */

/* Guaranteed to correctly free memory returned by the gdImage*Ptr
   functions */
BGD_DECLARE(void) gdFree(void *m);

/**
 * @brief Write an image as WBMP data to a newly allocated memory buffer.
 * @ingroup gdCodecWbmp
 *
 * The image is borrowed for the duration of the call. Pixels whose color
 * equals fg are written as black; all other pixels are written as white. The
 * returned buffer is caller-owned and must be freed with gdFree().
 *
 * @param im The image to write.
 * @param size Pointer to an integer that receives the returned buffer size.
 * @param fg Foreground color value to write as black.
 *
 * @return Returns a pointer to the newly allocated WBMP buffer, or NULL on failure.
 */
BGD_DECLARE(void *) gdImageWBMPPtr(gdImagePtr im, int *size, int fg);

/**
 * @addtogroup gdCodecJpeg
 * @{
 */

/**
 * @brief Write an image as JPEG data to a stdio file.
 * 
 * 100 is the highest quality (there is always a little loss with JPEG).
 * 0 is the lowest quality. 10 is about the lowest useful setting.
 * @param im The image to write.
 * @param out The stdio file to write the JPEG data to.
 * @param quality The JPEG quality (0-100).
  */
BGD_DECLARE(void) gdImageJpeg(gdImagePtr im, FILE *out, int quality);
/**
 * @brief Write an image as JPEG data to a gdIOCtx.
 * 
 * @package im The image to write.
 * @param out The gdIOCtx to write the JPEG data to.
 * @param quality The JPEG quality (0-100).
 */
BGD_DECLARE(void) gdImageJpegCtx(gdImagePtr im, gdIOCtxPtr out, int quality);

/**
 * @brief Write an image as JPEG data to a gdIOCtx with metadata.
 * 
 * @param im The image to write.
 * @param out The gdIOCtx to write the JPEG data to.
 * @param quality The JPEG quality (0-100).
 * @param metadata Pointer to a gdImageMetadata structure containing the metadata to include in the JPEG file.
 */
/**
 * @brief Write an image as JPEG data to a stdio file using write options.
 * 
 * @param im The image to write.
 * @param out The stdio file to write the JPEG data to.
 * @param options Pointer to a gdJpegWriteOptions struct containing the desired write options.
 * 
 * @return Returns 0 on success, or a negative value on error.
 */
BGD_DECLARE(int)
gdImageJpegWithOptions(gdImagePtr im, FILE *out, const gdJpegWriteOptions *options);

/**
 * @brief Write an image as JPEG data to a gdIOCtx using write options.
 * 
 * @param im The image to write.
 * @param out The gdIOCtx to write the JPEG data to.
 * @param options Pointer to a gdJpegWriteOptions struct containing the desired write options.
 * 
 * @return Returns 0 on success, or a negative value on error.
 */
BGD_DECLARE(int)
gdImageJpegCtxWithOptions(gdImagePtr im, gdIOCtxPtr out, const gdJpegWriteOptions *options);

/**
 * @brief Write an image as JPEG data to a newly allocated memory buffer.
 * 
 * Result must be freed with gdFree(). The image is borrowed for the duration of the call.
 * 
 * @param im The image to write.
 * @param size Pointer to an integer that will receive the size of the returned buffer.
 * @param quality The JPEG quality (0-100).
 * 
 * @return A pointer to the newly allocated buffer containing the JPEG data, or NULL on failure.
 */
BGD_DECLARE(void *) gdImageJpegPtr(gdImagePtr im, int *size, int quality);
/**
 * @brief Write an image as JPEG data to a memory buffer with metadata.
 * 
 * @param im The image to write.
 * @param size Pointer to an integer that will receive the size of the returned buffer.
 * @param quality The JPEG quality (0-100).
 * @param metadata Pointer to a gdImageMetadata structure containing the metadata to include in the JPEG file.
 * 
 * @return A pointer to the newly allocated buffer containing the JPEG data, or NULL on failure.
 */

/**
 * @brief Write an image as JPEG data to a memory buffer using write options.
 * 
 * @param im The image to write.
 * @param size Pointer to an integer that will receive the size of the returned buffer.
 * @param options Pointer to a gdJpegWriteOptions struct containing the desired write options.
 * 
 * @return A pointer to the newly allocated buffer containing the JPEG data, or NULL on failure.
 */
BGD_DECLARE(void *)
gdImageJpegPtrWithOptions(gdImagePtr im, int *size, const gdJpegWriteOptions *options);
/** @} */

/**
 * @brief Lossless WebP quality threshold.
 * @ingroup gdCodecWebp
 *
 * When the quantization value passed to gdImageWebpEx(), gdImageWebpCtx(), or
 * gdImageWebpPtrEx() is greater than or equal to gdWebpLossless, the image is
 * written in lossless WebP format.
 */
#define gdWebpLossless 101

/**
 * @brief Write an image as WebP data to a stdio file with a quality setting.
 * @ingroup gdCodecWebp
 *
 * gdImageWebpEx() does not close outFile. The image is borrowed for the
 * duration of the call and must be a truecolor image.
 *
 * @param im The image to write.
 * @param outFile Pointer to the output FILE stream.
 * @param quantization WebP quality: -1 for default, 0-100 for lossy, or gdWebpLossless for lossless.
 */
BGD_DECLARE(void) gdImageWebpEx(gdImagePtr im, FILE *outFile, int quantization);

/**
 * @brief Write an image as WebP data to a stdio file with default quality.
 * @ingroup gdCodecWebp
 *
 * gdImageWebp() does not close outFile. The image is borrowed for the duration
 * of the call and must be a truecolor image.
 *
 * @param im The image to write.
 * @param outFile Pointer to the output FILE stream.
 */
BGD_DECLARE(void) gdImageWebp(gdImagePtr im, FILE *outFile);

/**
 * @brief Write an image as WebP data to a newly allocated memory buffer.
 * @ingroup gdCodecWebp
 *
 * The image is borrowed for the duration of the call and must be a truecolor
 * image. The returned buffer is caller-owned and must be freed with gdFree().
 *
 * @param im The image to write.
 * @param size Pointer to an integer that receives the returned buffer size.
 *
 * @return Returns a pointer to the newly allocated WebP buffer, or NULL on failure.
 */
BGD_DECLARE(void *) gdImageWebpPtr(gdImagePtr im, int *size);

/**
 * @brief Write an image as WebP data to a newly allocated memory buffer with a quality setting.
 * @ingroup gdCodecWebp
 *
 * The image is borrowed for the duration of the call and must be a truecolor
 * image. The returned buffer is caller-owned and must be freed with gdFree().
 *
 * @param im The image to write.
 * @param size Pointer to an integer that receives the returned buffer size.
 * @param quantization WebP quality: -1 for default, 0-100 for lossy, or gdWebpLossless for lossless.
 *
 * @return Returns a pointer to the newly allocated WebP buffer, or NULL on failure.
 */
BGD_DECLARE(void *)
gdImageWebpPtrEx(gdImagePtr im, int *size, int quantization);

/**
 * @brief Write an image as WebP data to a stdio file using write options.
 * @ingroup gdCodecWebp
 *
 * gdImageWebpWithOptions() does not close outFile. The image is borrowed for the
 * duration of the call and must be a truecolor image.
 *
 * @param im The image to write.
 * @param outFile Pointer to the output FILE stream.
 * @param options Pointer to WebP write options, or NULL for defaults.
 *
 * @return Returns 0 on success, or 1 on failure.
 */
BGD_DECLARE(int)
gdImageWebpWithOptions(gdImagePtr im, FILE *outFile, const gdWebpWriteOptions *options);

/**
 * @brief Write an image as WebP data to a gdIOCtx using write options.
 * @ingroup gdCodecWebp
 *
 * gdImageWebpCtxWithOptions() does not close outfile. The image is borrowed for
 * the duration of the call and must be a truecolor image.
 *
 * @param im The image to write.
 * @param outfile Pointer to the gdIOCtx output context.
 * @param options Pointer to WebP write options, or NULL for defaults.
 *
 * @return Returns 0 on success, or 1 on failure.
 */
BGD_DECLARE(int)
gdImageWebpCtxWithOptions(gdImagePtr im, gdIOCtxPtr outfile, const gdWebpWriteOptions *options);

/**
 * @brief Write an image as WebP data to a newly allocated memory buffer using write options.
 * @ingroup gdCodecWebp
 *
 * The image is borrowed for the duration of the call and must be a truecolor
 * image. The returned buffer is caller-owned and must be freed with gdFree().
 *
 * @param im The image to write.
 * @param size Pointer to an integer that receives the returned buffer size.
 * @param options Pointer to WebP write options, or NULL for defaults.
 *
 * @return Returns a pointer to the newly allocated WebP buffer, or NULL on failure.
 */
BGD_DECLARE(void *)
gdImageWebpPtrWithOptions(gdImagePtr im, int *size, const gdWebpWriteOptions *options);

/**
 * @brief Write an image as WebP data to a gdIOCtx with a quality setting.
 * @ingroup gdCodecWebp
 *
 * gdImageWebpCtx() does not close outfile. The image is borrowed for the
 * duration of the call and must be a truecolor image.
 *
 * @param im The image to write.
 * @param outfile Pointer to the gdIOCtx output context.
 * @param quantization WebP quality: -1 for default, 0-100 for lossy, or gdWebpLossless for lossless.
 */
BGD_DECLARE(void)
gdImageWebpCtx(gdImagePtr im, gdIOCtxPtr outfile, int quantization);

/*
  Group: Types

  typedef: gdSink

  typedef: gdSinkPtr

        *Note:* This interface is *obsolete* and kept only for
        *compatibility*.  Use <gdIOCtx> instead.

        Represents a "sink" (destination) to which a PNG can be
        written. Programmers who do not wish to write PNGs to a file can
        provide their own alternate output mechanism, using the
        <gdImagePngToSink> function. See the documentation of that
        function for an example of the proper use of this type.

        > typedef struct {
        >     int (*sink) (void *context, char *buffer, int len);
        >     void *context;
        > } gdSink, *gdSinkPtr;

        The _sink_ function must return -1 on error, otherwise the number of
        bytes written, which must be equal to len.

        _context_ will be passed to your sink function.

*/

/** @deprecated in favor of gdIOCtx */
typedef struct {
    int (*sink)(void *context, const char *buffer, int len);
    void *context;
} gdSink, *gdSinkPtr;

/** @deprecated in favor of gdIOCtx */
BGD_DECLARE(void) gdImagePngToSink(gdImagePtr im, gdSinkPtr out);
/** @deprecated in favor of gdIOCtx */
BGD_DECLARE(void) gdImageQoiToSink(gdImagePtr im, gdSinkPtr out);

/**
 * @addtogroup gdCodecGd2
 * @{
 */

/** @name GD2 Writing */
/** @{ */

/**
 * @brief Write an image as GD2 data to a stdio file.
 * @deprecated
 * 
 * gdImageGd2() borrows im and out for the duration of the call and does not
 * close out. Pass cs as 0 to use GD2_CHUNKSIZE; otherwise values outside the
 * GD2_CHUNKSIZE_MIN to GD2_CHUNKSIZE_MAX range are clamped. The public fmt
 * values are GD2_FMT_RAW and GD2_FMT_COMPRESSED. For truecolor images, gd
 * writes the corresponding internal truecolor GD2 format automatically.
 *
 * @param im The image to write.
 * @param out Pointer to the output FILE stream.
 * @param cs Requested chunk size in pixels, or 0 for GD2_CHUNKSIZE.
 * @param fmt Output format, GD2_FMT_RAW or GD2_FMT_COMPRESSED.
 */
BGD_DECLARE(void) gdImageGd2(gdImagePtr im, FILE *out, int cs, int fmt);

/**
 * @brief Write an image as GD2 data to a newly allocated memory buffer.
 * @deprecated
 * 
 * gdImageGd2Ptr() borrows im for the duration of the call. Pass cs as 0 to use
 * GD2_CHUNKSIZE; otherwise values outside the GD2_CHUNKSIZE_MIN to
 * GD2_CHUNKSIZE_MAX range are clamped. The public fmt values are GD2_FMT_RAW
 * and GD2_FMT_COMPRESSED. On success, the returned buffer is owned by the
 * caller and must be freed with gdFree().
 *
 * @param im The image to write.
 * @param cs Requested chunk size in pixels, or 0 for GD2_CHUNKSIZE.
 * @param fmt Output format, GD2_FMT_RAW or GD2_FMT_COMPRESSED.
 * @param size Output location for the returned buffer size in bytes.
 * @return A newly allocated GD2 buffer, or NULL on error.
 */
BGD_DECLARE(void *) gdImageGd2Ptr(gdImagePtr im, int cs, int fmt, int *size);

/** @} */

/** @} */

/**
 * @brief Destroys an image and frees its memory
 * 
 * @param im The image to destroy.
 */
BGD_DECLARE(void) gdImageDestroy(gdImagePtr im);

/**
 * @brief Allocates a color
 *
 * This is a simplified variant of <gdImageColorAllocateAlpha> where the alpha
 * channel is always opaque.
 *
 * @param im The image.
 * @param r  The value of the red component.
 * @param g  The value of the green component.
 * @param b  The value of the blue component.
 *
 * @return The color value.
 *
 * @see gdImageColorDeallocate
 */
BGD_DECLARE(int) gdImageColorAllocate(gdImagePtr im, int r, int g, int b);

/**
 * @brief Allocates a color
 *
 * This is typically used for palette images, but can be used for truecolor
 * images as well.
 *
 * @param im The image.
 * @param r  The value of the red component.
 * @param g  The value of the green component.
 * @param b  The value of the blue component.
 *
 * @return The color value.
 *
 * @see gdImageColorDeallocate
 */
BGD_DECLARE(int)
gdImageColorAllocateAlpha(gdImagePtr im, int r, int g, int b, int a);

/** @brief Gets the closest color of the image
 *
 * This is a simplified variant of <gdImageColorClosestAlpha> where the alpha
 * channel is always opaque.
 *
 * @param im The image.
 * @param r  The value of the red component.
 * @param g  The value of the green component.
 * @param b  The value of the blue component.
 * 
 * @return The closest color already available in the palette for palette images;
 *         the color value of the given components for truecolor images.
 *
 * @see gdImageColorExact
 */
BGD_DECLARE(int) gdImageColorClosest(gdImagePtr im, int r, int g, int b);

/**
 * @brief Gets the closest color of the image with alpha channel
 *
 * @param im The image.
 * @param r  The value of the red component.
 * @param g  The value of the green component.
 * @param b  The value of the blue component.
 * @param a  The value of the alpha component.
 *
 * @return The closest color already available in the palette for palette images;
 *         the color value of the given components for truecolor images.
 *
 * @see gdImageColorExactAlpha
 */
BGD_DECLARE(int)
gdImageColorClosestAlpha(gdImagePtr im, int r, int g, int b, int a);

/**
 * @brief Gets the closest color of the image using HWB color space
 * 
 * This function finds the closest color in the image's palette to the specified RGB color using the HWB (Hue, Whiteness, Blackness) color space. It is a more perceptually accurate method for color matching compared to simple RGB distance calculations.
 *
 * @param im The image.
 * @param r  The value of the red component.
 * @param g  The value of the green component.
 * @param b  The value of the blue component.
 *
 * @return The closest color already available in the palette for palette images; if
 *         there is no exact color, -1 is returned.
 *         For truecolor images the color value of the given components is returned.
 *
 * @see gdImageColorExact
 */
BGD_DECLARE(int) gdImageColorClosestHWB(gdImagePtr im, int r, int g, int b);

/**
 * @brief Gets the exact color of the image
 *
 * This is a simplified variant of <gdImageColorExactAlpha> where the alpha
 * channel is always opaque.
 *
 * @param im The image.
 * @param r  The value of the red component.
 * @param g  The value of the green component.
 * @param b  The value of the blue component.
 *
 * @return The exact color already available in the palette for palette images; if
 *         there is no exact color, -1 is returned.
 *         For truecolor images the color value of the given components is returned.
 *
 * @see gdImageColorClosest
 */
BGD_DECLARE(int) gdImageColorExact(gdImagePtr im, int r, int g, int b);

/**
 * @brief Gets the exact color of the image
 *
 * This is a simplified variant of <gdImageColorExactAlpha> where the alpha
 * channel is always opaque.
 *
 * @param im The image.
 * @param r  The value of the red component.
 * @param g  The value of the green component.
 * @param b  The value of the blue component.
 * @param a  The value of the alpha component.
 *
 * @return The exact color already available in the palette for palette images; if
 *         there is no exact color, -1 is returned.
 *         For truecolor images the color value of the given components is returned.
 *
 * @see gdImageColorClosestAlpha gdTrueColorAlpha
 */
BGD_DECLARE(int)
gdImageColorExactAlpha(gdImagePtr im, int r, int g, int b, int a);

/**
 * @brief Resolves a color in the image
 * @see gdImageColorResolve is an alternative for the code fragment
 * @code
 *  if ((color=gdImageColorExact(im,R,G,B)) < 0)
 *      if ((color=gdImageColorAllocate(im,R,G,B)) < 0)
 *          color=gdImageColorClosest(im,R,G,B);
 * @endcode
 * in a single function.    Its advantage is that it is guaranteed to
 * @return a color index in one search over the color table.
  */
BGD_DECLARE(int) gdImageColorResolve(gdImagePtr im, int r, int g, int b);

/**
 * @brief Same as @ref gdImageColorResovle but with alpha
 * 
 * @param im The image.
 * @param r The red component.
 * @param g The green component.
 * @param b The blue component.
 * @param a The alpha component.
 * 
 * @return The color index of the closest color in the palette or the newly allocated color.
 * 
 * @see gdImageColorExactAlpha gdImageColorClosestAlpha gdTrueColorAlpha
 */
BGD_DECLARE(int)
gdImageColorResolveAlpha(gdImagePtr im, int r, int g, int b, int a);

/**
 * @brief Compose a truecolor value from its components
 *
 * use it only when needed an actual truecolor value, for example when drawing on a truecolor image.
 * @param r The red channel (0-255)
 * @param g The green channel (0-255)
 * @param b The blue channel (0-255)
 *
 * @see gdTrueColorAlpha gdTrueColorGetAlpha gdTrueColorGetRed gdTrueColorGetGreen gdTrueColorGetBlue
 */
#define gdTrueColor(r, g, b) (((r) << 16) + ((g) << 8) + (b))

/**
 * Group: Color Composition
 */

/**
 * @brief Compose a truecolor value from its components
 *
 * @param r The red channel (0-255)
 * @param g The green channel (0-255)
 * @param b The blue channel (0-255)
 * @param a The alpha channel (0-127, where 127 is fully transparent, and 0 is
 *          completely opaque).
 *
 * @see gdTrueColorGetAlpha gdTrueColorGetRed gdTrueColorGetGreen gdTrueColorGetBlue gdImageColorExactAlpha
 */
#define gdTrueColorAlpha(r, g, b, a) (((a) << 24) + ((r) << 16) + ((g) << 8) + (b))

/**
 * @brief Removes a palette entry
 *
 * This is a no-op for truecolor images.
 * The function does not alter the image data nor the transparent color or any
 * other places where this color index could have been referenced.
 * The index is marked as open and will be used too for any subsequent
 * @ref gdImageColorAllocate or @ref gdImageColorAllocateAlpha calls. Other lower
 * index may be open as well, the fist open index found will be used.
 *
 * @param im    The image.
 * @param color The palette index.
 *
 * @see gdImageColorAllocate gdImageColorAllocateAlpha
 */
BGD_DECLARE(void) gdImageColorDeallocate(gdImagePtr im, int color);

/**
 * @brief Bring the palette colors in im2 to be closer to im1.
 *
 * @param im1 The first image.
 * @param im2 The second image.
 *
 * @return 0 on success, or -1 on failure.
 */
BGD_DECLARE(int) gdImageColorMatch(gdImagePtr im1, gdImagePtr im2);

/**
 * @brief Sets the transparent color of the image
 *
 * 
 * Specifies a color index (if a palette image) or an
 * RGB color (if a truecolor image) which should be
 * considered 100% transparent. FOR TRUECOLOR IMAGES,
 * THIS IS IGNORED IF AN ALPHA CHANNEL IS BEING
 * SAVED. Use gdImageSaveAlpha(im, 0); to
 * turn off the saving of a full alpha channel in
 * a truecolor image. Note that gdImageColorTransparent
 * is usually compatible with older browsers that
 * do not understand full alpha channels well. TBB
 * 
 * @param im    The image.
 * @param color The color.
 *
 * @see gdImageGetTransparent
 */
BGD_DECLARE(void) gdImageColorTransparent(gdImagePtr im, int color);

/**
 * @brief Copies the palette from one image to another
 *
 * @param dst The destination image.
 * @param src The source image.
 */
BGD_DECLARE(void) gdImagePaletteCopy(gdImagePtr dst, gdImagePtr src);

typedef int (*gdCallbackImageColor)(gdImagePtr im, int src);

/**
 * @brief Replaces a color in the image with another color
 * 
 * @param im  The image.
 * @param src The source color to be replaced.
 * @param dst The destination color to replace with.
 */
BGD_DECLARE(int) gdImageColorReplace(gdImagePtr im, int src, int dst);

/**
 * @brief Replaces colors in an image with a threshold for perceptual color distance.
 *
 * Note: threshold semantics changed in versions >=2.3.4 — the value now scales
 * linearly with perceptual color distance. Callers using threshold values
 * tuned against the old behavior should apply new_t = sqrt(old_t / 100) * 100
 * to approximate the previous behavior. This is due to a bug fix in the color
 * distance calculation, which previously did not take the square root
 * of the sum of squares, and thus returned a value that was the square
 * of the actual perceptual color distance.
 * The new behavior is more intuitive and consistent with common color distance metrics
 * 
 * @param im The image to operate on.
 * @param src The source color to replace.
 * @param dst The destination color to replace with.
 * @param threshold The threshold for color matching. Colors within this distance from the source color will be replaced with the destination color.
 * @return The number of pixels that were replaced.
 */
BGD_DECLARE(int)
gdImageColorReplaceThreshold(gdImagePtr im, int src, int dst, float threshold);

/**
 * @brief Replaces multiple colors in an image with corresponding destination colors.
 * 
 * @param im The image to operate on.
 * @param len The number of colors to replace.
 * @param src An array of source colors to be replaced.
 * @param dst An array of destination colors to replace with.
 * 
 * @return The number of pixels that were replaced.
 */
BGD_DECLARE(int)
gdImageColorReplaceArray(gdImagePtr im, int len, int *src, int *dst);

/**
 * @brief Replaces colors in an image using a callback function to determine the replacement color.
 * 
 * @param im The image to operate on.
 * @param callback A callback function that takes the image and a source color as parameters and returns the destination color to replace with. @see gdCallbackImageColor
 * 
 * @return The number of pixels that were replaced.
 */
BGD_DECLARE(int)
gdImageColorReplaceCallback(gdImagePtr im, gdCallbackImageColor callback);

/**
 * @defgroup  Per Pixel Operations
 * @{ */

/**
 * @brief Sets the pixel at the specified coordinates to the given color.
 * Replaces or blends with the background depending on the
 * most recent call to @ref gdImageAlphaBlending and the
 * alpha channel value of 'color'; default is to overwrite.
 * Tiling and line styling are also implemented
 * here. All other gd drawing functions pass through this call,
 * allowing for many useful effects.
 * Overlay and multiply effects are used when @ref gdImageAlphaBlending
 * is passed @ref gdEffectOverlay and @ref gdEffectMultiply 
 * 
 * @param im The image.
 * @param x The x-coordinate of the pixel.
 * @param y The y-coordinate of the pixel.
 * @param color The color to set the pixel to. Color can be a palette index for palette images or a truecolor value for truecolor images.
 * 
 * @see @ref gdImageGetPixel gdImageGetTrueColorPixel gdImageAlphaBlending gdImageCreateTruecolor gdImageCreatePalette
 */
BGD_DECLARE(void) gdImageSetPixel(gdImagePtr im, int x, int y, int color);

/**
 * @brief Gets the color of the pixel at the specified coordinates.
 * 
 * @param im The image.
 * @param x The x-coordinate of the pixel.
 * @param y The y-coordinate of the pixel.
 * 
 * @return The color of the pixel. For palette images, this is the palette index. For truecolor images, this is the truecolor value.
 */
BGD_DECLARE(int) gdImageGetPixel(gdImagePtr im, int x, int y);

/**
 * @brief Gets the truecolor value of the pixel at the specified coordinates.
 * 
 * @param im The image.
 * @param x The x-coordinate of the pixel.
 * @param y The y-coordinate of the pixel.
 * 
 * @return The truecolor value of the pixel. For palette images, this function will return the truecolor value corresponding to the palette index of the pixel.
 */
BGD_DECLARE(int) gdImageGetTrueColorPixel(gdImagePtr im, int x, int y);
/** @} */

/**
 * @brief Sets the resolution of an image.
 *
 * @param im    The image.
 * @param res_x The horizontal resolution in DPI.
 * @param res_y The vertical resolution in DPI.
 *
 * @see gdImageResolutionX gdImageResolutionY
 */
BGD_DECLARE(void)
gdImageSetResolution(gdImagePtr im, const unsigned int res_x, const unsigned int res_y);

/**
 * @defgroup Font Text Rendering, Bitmap Fonts
 *
 * @{ */

/**
 * @brief Gets the built-in giant font.
 */
BGD_DECLARE(gdFontPtr) gdFontGetGiant(void);
/**
 * @brief Gets the built-in large font.
 */
BGD_DECLARE(gdFontPtr) gdFontGetLarge(void);
/**
 * @brief Gets the built-in medium bold font.
 */
BGD_DECLARE(gdFontPtr) gdFontGetMediumBold(void);
/**
 * @brief Gets the built-in small font.
 */
BGD_DECLARE(gdFontPtr) gdFontGetSmall(void);
/**
 * @brief Gets the built-in tiny font.
 */
BGD_DECLARE(gdFontPtr) gdFontGetTiny(void);
/**
 * @brief Draws a single character.
 *
 * @param im    The image to draw onto.
 * @param f     The raster font.
 * @param x     The x coordinate of the upper left pixel.
 * @param y     The y coordinate of the upper left pixel.
 * @param c     The character.
 * @param color The color.
 *
 * Variants @ref gdImageCharUp
 *
 * @see gdFontPtr
 */
BGD_DECLARE(void) gdImageChar(gdImagePtr im, gdFontPtr f, int x, int y, int c, int color);

/**
 * @brief Draws a single character rotated 90 degrees counterclockwise.
 * 
 * @param im    The image to draw onto.
 * @param f     The raster font.
 * @param x     The x coordinate of the upper left pixel.
 * @param y     The y coordinate of the upper left pixel.
 * @param c     The character.
 * @param color The color.
 */
BGD_DECLARE(void) gdImageCharUp(gdImagePtr im, gdFontPtr f, int x, int y, int c, int color);

/**
 * @brief Draws a character string.
 *
 * @param im    The image to draw onto.
 * @param f     The raster font.
 * @param x     The x coordinate of the upper left pixel.
 * @param y     The y coordinate of the upper left pixel.
 * @param s     The character string.
 * @param color The color.
 *
 * Variants:
 *  - @ref gdImageStringUp
 *  - @ref gdImageString16
 *  - @ref gdImageStringUp16
 *
 * @see gdFontPtr gdImageStringTTF gdImageString
 */
BGD_DECLARE(void)
gdImageString(gdImagePtr im, gdFontPtr f, int x, int y, unsigned char *s, int color);

/**
 * @brief Draws a string rotated 90 degrees counterclockwise.
 *
 * @param im    The image to draw onto.
 * @param f     The raster font.
 * @param x     The x coordinate of the upper left pixel.
 * @param y     The y coordinate of the upper left pixel.
 * @param s     The string.
 * @param color The color.
 */
BGD_DECLARE(void)
gdImageStringUp(gdImagePtr im, gdFontPtr f, int x, int y, unsigned char *s, int color);

/**
 * @brief Draws a character string with 16-bit characters.
 * 
 * @param im    The image to draw onto.
 * @param f     The raster font.
 * @param x     The x coordinate of the upper left pixel.
 * @param y     The y coordinate of the upper left pixel.
 * @param s     The character string (16-bit).
 * @param color The color.
 */
BGD_DECLARE(void)
gdImageString16(gdImagePtr im, gdFontPtr f, int x, int y, unsigned short *s, int color);

/**
 * @brief Draws a string rotated 90 degrees counterclockwise with 16-bit characters.
 * 
 * @param im    The image to draw onto.
 * @param f     The raster font.
 * @param x     The x coordinate of the upper left pixel.
 * @param y     The y coordinate of the upper left pixel.
 * @param s     The string (16-bit).
 * @param color The color.
 */
BGD_DECLARE(void)
gdImageStringUp16(gdImagePtr im, gdFontPtr f, int x, int y, unsigned short *s, int color);
/** @} */

/**
 * @defgroup freetypefont Font Text Rendering, FreeType 2
 * @{
 */

/**
 * @brief Set up the font cache.
 *
 * This is called automatically from the string rendering functions, if it
 * has not already been called. So there's no need to call this function
 * explicitly.
 */
BGD_DECLARE(int) gdFontCacheSetup(void);

/**
 * @brief Shut down the font cache and free the allocated resources.
 *
 * @note This function has to be called whenever FreeType operations have been invoked, to avoid resource leaks. It doesn't harm to call this function multiple times.
 */
BGD_DECLARE(void) gdFontCacheShutdown(void);

/**
 * @brief Alias of @ref gdFontCacheShutdown.
 * @deprecated
 */
BGD_DECLARE(void) gdFreeFontCache(void);


/**
 * @brief Draws a string using FreeType 2 fonts. Alias of @ref gdImageStringFT. Provided for backwards compatibility only. 
 * @deprecated
 */
BGD_DECLARE(char *)
gdImageStringTTF(gdImagePtr im, int *brect, int fg, const char *fontlist, double ptsize,
                 double angle, int x, int y, const char *string);


/**
 * @brief Render an UTF-8 string onto a gd image.
 *
 * @param im       The image to draw onto.
 * @param brect    The bounding rectangle as array of 8 integers where each pair
 *                 represents the x- and y-coordinate of a point. The points
 *                 specify the lower left, lower right, upper right and upper left
 *                 corner.
 * @param fg       The font color.
 * @param fontlist The semicolon delimited list of font filenames to look for.
 * @param ptsize   The height of the font in typographical points (pt).
 * @param angle    The angle in radian to rotate the font counter-clockwise.
 * @param x        The x-coordinate of the basepoint (roughly the lower left corner)
 *                   of the first letter.
 * @param y        The y-coordinate of the basepoint (roughly the lower left corner)
 *                   of the first letter.
 * @param string   The string to render.
 *
 * Variant @ref gdImageStringFTEx
 *
 * @see gdImageString
 */
BGD_DECLARE(char *)
gdImageStringFT(gdImagePtr im, int *brect, int fg, const char *fontlist, double ptsize,
                double angle, int x, int y, const char *string);

/* 2.0.5: provides an extensible way to pass additional parameters.
   Thanks to Wez Furlong, sorry for the delay. */
/**
 * @brief Structure for passing additional parameters to FreeType 2 string rendering functions.
 *
 * This structure allows for fine-tuning of FreeType 2 string rendering, including line spacing, character mapping, resolution, and more. It is used with the @ref gdImageStringFTEx function.
 */
typedef struct {
    int flags; /**< Logical OR of gdFTEX_* option flags. */
    double linespacing; /**< Fine-tunes line spacing for newline-separated text. */
    int charmap; /**< Character map to use when @ref gdFTEX_CHARMAP is set: @ref gdFTEX_Unicode,
                      @ref gdFTEX_Shift_JIS, @ref gdFTEX_Big5, or @ref gdFTEX_Adobe_Custom.
                      When not specified, maps are searched in that order. */
    int hdpi; /**< Horizontal resolution in DPI when @ref gdFTEX_RESOLUTION is set. */
    int vdpi; /**< Vertical resolution in DPI when @ref gdFTEX_RESOLUTION is set. */
    char *xshow; /**< When @ref gdFTEX_XSHOW is set, receives a gd-allocated string
                      containing xshow position data for the last string. The
                      caller must free it with gdFree(). */
    char *fontpath; /**< When @ref gdFTEX_RETURNFONTPATHNAME is set, receives a
                         gd-allocated string containing the actual font file
                         path used. This is useful when fontconfig selects the
                         font. The caller must free it with gdFree(). */
} gdFTStringExtra, *gdFTStringExtraPtr;

/**
 * @name gdFTStringExtra option flags
 *
 * These flags are combined in gdFTStringExtra::flags and used by
 * @ref gdImageStringFTEx.
 *
 * @{
 */
#define gdFTEX_LINESPACE 1 /**< Use gdFTStringExtra::linespacing for
                                newline-separated text. The value is a multiple
                                of the font height; without this flag, the
                                default line spacing is 1.05. */
#define gdFTEX_CHARMAP 2 /**< Use gdFTStringExtra::charmap as the preferred
                              FreeType character map. If the requested map is
                              not available, GD attempts compatible fallback
                              maps where possible. */
#define gdFTEX_RESOLUTION 4 /**< Use gdFTStringExtra::hdpi and
                                 gdFTStringExtra::vdpi as the FreeType
                                 rendering resolution. Without this flag, GD
                                 uses `GD_RESOLUTION` for both axes. */
#define gdFTEX_DISABLE_KERNING 8 /**< Disable FreeType kerning adjustments
                                      between consecutive glyphs. */
#define gdFTEX_XSHOW 16 /**< Return a gd-allocated xshow advance string in
                             gdFTStringExtra::xshow. The caller must free that
                             string with gdFree(). */
#define gdFTEX_FONTPATHNAME 32 /**< Interpret the fontlist argument as a full
                                    or partial font file path, even when
                                    fontconfig has been enabled by default with
                                    gdFTUseFontConfig(). */
#define gdFTEX_FONTCONFIG 64 /**< Interpret the fontlist argument as a
                                  fontconfig pattern for this call. This is not
                                  needed when fontconfig has already been
                                  enabled by default with gdFTUseFontConfig(). */
#define gdFTEX_RETURNFONTPATHNAME 128 /**< Return a gd-allocated copy of the
                                           actual font file path used in
                                           gdFTStringExtra::fontpath. This is
                                           useful when fontconfig selects the
                                           font. The caller must free that string
                                           with gdFree(). */
/** @} */

/**
 * @brief Enable or disable fontconfig by default.
 * 
 * If flag is nonzero, the fontlist parameter to gdImageStringFT
 * and @ref gdImageStringFTEx shall be assumed to be a fontconfig font pattern
 * if fontconfig was compiled into gd. This function returns zero
 * if fontconfig is not available, nonzero otherwise.
 * If GD is built without libfontconfig support, this function is a NOP.
 *
 * @param flag Zero to disable, nonzero to enable.
 *
 * @see gdImageStringFTEx
 */
BGD_DECLARE(int) gdFTUseFontConfig(int flag);

/**
 * @name gdFTStringExtra character map values
 *
 * These are not option flags. Set one value in gdFTStringExtra::charmap when
 * gdFTStringExtra::flags includes @ref gdFTEX_CHARMAP. Without
 * @ref gdFTEX_CHARMAP, GD prefers Unicode.
 *
 * @{
 */
#define gdFTEX_Unicode 0 /**< Prefer a Unicode character map. GD may fall back
                              to symbol or Adobe maps when no Unicode map is
                              available. */
#define gdFTEX_Shift_JIS 1 /**< Prefer a Shift_JIS character map. */
#define gdFTEX_Big5 2 /**< Prefer a Big5 character map. */
#define gdFTEX_Adobe_Custom 3 /**< Prefer an Adobe Custom character map. GD may
                                   fall back to Apple Roman when no Adobe Custom
                                   map is available. */
#define gdFTEX_MacRoman gdFTEX_Adobe_Custom /**< Deprecated compatibility name
                                                 used by bundled PHP's
                                                 historical libgd. */
/** @} */

/**
 * @brief Draws a string using FreeType 2 fonts with additional parameters.
 *
 * gdImageStringFTEx() extends @ref gdImageStringFT by accepting an optional
 * @ref gdFTStringExtra structure. Pass NULL for strex to use the same behavior
 * as gdImageStringFT().
 *
 * The gdFTStringExtra::flags field controls which extra fields are used:
 * - @ref gdFTEX_LINESPACE uses gdFTStringExtra::linespacing for multiline
 *   text. The value is expressed as a multiple of the font height; 1.0 is the
 *   minimum spacing that normally prevents lines from colliding. Without this
 *   flag, or when strex is NULL, line spacing defaults to 1.05.
 *   @code{.c}
 *   strex.flags |= gdFTEX_LINESPACE;
 *   strex.linespacing = 1.2;
 *   @endcode
 * - @ref gdFTEX_CHARMAP uses gdFTStringExtra::charmap as the preferred
 *   character map. Valid values are @ref gdFTEX_Unicode,
 *   @ref gdFTEX_Shift_JIS, @ref gdFTEX_Big5, and
 *   @ref gdFTEX_Adobe_Custom. Without this flag, GD tries Unicode first. If
 *   the preferred map is unavailable, GD attempts compatible fallback maps
 *   where possible.
 *   @code{.c}
 *   strex.flags |= gdFTEX_CHARMAP;
 *   strex.charmap = gdFTEX_Unicode;
 *   @endcode
 * - @ref gdFTEX_RESOLUTION uses gdFTStringExtra::hdpi and
 *   gdFTStringExtra::vdpi as the FreeType rendering resolution in dots per
 *   inch. Without this flag, GD uses its default screen resolution.
 *   @code{.c}
 *   strex.flags |= gdFTEX_RESOLUTION;
 *   strex.hdpi = 300;
 *   strex.vdpi = 300;
 *   @endcode
 * - @ref gdFTEX_DISABLE_KERNING disables FreeType kerning adjustments between
 *   consecutive glyphs.
 *   @code{.c}
 *   strex.flags |= gdFTEX_DISABLE_KERNING;
 *   @endcode
 * - @ref gdFTEX_XSHOW returns a gd-allocated string of character advance
 *   values in gdFTStringExtra::xshow. The caller must free this string with
 *   gdFree().
 *   @code{.c}
 *   strex.flags |= gdFTEX_XSHOW;
 *   @endcode
 * - @ref gdFTEX_RETURNFONTPATHNAME returns a gd-allocated copy of the actual
 *   font file path used in gdFTStringExtra::fontpath. The caller must free this
 *   string with gdFree().
 *   @code{.c}
 *   strex.flags |= gdFTEX_RETURNFONTPATHNAME;
 *   @endcode
 *
 * Font selection normally treats fontlist as a semicolon-delimited list of font
 * file names. When GD is built with fontconfig, @ref gdFTEX_FONTCONFIG makes
 * fontlist a fontconfig pattern for this call, and gdFTUseFontConfig() can make
 * fontconfig patterns the default. If fontconfig has been enabled by default,
 * @ref gdFTEX_FONTPATHNAME forces fontlist to be interpreted as font path names
 * for this call.
 * @code{.c}
 * strex.flags |= gdFTEX_FONTCONFIG;
 * strex.flags |= gdFTEX_FONTPATHNAME;
 * @endcode
 *
 * If brect is not NULL, it must point to an array of 8 integers. On success,
 * GD fills it with the lower-left, lower-right, upper-right, and upper-left
 * corners of the rendered text bounding rectangle. Passing NULL for im computes
 * the bounding rectangle without drawing.
 *
 * @param im       The image to draw onto, or NULL to compute brect only.
 * @param brect    Optional output array of 8 integers receiving the text
 *                 bounding rectangle.
 * @param fg       The font color. Negative values select monochrome rendering
 *                 using -fg as the color.
 * @param fontlist The semicolon-delimited list of font file names, or a
 *                 fontconfig pattern when fontconfig mode is active.
 * @param ptsize   The height of the font in typographical points.
 * @param angle    The angle in radians to rotate the font counter-clockwise.
 * @param x        The x-coordinate of the baseline starting point.
 * @param y        The y-coordinate of the baseline starting point.
 * @param string   The string to render.
 * @param strex    Optional pointer to a gdFTStringExtra structure containing
 *                 additional rendering options, or NULL.
 *
 * @return NULL on success, or a pointer to a static error message on failure.
 */
BGD_DECLARE(char *)
gdImageStringFTEx(gdImagePtr im, int *brect, int fg, const char *fontlist, double ptsize,
                  double angle, int x, int y, const char *string, gdFTStringExtraPtr strex);

/** @} */

/**
 * @defgroup PixelDraw lines, ellipses, polygons and Arc Drawing pixel operations
 * 
 * @note 2.4+ brings a 2D Vector APIs with high quality rendering and options. Similar to Canvas 2D APIs. We recommend it for new usages.
 * 
 * @{
 */

 /**
 * @brief A point in the coordinate space of the image
 */
typedef struct {
    int x, y; /**< The x and y coordinates of the point. */
} gdPoint, *gdPointPtr; /**< A pointer to a <gdPoint>. */

/**
 * @brief A rectangle in the coordinate space of the image
  */
typedef struct {
    int x, y; /**< The x and y coordinates of the upper left corner. */
    int width, height; /**< The width and height of the rectangle. */
} gdRect, *gdRectPtr; /**< A pointer to a @ref gdRect. */

/** 
 * @brief Style flags for drawing arcs and chords
 * Style is a bitwise OR ( | operator ) of these.
 * gdArc and gdChord are mutually exclusive;
 * gdChord just connects the starting and ending
 * angles with a straight line, while gdArc produces
 * a rounded edge. gdPie is a synonym for gdArc.
 * gdNoFill indicates that the arc or chord should be
 * outlined, not filled. gdEdged, used together with
 * gdNoFill, indicates that the beginning and ending
 * angles should be connected to the center; this is
 * a good way to outline (rather than fill) a
 * 'pie slice'.
 */
#define gdArc 0 /**< mutually exclusive with gdChord */
#define gdPie gdArc /**< synonym for gdArc */
#define gdChord 1 /**< mutually exclusive with gdArc */
#define gdNoFill 2 /**< indicates that the arc or chord should be outlined, not filled */
#define gdEdged 4 /**< used together with gdNoFill, indicates that the beginning and ending angles should be connected to the center */

/**
 * @brief Draws a closed polygon
 *
 * @param  im The image.
 * @param  p  The vertices as array of <gdPoint>s.
 * @param  n  The number of vertices.
 * @param  c  The color.
 *
 * @see gdImageOpenPolygon gdImageFilledPolygon
 */
BGD_DECLARE(void) gdImagePolygon(gdImagePtr im, gdPointPtr p, int n, int c);

/**
 * @brief Draws an open polygon
 *
 * @param im The image.
 * @param p  The vertices as array of <gdPoint>s.
 * @param n  The number of vertices.
 * @param c  The color.
 *
 * @see gdImagePolygon
 */
BGD_DECLARE(void) gdImageOpenPolygon(gdImagePtr im, gdPointPtr p, int n, int c);


/**
 * @brief Draws a filled polygon
 *
 * The polygon is filled using the even-odd fillrule what can leave unfilled
 * regions inside of self-intersecting polygons. This behavior might change in
 * a future version.
 *
 * @param  im The image.
 * @param  p  The vertices as array of <gdPoint>s.
 * @param  n  The number of vertices.
 * @param  c  The color.
 *
 * @see gdImagePolygon
 */
BGD_DECLARE(void)
gdImageFilledPolygon(gdImagePtr im, gdPointPtr p, int n, int c);

/**
 * @brief Draws a filled arc or a filled chord
 * 
 * @param  im    The image.
 * @param  cx    The x-coordinate of the center.
 * @param  cy    The y-coordinate of the center.
 * @param  w     The width of the arc.
 * @param  h     The height of the arc.
 * @param  s     The starting angle in degrees.
 * @param  e     The ending angle in degrees.
 * @param  color The color of the arc. A color identifier created with one of the
 *               image color allocate functions.
 * @param  style The style of the arc. A bitwise OR of gdArc,
 * 
 * @see gdImageArc
 */
BGD_DECLARE(void)
gdImageFilledArc(gdImagePtr im, int cx, int cy, int w, int h, int s, int e, int color, int style);

/**
 * @brief Draws an arc or a chord
 * 
 * @param  im    The image.
 * @param  cx    The x-coordinate of the center.
 * @param  cy    The y-coordinate of the center.
 * @param  w     The width of the arc.
 * @param  h     The height of the arc.
 * @param  s     The starting angle in degrees.
 * @param  e     The ending angle in degrees.
 * @param  color The color of the arc. A color identifier created with one of the
 *               image color allocate functions.
 * @see gdImageFilledArc
 */
BGD_DECLARE(void)
gdImageArc(gdImagePtr im, int cx, int cy, int w, int h, int s, int e, int color);

/**
 * @brief Draw an ellipse, stroke only.
 *
 * @note This function does not support @ref gdImageSetThickness. GD 3.0 supports
 * actual 2D vectors operation, you may rely on it if you need better 2D drawing
 * operations.
 *
 * @param  im   The destination image.
 * @param  cx   x-coordinate of the center.
 * @param  cy   y-coordinate of the center.
 * @param  w    The ellipse width.
 * @param  h    The ellipse height.
 * @param  color The color of the ellipse. A color identifier created with one of the
 * image color allocate functions.
 *
 * @see gdImageFilledEllipse
 */
BGD_DECLARE(void)
gdImageEllipse(gdImagePtr im, int cx, int cy, int w, int h, int color);

/**
 * @brief Draw a filled ellipse.
 * 
 * @param  im    The destination image.
 * @param  cx    x-coordinate of the center.
 * @param  cy    y-coordinate of the center.
 * @param  w     The ellipse width.
 * @param  h     The ellipse height.
 * @param  color The color of the ellipse. A color identifier created with one of the
 *               image color allocate functions.
 */
BGD_DECLARE(void)
gdImageFilledEllipse(gdImagePtr im, int cx, int cy, int w, int h, int color);

BGD_DECLARE(void) gdImageAABlend(gdImagePtr im);

BGD_DECLARE(void) gdImageLine(gdImagePtr im, int x1, int y1, int x2, int y2, int color);

/* For backwards compatibility only. Use gdImageSetStyle()
   for much more flexible line drawing. */
BGD_DECLARE(void) gdImageDashedLine(gdImagePtr im, int x1, int y1, int x2, int y2, int color);

/**
 * @brief Draws a rectangle.
 *
 * Corners are specified by their coordinates. The rectangle is drawn using the current line style and thickness.
 * 
 * @param  im    The image.
 * @param  x1    The x-coordinate of one of the corners.
 * @param  y1    The y-coordinate of one of the corners.
 * @param  x2    The x-coordinate of another corner.
 * @param  y2    The y-coordinate of another corner.
 * @param  color The color.
 *
 * @see gdImageFilledRectangle
 */
BGD_DECLARE(void) gdImageRectangle(gdImagePtr im, int x1, int y1, int x2, int y2, int color);

/**
 * @brief Draws a filled rectangle.
 * 
 * @param  im    The image.
 * @param  x1    The x-coordinate of one of the corners.
 * @param  y1    The y-coordinate of one of the corners.
 * @param  x2    The x-coordinate of another corner.
 * @param  y2    The y-coordinate of another corner.
 * @param  color The color.
 */
BGD_DECLARE(void) gdImageFilledRectangle(gdImagePtr im, int x1, int y1, int x2, int y2, int color);

/**
 * @brief Sets the clipping rectangle
 *
 * The clipping rectangle restricts the drawing area for following drawing
 * operations.
 *
 * @param  im - The image.
 * @param  x1 - The x-coordinate of the upper left corner.
 * @param  y1 - The y-coordinate of the upper left corner.
 * @param  x2 - The x-coordinate of the lower right corner.
 * @param  y2 - The y-coordinate of the lower right corner.
 *
 * @see gdImageGetClip
 */
BGD_DECLARE(void) gdImageSetClip(gdImagePtr im, int x1, int y1, int x2, int y2);

/**
 * @brief Gets the current clipping rectangle
 *
 * @param  im   The image.
 * @param  x1P  (out) The x-coordinate of the upper left corner.
 * @param  y1P  (out) The y-coordinate of the upper left corner.
 * @param  x2P  (out) The x-coordinate of the lower right corner.
 * @param  y2P  (out) The y-coordinate of the lower right corner.
 *
 * @see gdImageSetClip
 */
BGD_DECLARE(void) gdImageGetClip(gdImagePtr im, int *x1P, int *y1P, int *x2P, int *y2P);

/**
 * @brief Sets the brush for following drawing operations
 *
 * @param  im    The image.
 * @param  brush The brush image.
 */
BGD_DECLARE(void) gdImageSetBrush(gdImagePtr im, gdImagePtr brush);

/**
 * @brief Sets the tile for following drawing operations
 * 
 * The tile is used for filling areas with a repeating pattern. The tile image is repeated to fill the area being drawn.
 * 
 * @param  im   The image.
 * @param  tile The tile image.
 */
BGD_DECLARE(void) gdImageSetTile(gdImagePtr im, gdImagePtr tile);


/**
 * @brief Set the color for subsequent anti-aliased drawing
 *
 * If @ref gdAntiAliased is passed as color to drawing operations that support
 * anti-aliased drawing (such as @ref gdImageLine and @ref gdImagePolygon), the actual
 * color to be used can be set with this function.
 *
 * Example: draw an anti-aliased blue line:
 * @code
 * gdImageSetAntiAliased(im, gdTrueColorAlpha(0, 0, gdBlueMax, gdAlphaOpaque));
 * gdImageLine(im, 10,10, 20,20, gdAntiAliased);
 * @endcode
 *
 * @param  im - The image.
 * @param  c  - The color.
 *
 * @see gdImageSetAntiAliasedDontBlend
 */
BGD_DECLARE(void) gdImageSetAntiAliased(gdImagePtr im, int c);

/**
 * Set the color and "dont_blend" color for subsequent anti-aliased drawing
 *
 * This extended variant of <gdImageSetAntiAliased> allows to also specify a
 * (background) color that will not be blended in anti-aliased drawing
 * operations.
 *
 * @param im         The image.
 * @param c          The color.
 * @param dont_blend Whether to blend.
 */
BGD_DECLARE(void) gdImageSetAntiAliasedDontBlend(gdImagePtr im, int c, int dont_blend);

/**
 * @brief Sets the style for following drawing operations
 *
 * @param  im        The image.
 * @param  style     An array of color values.
 * @param  noOfPixel The number of color values.
 */
BGD_DECLARE(void) gdImageSetStyle(gdImagePtr im, int *style, int noOfPixels);


/**
 * Sets the thickness for following drawing operations
 *
 * @param  im        The image.
 * @param  thickness The thickness in pixels.
 */
BGD_DECLARE(void) gdImageSetThickness(gdImagePtr im, int thickness);


/** @} */

BGD_DECLARE(void)
gdImageFillToBorder(gdImagePtr im, int x, int y, int border, int color);

/**
 * @brief Flood fill an area of the image with a color
 * 
 * @param im    The image.
 * @param x     The x-coordinate of the starting point.
 * @param y     The y-coordinate of the starting point.
 * @param color The color to fill with.
 */
BGD_DECLARE(void) gdImageFill(gdImagePtr im, int x, int y, int color);


/** @defgroup cloneandcopy Clone, copy and image properties
 * @{ */
/**
 * @brief Copy an area of an image to another image
 *
 * @param dst  - The destination image.
 * @param src  - The source image.
 * @param dstX - The x-coordinate of the upper left corner to copy to.
 * @param dstY - The y-coordinate of the upper left corner to copy to.
 * @param srcX - The x-coordinate of the upper left corner to copy from.
 * @param srcY - The y-coordinate of the upper left corner to copy from.
 * @param w    - The width of the area to copy.
 * @param h    - The height of the area to copy.
 *
 * @see gdImageCopyMerge gdImageCopyMergeGray gdImageCopyResized gdImageCopyResampled gdImageCopyRotated gdImageScale gdImageScaleWithOptions
 */
BGD_DECLARE(void)
gdImageCopy(gdImagePtr dst, gdImagePtr src, int dstX, int dstY, int srcX, int srcY, int w, int h);

/**
 * @brief Copy an area of an image to another image ignoring alpha
 *
 * The source area will be copied to the destination are by merging the pixels.
 *
 * @note This function is a substitute for real alpha channel operations, so it doesn't pay attention to the alpha channel.
 *
 * @param dst  The destination image.
 * @param src  The source image.
 * @param dstX The x-coordinate of the upper left corner to copy to.
 * @param dstY The y-coordinate of the upper left corner to copy to.
 * @param srcX The x-coordinate of the upper left corner to copy from.
 * @param srcY The y-coordinate of the upper left corner to copy from.
 * @param w    The width of the area to copy.
 * @param h    The height of the area to copy.
 * @param pct  The percentage in range 0..100.
 *
 * @see gdImageCopy  gdImageCopyMergeGray
 */
BGD_DECLARE(void)
gdImageCopyMerge(gdImagePtr dst, gdImagePtr src, int dstX, int dstY, int srcX, int srcY, int w,
                 int h, int pct);


/**
 * @brief Copy an area of an image to another image ignoring alpha
 *
 * The source area will be copied to the grayscaled destination area by merging
 * the pixels.
 *
 * @note This function is a substitute for real alpha channel operations, so it doesn't pay attention to the alpha channel.
 *
 * @param dst  - The destination image.
 * @param src  - The source image.
 * @param dstX - The x-coordinate of the upper left corner to copy to.
 * @param dstY - The y-coordinate of the upper left corner to copy to.
 * @param srcX - The x-coordinate of the upper left corner to copy from.
 * @param srcY - The y-coordinate of the upper left corner to copy from.
 * @param w    - The width of the area to copy.
 * @param h    - The height of the area to copy.
 * @param pct  - The percentage of the source color intensity in range 0..100.
 *
 * @see gdImageCopy gdImageCopyMerge
 */
BGD_DECLARE(void)
gdImageCopyMergeGray(gdImagePtr dst, gdImagePtr src, int dstX, int dstY, int srcX, int srcY, int w,
                     int h, int pct);


/**
 * @brief Copy a resized area from an image to another image
 *
 * If the source and destination area differ in size, the area will be resized
 * using nearest-neighbor interpolation.
 *
 * @param dst  The destination image.
 * @param src  The source image.
 * @param dstX The x-coordinate of the upper left corner to copy to.
 * @param dstY The y-coordinate of the upper left corner to copy to.
 * @param srcX The x-coordinate of the upper left corner to copy from.
 * @param srcY The y-coordinate of the upper left corner to copy from.
 * @param dstW The width of the area to copy to.
 * @param dstH The height of the area to copy to.
 * @param srcW The width of the area to copy from.
 * @param srcH The height of the area to copy from.
 *
 * @see gdImageCopyResampled gdImageScale
 */
BGD_DECLARE(void)
gdImageCopyResized(gdImagePtr dst, gdImagePtr src, int dstX, int dstY, int srcX, int srcY, int dstW,
                   int dstH, int srcW, int srcH);

/**
 * @brief Copy a resampled area from an image to another image
 *
 * If the source and destination area differ in size, the area will be resized
 * using bilinear interpolation for truecolor images, and nearest-neighbor
 * interpolation for palette images.
 *
 * @param dst  The destination image.
 * @param src  The source image.
 * @param dstX The x-coordinate of the upper left corner to copy to.
 * @param dstY The y-coordinate of the upper left corner to copy to.
 * @param srcX The x-coordinate of the upper left corner to copy from.
 * @param srcY The y-coordinate of the upper left corner to copy from.
 * @param dstW The width of the area to copy to.
 * @param dstH The height of the area to copy to.
 * @param srcW The width of the area to copy from.
 * @param srcH The height of the area to copy from.
 *
 * @see gdImageCopyResized gdImageScale
 */
BGD_DECLARE(void)
gdImageCopyResampled(gdImagePtr dst, gdImagePtr src, int dstX, int dstY, int srcX, int srcY,
                     int dstW, int dstH, int srcW, int srcH);

/**
 * @brief Copy a rotated area from an image to another image
 *
 * The area is counter-clockwise rotated using nearest-neighbor interpolation.
 *
 * @param dst The destination image.
 * @param src The source image.
 * @param  dstX  The x-coordinate of the center of the area to copy to.
 * @param  dstY  The y-coordinate of the center of the area to copy to.
 * @param  srcX  The x-coordinate of the upper left corner to copy from.
 * @param  srcY  The y-coordinate of the upper left corner to copy from.
 * @param  srcW  The width of the area to copy from.
 * @param  srcH  The height of the area to copy from.
 * @param  angle The angle in degrees.
 *
 * @see gdImageRotateInterpolated
 */
BGD_DECLARE(void)
gdImageCopyRotated(gdImagePtr dst, gdImagePtr src, double dstX, double dstY, int srcX, int srcY,
                   int srcWidth, int srcHeight, int angle);

/**
 * @brief Clones an image
 *
 * Creates an exact duplicate of the given image.
 *
 * @param src The source image.
 *
 * @returns The cloned image on success, NULL on failure.
 */
BGD_DECLARE(gdImagePtr) gdImageClone(gdImagePtr src);

/**
 * @brief Sets whether an image is interlaced
 *
 * This is relevant only when saving the image in a format that supports
 * interlacing.
 *
 * @param im            The image.
 * @param interlaceArg  Whether the image is interlaced.
 *
 * @see gdImageGetInterlaced
 */
BGD_DECLARE(void) gdImageInterlace(gdImagePtr im, int interlaceArg);

/** @} */

/**
 * @brief Sets the effect for subsequent drawing operations
 *
 * @note The effect is used for truecolor images only.
 *
 * @note in gd 2.4+, a configure flag is available to use the accurate and correct blending algorithm for truecolor images. This is the default behavior.
 *       The old, faster, but less accurate algorithm can be used by configuring gd with --disable-accurate-blending.
 * @param im The image.
 * @param alphaBlendingArg The effect.
 *
 * Effects: @ref gdEffectOverlay, @ref gdEffectMultiply, @ref gdEffectNormal
 * 
 * 
 */
BGD_DECLARE(void) gdImageAlphaBlending(gdImagePtr im, int alphaBlendingArg);

/**
 * @brief Sets the save alpha flag
 *
 * The save alpha flag specifies whether the alpha channel of the pixels should
 * be saved. This is supported only for image formats that support full alpha
 * transparency, e.g. PNG.
 * 
 * @param im The image.
 * @param saveAlphaArg The save alpha flag (1 to save alpha, 0 to not save alpha).
 */
BGD_DECLARE(void) gdImageSaveAlpha(gdImagePtr im, int saveAlphaArg);

/**
 * @defgroup  Color Quantization
 * 
 * @{ */

 /**
 * Note that @ref GD_QUANT_JQUANT does not retain the alpha channel, and
 * @ref GD_QUANT_NEUQUANT does not support dithering.
 *
 * @see gdImageTrueColorToPaletteSetMethod
 */
enum gdPaletteQuantizationMethod {
    GD_QUANT_DEFAULT = 0, /**< Default quantization method */
    GD_QUANT_JQUANT = 1, /**< libjpeg's old median cut */
    GD_QUANT_NEUQUANT = 2, /**< NeuQuant - approximation using Kohonen neural network */
    GD_QUANT_LIQ = 3 /**< libimagequant combination aiming for highest quality */
};


/**
 * @brief Creates a new palette image from a truecolor image
 *
 * This is the same as calling @ref gdImageCreatePaletteFromTrueColor with the
 * quantization method @ref GD_QUANT_NEUQUANT.
 *
 * @param im            - The image.
 * @param max_color     - The number of desired palette entries.
 * @param sample_factor - The quantization precision between 1 (highest quality) and
 *                        10 (fastest).
 *
 * @returns A newly create palette image; NULL on failure.
 */
BGD_DECLARE(gdImagePtr)
gdImageNeuQuant(gdImagePtr im, const int max_color, int sample_factor);


/**  @brief Selects quantization method used for subsequent @ref gdImageTrueColorToPalette calls.
 *
 *   @details See @ref gdPaletteQuantizationMethod enum (e.g. @ref GD_QUANT_NEUQUANT,
 *   @ref GD_QUANT_LIQ). Speed is from 1 (highest quality) to 10 (fastest). Speed 0
 *   selects method-specific default (recommended).
 * 
 *   @param im The image to set the quantization method for.
 *   @param method The quantization method to use.
 *   @param speed The speed/quality tradeoff for the selected method.
 * 
 *   @returns FALSE if the given method is invalid or not available.
 */
BGD_DECLARE(int)
gdImageTrueColorToPaletteSetMethod(gdImagePtr im, int method, int speed);

/**
 * @brief Sets the quality range for subsequent @ref gdImageTrueColorToPalette calls.
 * @details  Chooses quality range that subsequent call to @ref gdImageTrueColorToPalette will
 * aim for. Min and max quality is in range 1-100 (1 = ugly, 100 = perfect). Max
 * must be higher than min. If palette cannot represent image with at least
 * min_quality, then image will remain true-color. If palette can represent image
 * with quality better than max_quality, then lower number of colors will be
 * used. This function has effect only when @ref GD_QUANT_LIQ method has been selected
 * and the source image is true-color.
 *
 *   @param im           The image.
 *   @param min_quality  The minimum quality in range 1-100 (1 = ugly, 100 = perfect).
 *                       If the palette cannot represent the image with at least
 *                       min_quality, then no conversion is done.
 *   @param max_quality  The maximum quality in range 1-100 (1 = ugly, 100 = perfect),
 *                       which must be higher than the min_quality. If the palette can
 *                       represent the image with a quality better than max_quality,
 *                       then fewer colors than requested will be used.
 */
BGD_DECLARE(void)
gdImageTrueColorToPaletteSetQuality(gdImagePtr im, int min_quality, int max_quality);


/** 
 * @brief Converts a truecolor image to a palette-based image.
 * @details This function converts a truecolor image to a palette-based image
 * using a high-quality two-pass quantization routine
 * which attempts to preserve alpha channel information
 * as well as R/G/B color information when creating
 * a palette. If ditherFlag is set, the image will be
 * dithered to approximate colors better, at the expense
 * of some obvious "speckling." colorsWanted can be
 * anything up to 256. If the original source image
 * includes photographic information or anything that
 * came out of a JPEG, 256 is strongly recommended.
 * 
 * Better yet, don't use these function -- write real
 * truecolor PNGs and JPEGs. The disk space gain of
 * conversion to palette is not great (for small images
 * it can be negative) and the quality loss is ugly.
 *
 * DIFFERENCES: @ref gdImageCreatePaletteFromTrueColor creates and
 * returns a new image. @ref gdImageTrueColorToPalette modifies
 * an existing image, and the truecolor pixels are discarded.
 *
 * @param im           The image.
 * @param dither       Whether dithering should be applied.
 * @param colorsWanted The number of desired palette entries.
 * 
 * @returns a newly created palette image on success, NULL on failure.
 */
BGD_DECLARE(gdImagePtr)
gdImageCreatePaletteFromTrueColor(gdImagePtr im, int ditherFlag, int colorsWanted);


/**
 * @brief Converts a truecolor image to a palette image
 *
 * @param im           The image.
 * @param dither       Whether dithering should be applied.
 * @param colorsWanted The number of desired palette entries.
 *
 * @return Non-zero if the conversion succeeded, zero otherwise.
 *
 * @see gdImageCreatePaletteFromTrueColor gdImageTrueColorToPaletteSetMethod  gdImagePaletteToTrueColor
 */
BGD_DECLARE(int)
gdImageTrueColorToPalette(gdImagePtr im, int ditherFlag, int colorsWanted);

/** @brief Converts a palette-based image to a truecolor image 
 * 
 * @details This function converts a palette-based image to a truecolor image. The
 * palette is discarded, and the image is converted to truecolor. The alpha channel
 * information is preserved.
 * @param src The source image.
 * @return Non-zero if the conversion succeeded, zero otherwise.
 *
 */
BGD_DECLARE(int) gdImagePaletteToTrueColor(gdImagePtr src);

/** @} */

/**
 * @defgroup  ImageFilters Image Filters and convolutions
 * @{ */
/**
 * @brief @ref gdImagePixelate options
 *
 * Negate the imag src, white becomes black,
 * The red, green, and blue intensities of an image are negated.
 * White becomes black, yellow becomes blue, etc.
 */
enum gdPixelateMode { 
    GD_PIXELATE_UPPERLEFT, /**< Use the upper-left pixel of each block */
    GD_PIXELATE_AVERAGE /**< Use the average color of each block */
};

/**
 * @brief Pixelates an image
 * 
 * Pixelates an image by dividing it into blocks of the specified size and replacing each block with a single color.
 * The color can be determined by either the upper-left pixel of the block or the average color of all pixels in the block, depending on the mode specified.
 * 
 * @param im The image to pixelate.
 * @param block_size The size of the blocks to use for pixelation. Must be greater than 0.
 * @param mode The mode to use for determining the color of each block. @ref gdPixelateMode
 * 
 * @return Non-zero on success, zero on failure. Failure: Returns zero if im is NULL or block_size is less than or equal to 0.
 */
BGD_DECLARE(int)
gdImagePixelate(gdImagePtr im, int block_size, const unsigned int mode);

/**
 * @brief Options to Scatter an image 
 */
typedef struct {
    int sub; /**< The subtraction value for scattering. */
    int plus; /**< The addition value for scattering. */
    unsigned int num_colors; /**< The number of colors to use for scattering. */
    int *colors; /**< The array of colors to use for scattering. */
    unsigned int seed; /**< The seed for the random number generator. */
} gdScatter, *gdScatterPtr;

/**
 * @brief Scatter an image
 * 
 * Scatters the pixels of an image by randomly adjusting their color values based on the specified subtraction and addition values.
 * 
 * @param im The image to scatter.
 * @param sub The subtraction value for scattering. Must be greater than or equal to 0.
 * @param plus The addition value for scattering. Must be greater than or equal to 0
 * 
 * @return Non-zero on success, zero on failure. Failure: Returns zero if im is NULL, sub or plus is less than 0.
 */
BGD_DECLARE(int) gdImageScatter(gdImagePtr im, int sub, int plus);

/**
 * @brief Scatter an image with specified colors
 * 
 * Scatters the pixels of an image by randomly adjusting their color values based on the specified subtraction and addition values, using a specified set of colors.
 * 
 * @param im The image to scatter.
 * @param sub The subtraction value for scattering. Must be greater than or equal to 0.
 * @param plus The addition value for scattering. Must be greater than or equal to 0.
 * @param colors An array of colors to use for scattering. Must not be NULL.
 * @param num_colors The number of colors in the colors array. Must be greater than
 * 
 * @return Non-zero on success, zero on failure. Failure: Returns zero if im is NULL, sub or plus is less than 0, colors is NULL, or num_colors is 0.
 */
BGD_DECLARE(int) gdImageScatterColor(gdImagePtr im, int sub, int plus, int colors[], unsigned int num_colors);

/**
 * @brief Scatter an image with extended options
 * 
 * Scatters the pixels of an image using the specified scattering options.
 * 
 * @param im The image to scatter.
 * @param s A pointer to a gdScatter structure containing the scattering options. Must not be NULL.
 * 
 * @return
 */
BGD_DECLARE(int) gdImageScatterEx(gdImagePtr im, gdScatterPtr s);

/**
 * @brief Smooth an image
 *
 * (see smooth.jpg)
 *
 * @param im     The image.
 * @param weight The strength of the smoothing.
 *
 * @return Non-zero on success, zero on failure.
 *
 * @see gdImageConvolution
 */

/**
 * @brief Smooth an image
 *
 * Smooth an image
 *
 * (see smooth.jpg)
 * 
 * @param src        The source image.
 * @param weight     The strength of the smoothing.
 *
 * @return Non-zero on success, zero on failure. Failure: Returns zero if im is NULL or weight is invalid.
 *
 * @see gdImageConvolution @ref gdImageGaussianBlur gdImageEmboss gdImageMeanRemoval
 */
 BGD_DECLARE(int) gdImageSmooth(gdImagePtr im, float weight);

/**
 * @brief Mean removal of an image
 *
 * (see mean_removal.jpg)
 *
 * @param im The image.
 *
 * @return Non-zero on success, zero on failure.
 *
 * @see gdImageEdgeDetectQuick gdImageConvolution
 */
 BGD_DECLARE(int) gdImageMeanRemoval(gdImagePtr im);

/**
 * @brief Emboss an image
 *
 * (see emboss.jpg)
 *
 * @param im The image.
 *
 * @return Non-zero on success, zero on failure.
 *
 * @see gdImageConvolution
 */
BGD_DECLARE(int) gdImageEmboss(gdImagePtr im);

/**
 * @brief Gaussian blur of an image
* Performs a Gaussian blur of radius 1 on the
* image.  The image is modified in place.
*
* *NOTE:* You will almost certain want to use
* @ref gdImageCopyGaussianBlurred instead, as it allows you to change
* your kernel size and sigma value.  Future versions of this
* function may fall back to calling it instead of
* @ref gdImageConvolution, causing subtle changes so be warned.
*
* @param im The image to blur.
*
* @returns GD_TRUE (1) on success, GD_FALSE (0) on failure.
*
* @see @gdImageConvolution for more information on how to use convolution matrices to achieve different effects.
*/
BGD_DECLARE(int) gdImageGaussianBlur(gdImagePtr im);

/**
 * @brief Edge detection of an image
 *
 * (see edge_detect_quick.jpg)
 *
 * @param src The image.
 *
 * @return Non-zero on success, zero on failure.
 *
 * @see gdImageMeanRemoval gdImageConvolution
 */
BGD_DECLARE(int) gdImageEdgeDetectQuick(gdImagePtr src);

/**
 * @brief Selective blur of an image
 * 
 * @param src The image.
 *
 * @return Non-zero on success, zero on failure.
 */
BGD_DECLARE(int) gdImageSelectiveBlur(gdImagePtr src);

/**
 * @brief Apply a convolution matrix to an image.
 *
 * Depending on the matrix, a wide range of effects can be accomplished, e.g.
 * blurring, sharpening, embossing, and edge detection.
 *
 * @param src        The image.
 * @param filter     The 3x3 convolution matrix.
 * @param filter_div The value to divide the convoluted channel values by.
 * @param offset     The value to add to the convoluted channel values.
 *
 * @return Non-zero on success, zero on failure.
 *
 * @see gdImageEdgeDetectQuick
 * @see gdImageGaussianBlur
 * @see gdImageEmboss
 * @see gdImageMeanRemoval
 * @see gdImageSmooth
 */
BGD_DECLARE(int)
gdImageConvolution(gdImagePtr src, float filter[3][3], float filter_div, float offset);

/**
 * @brief Change channel values of an image
 *
 * @param src   The image.
 * @param red   The value to add to the red channel of all pixels.
 * @param green The value to add to the green channel of all pixels.
 * @param blue  The value to add to the blue channel of all pixels.
 * @param alpha The value to add to the alpha channel of all pixels.
 *
 * @return Non-zero on success, zero on failure.
 *
 * @see gdImageBrightness
 */
BGD_DECLARE(int)
gdImageColor(gdImagePtr src, const int red, const int green, const int blue, const int alpha);

/**
 * @brief Change the contrast of an image
 *
 * @param src      The image.
 * @param contrast The contrast adjustment value. Negative values increase, positive
 *                 values decrease the contrast. The larger the absolute value, the
 *                 stronger the effect.
 *
 * @return Non-zero on success, zero on failure.
 *
 * @see gdImageBrightness
 */
BGD_DECLARE(int) gdImageContrast(gdImagePtr src, double contrast);

/**
 * @brief Change the brightness of an image
 *
 * @param src        The image.
 * @param brightness The value to add to the color channels of all pixels.
 *
 * @return Non-zero on success, zero on failure.
 *
 * @see gdImageContrast gdImageColor
 */
BGD_DECLARE(int) gdImageBrightness(gdImagePtr src, int brightness);


/**
 * @brief Convert an image to grayscale
 *
 * The red, green and blue components of each pixel are replaced by their
 * weighted sum using the same coefficients as the REC.601 luma (Y')
 * calculation. The alpha components are retained.
 *
 * For palette images the result may differ due to palette limitations.
 *
 * @param src The image.
 *
 * @return Non-zero on success, zero on failure.
 */
BGD_DECLARE(int) gdImageGrayScale(gdImagePtr src);

/**
 * @brief Invert an image
 *
 * @param src The image.
 *
 * @return Non-zero on success, zero on failure.
 */
BGD_DECLARE(int) gdImageNegate(gdImagePtr src);

/**
 * @brief Return a copy of the source image _src_ blurred according to the parameters using the Gaussian Blur algorithm.
 * Return a copy of the source image _src_ blurred according to the
 * parameters using the Gaussian Blur algorithm.
 * 
 * _radius_ is a radius, not a diameter so a radius of 2 (for
 * example) will blur across a region 5 pixels across (2 to the
 * center, 1 for the center itself and another 2 to the other edge).
 * 
 * _sigma_ represents the "fatness of the curve (lower == fatter).
 * If _sigma_ is less than or equal to 0,
 * <gdImageCopyGaussianBlurred> ignores it and instead computes an
 * "optimal" value.  Be warned that future versions of this function
 * may compute sigma differently.
 * 
 * The resulting image is always truecolor.
 * 
 * More Details:
 * 
 * A Gaussian Blur is generated by replacing each pixel's color
 * values with the average of the surrounding pixels' colors.  This
 * region is a circle whose radius is given by argument _radius_.
 * Thus, a larger radius will yield a blurrier image.
 * 
 * This average is not a simple mean of the values.  Instead, values
 * are weighted using the Gaussian function (roughly a bell curve
 * centered around the destination pixel) giving it much more
 * influence on the result than its neighbours.  Thus, a fatter curve
 * will give the center pixel more weight and make the image less
 * blurry; lower _sigma_ values will yield flatter curves.
 * 
 * Currently, <gdImageCopyGaussianBlurred> computes the default sigma
 * as
 * 
 * (2/3)*radius
 * 
 * Note, however that we reserve the right to change this if we find
 * a better ratio.  If you absolutely need the current sigma value,
 * you should set it yourself.
 *
 * @param  src  the source image
 * @param  radius  the blur radius (*not* diameter--range is 2*radius + 1)
 * @param  sigma   the sigma value or a value <= 0.0 to use the computed default
 *
 * @return The new image or NULL if an error occurred.  The result is always truecolor.
 *
 * Example:
 * @code
 * 
 * FILE *in;
 * gdImagePtr result, src;
 * 
 * in = fopen("foo.png", "rb");
 * src = gdImageCreateFromPng(in);
 * 
 * result = gdImageCopyGaussianBlurred(im, src->sx / 10, -1.0);
 * 
 * @endcode
 */
BGD_DECLARE(gdImagePtr)
gdImageCopyGaussianBlurred(gdImagePtr src, int radius, double sigma);
/** @} */

/**
 * Group: Accessor Macros
 */

/**
 * Macro: gdImageTrueColor
 *
 * Whether an image is a truecolor image.
 *
 * Parameters:
 *   im - The image.
 *
 * Returns:
 *   Non-zero if the image is a truecolor image, zero for palette images.
 */
#define gdImageTrueColor(im) ((im)->trueColor)

/**
 * Macro: gdImageSX
 *
 * Gets the width (in pixels) of an image.
 *
 * Parameters:
 *   im - The image.
 */
#define gdImageSX(im) ((im)->sx)

/**
 * Macro: gdImageSY
 *
 * Gets the height (in pixels) of an image.
 *
 * Parameters:
 *   im - The image.
 */
#define gdImageSY(im) ((im)->sy)

/**
 * Macro: gdImageColorsTotal
 *
 * Gets the number of colors in the palette.
 *
 * This macro is only valid for palette images.
 *
 * Parameters:
 *   im - The image
 */
#define gdImageColorsTotal(im) ((im)->colorsTotal)

/**
 * Macro: gdImageRed
 *
 * Gets the red component value of a given color.
 *
 * Parameters:
 *   im - The image.
 *   c  - The color.
 */
#define gdImageRed(im, c) ((im)->trueColor ? gdTrueColorGetRed(c) : (im)->red[(c)])

/**
 * Macro: gdImageGreen
 *
 * Gets the green component value of a given color.
 *
 * Parameters:
 *   im - The image.
 *   c  - The color.
 */
#define gdImageGreen(im, c) ((im)->trueColor ? gdTrueColorGetGreen(c) : (im)->green[(c)])

/**
 * Macro: gdImageBlue
 *
 * Gets the blue component value of a given color.
 *
 * Parameters:
 *   im - The image.
 *   c  - The color.
 */
#define gdImageBlue(im, c) ((im)->trueColor ? gdTrueColorGetBlue(c) : (im)->blue[(c)])

/**
 * Macro: gdImageAlpha
 *
 * Gets the alpha component value of a given color.
 *
 * Parameters:
 *   im - The image.
 *   c  - The color.
 */
#define gdImageAlpha(im, c) ((im)->trueColor ? gdTrueColorGetAlpha(c) : (im)->alpha[(c)])

/**
 * Macro: gdImageGetTransparent
 *
 * Gets the transparent color of the image.
 *
 * Parameters:
 *   im - The image.
 *
 * See also:
 *   - <gdImageColorTransparent>
 */
#define gdImageGetTransparent(im) ((im)->transparent)

/**
 * Macro: gdImageGetInterlaced
 *
 * Whether an image is interlaced.
 *
 * Parameters:
 *   im - The image.
 *
 * Returns:
 *   Non-zero for interlaced images, zero otherwise.
 *
 * See also:
 *   - <gdImageInterlace>
 */
#define gdImageGetInterlaced(im) ((im)->interlace)

/**
 * Macro: gdImagePalettePixel
 *
 * Gets the color of a pixel.
 *
 * Calling this macro is only valid for palette images.
 * No bounds checking is done for the coordinates.
 *
 * Parameters:
 *   im - The image.
 *   x  - The x-coordinate.
 *   y  - The y-coordinate.
 *
 * See also:
 *   - <gdImageTrueColorPixel>
 *   - <gdImageGetPixel>
 */
#define gdImagePalettePixel(im, x, y) (im)->pixels[(y)][(x)]

/**
 * Macro: gdImageTrueColorPixel
 *
 * Gets the color of a pixel.
 *
 * Calling this macro is only valid for truecolor images.
 * No bounds checking is done for the coordinates.
 *
 * Parameters:
 *   im - The image.
 *   x  - The x-coordinate.
 *   y  - The y-coordinate.
 *
 * See also:
 *   - <gdImagePalettePixel>
 *   - <gdImageGetTrueColorPixel>
 */
#define gdImageTrueColorPixel(im, x, y) (im)->tpixels[(y)][(x)]

/**
 * Macro: gdImageResolutionX
 *
 * Gets the horizontal resolution in DPI.
 *
 * Parameters:
 *   im - The image.
 *
 * See also:
 *   - <gdImageResolutionY>
 *   - <gdImageSetResolution>
 */
#define gdImageResolutionX(im) (im)->res_x

/**
 * Macro: gdImageResolutionY
 *
 * Gets the vertical resolution in DPI.
 *
 * Parameters:
 *   im - The image.
 *
 * See also:
 *   - <gdImageResolutionX>
 *   - <gdImageSetResolution>
 */
#define gdImageResolutionY(im) (im)->res_y

/* I/O Support routines. */
/**
 * @defgroup gdIOCtx I/O Contexts
 * @{
 */
/** 
 * @brief Creates a new I/O context for reading/writing to a file
 * 
 * returns a new I/O context for reading/writing to the specified file. The caller is responsible for closing the file when done.
 * @param file A pointer to a FILE object that identifies the file to be used for I/O.
 * @return A pointer to a new gdIOCtx structure, or NULL on failure.
 */
BGD_DECLARE(gdIOCtxPtr) gdNewFileCtx(FILE *);

/**
 * @brief Creates a new I/O context for reading/writing to a dynamic memory buffer
 * 
 * If data is null, size is ignored and an initial data buffer is allocated automatically.
 * This function assumes gd has the right to free or reallocate "data" at will! 
 * Also note that gd will free "data" when the IO context is freed. 
 * If data is not null, it must point to memory allocated with gdMalloc, or
 * by a call to gdImage[something]Ptr. If not, see gdNewDynamicCtxEx for an alternative.
 * 
 * @param size The initial size of the dynamic memory buffer.
 * @param data A pointer to a memory buffer, or NULL to allocate a new buffer.
 * @return A pointer to a new gdIOCtx structure, or NULL on failure.
 */
BGD_DECLARE(gdIOCtxPtr) gdNewDynamicCtx(int size, void *data);

/**
 * @brief Creates a new I/O context for reading/writing to a dynamic memory buffer with control over memory management
 * 2.0.21: if freeFlag is nonzero, gd will free and/or reallocate "data" as
 * needed as described above. If freeFlag is zero, gd will never free
 * or reallocate "data", which means that the context should only be used
 * for *reading* an image from a memory buffer, or writing an image to a
 * memory buffer which is already large enough. If the memory buffer is
 * not large enough and an image write is attempted, the write operation
 * will fail. Those wishing to write an image to a buffer in memory have
 * a much simpler alternative in the gdImage[something]Ptr functions
 * 
 * @param size The initial size of the dynamic memory buffer.
 * @param data A pointer to a memory buffer, or NULL to allocate a new buffer.
 * @param freeFlag A flag indicating whether gd should manage the memory of "data". If nonzero, gd will free and/or reallocate "data" as needed. If zero, gd will not free or reallocate "data".
 * 
 * @return A pointer to a new gdIOCtx structure, or NULL on failure.
 */
BGD_DECLARE(gdIOCtxPtr) gdNewDynamicCtxEx(int size, void *data, int freeFlag);

/** @deprecated will be removed in a future version in favor of gdNewDynamicCtxEx and retated CTX APIs */
BGD_DECLARE(gdIOCtxPtr) gdNewSSCtx(gdSourcePtr in, gdSinkPtr out);
BGD_DECLARE(void *) gdDPExtractData(gdIOCtxPtr ctx, int *size);
/** @} */

/**
 * @addtogroup gdCodecGd2
 * @{
 */

/** @name GD2 Constants */
/** @{ */

/** Default GD2 chunk size in pixels. */
#define GD2_CHUNKSIZE 128
/** Minimum accepted GD2 chunk size in pixels. */
#define GD2_CHUNKSIZE_MIN 64
/** Maximum accepted GD2 chunk size in pixels. */
#define GD2_CHUNKSIZE_MAX 4096

/** Current GD2 file format version written by gd. */
#define GD2_VERS 2
/** GD2 file signature string. */
#define GD2_ID "gd2"
/** Write uncompressed GD2 chunks. */
#define GD2_FMT_RAW 1
/** Write zlib-compressed GD2 chunks. */
#define GD2_FMT_COMPRESSED 2

/** @} */

/** @} */


/**
 * @defgroup imagecomparison Image Comparison
 * @{ */
#define GD_CMP_IMAGE 1 /**< Actual image IS different */
#define GD_CMP_NUM_COLORS 2 /**< Number of colors in palette differ */
#define GD_CMP_COLOR 4 /**< Image colors differ */
#define GD_CMP_SIZE_X 8 /**< Image width differs */
#define GD_CMP_SIZE_Y 16 /**< Image heights differ */
#define GD_CMP_TRANSPARENT 32 /**< Transparent color differs */
#define GD_CMP_BACKGROUND 64 /**< Background color differs */
#define GD_CMP_INTERLACE 128 /**< Interlaced setting differs */
#define GD_CMP_TRUECOLOR 256 /**< Truecolor vs palette differs */

/**
 * @brief Compare two images
 *
 * compare two images and some of its attributes. The images must be of the same size, otherwise the function will return -1.
 * For accurate image content comparison, use @ref gdImagePerceptualDiff instead.
 * 
 * @param im1 An image.
 * @param im2 Another image.
 *
 * @return A bitmask of @ref <Image Comparison> flags where each set flag signals which attributes of the images are different.
 */
BGD_DECLARE(int) gdImageCompare(gdImagePtr im1, gdImagePtr im2);

/** @brief Options for perceptual image comparison mode
 */
typedef enum {
    GD_IMAGE_DIFF_NONE, /**< No difference */
    GD_IMAGE_DIFF_OVERLAY, /**< Overlay difference */
    GD_IMAGE_DIFF_MASK /**< Mask difference */
} gdImageDiffMode;

/** @brief Options for perceptual image comparison */
typedef struct {
    gdImageDiffMode mode; /**< The mode of the perceptual difference. */
    int highlight_color; /**< The color used to highlight differences. */
} gdImagePerceptualDiffOptions;

/** @brief Result of perceptual image comparison */
typedef struct {
    unsigned int pixels_changed; /**< Number of pixels that changed. */
    /* Largest normalized perceptual distance, in the range 0.0 to 1.0. */
    double maximum_delta; /**< The maximum perceptual distance found. */
} gdImagePerceptualDiffResult;

/*
 * Compare two equally sized images using a perceptual YIQ distance.
 *
 * A NULL options pointer selects an overlay with opaque red highlights. A
 * non-NULL diff_image receives a newly allocated truecolor image for overlay
 * and mask modes; the caller owns it and must call @ref gdImageDestroy. Passing
 * NULL for diff_image computes statistics only. The result is always reset,
 * including on failure.
 *
 * Returns 1 on success, or 0 for invalid arguments or allocation failure.
 */
BGD_DECLARE(int)
gdImagePerceptualDiff(gdImagePtr image1, gdImagePtr image2, double threshold,
                      const gdImagePerceptualDiffOptions *options,
                      gdImagePtr *diff_image,
                      gdImagePerceptualDiffResult *result);
/** @} */


/**
 * @defgroup TransformScaleRotate Transform, scale and rotate
 *
 * Image transformation APIs for interpolation, scaling, rotation and affine
 * mapping.
 *
 * Affine matrices use a six-element double array:
 *
 * matrix[0] == xx
 * matrix[1] == yx
 * matrix[2] == xy
 * matrix[3] == yy
 * matrix[4] == x0
 * matrix[5] == y0
 *
 * A point (x, y) is transformed as:
 *
 * x_new = xx * x + xy * y + x0
 * y_new = yx * x + yy * y + y0
 *
 * @{
 */

/**
 * @brief Flip an image vertically
 *
 * The image is mirrored upside-down.
 *
 * @param im The image.
 *
 * @see gdImageFlipHorizontal, gdImageFlipBoth
 */
BGD_DECLARE(void) gdImageFlipHorizontal(gdImagePtr im);

/**
 * @brief Flip an image horizontally
 *
 * The image is mirrored left-right.
 *
 * @param im The image.
 * @see gdImageFlipVertical, gdImageFlipBoth
 */
BGD_DECLARE(void) gdImageFlipVertical(gdImagePtr im);

/**
 * @brief Flip an image vertically and horizontally
 *
 * The image is mirrored upside-down and left-right.
 *
 * @param im The image.
 * @see gdImageFlipVertical, gdImageFlipHorizontal
 */
BGD_DECLARE(void) gdImageFlipBoth(gdImagePtr im);

/**
 * Group: Crop
 *
 * @see gdImageCropAuto gdImageCropThreshold gdCrop
 **/
enum gdCropMode {
    GD_CROP_DEFAULT = 0, /**< Same as GD_CROP_TRANSPARENT */
    GD_CROP_TRANSPARENT, /**< Crop using the transparent color */
    GD_CROP_BLACK, /**< Crop black borders */
    GD_CROP_WHITE, /**< Crop white borders */
    GD_CROP_SIDES, /**< Crop using colors of the 4 corners */
    GD_CROP_THRESHOLD /**< Crop using a threshold */
};

/**
 * @brief Crop an image to a given rectangle
 *
 * @param src The image.
 * @param crop The cropping rectangle, @ref gdRect.
 *
 * @returns The newly created cropped image, or NULL on failure.
 *
  * @see gdImageCrop gdImageCropThreshold gdImageAutoCropWithOptions
 */
BGD_DECLARE(gdImagePtr) gdImageCrop(gdImagePtr src, const gdRect *crop);


/**
 * @brief Crop an image automatically
 *
 * This function detects the cropping area according to the given _mode_.
 *
 * @param im The image.
 * @param mode The cropping mode, @ref gdCropMode.
 *
 * @returns The newly created cropped image, or NULL on failure.
 *
 * @see gdImageCrop gdImageCropThreshold gdImageAutoCropWithOptions
 */
BGD_DECLARE(gdImagePtr) gdImageCropAuto(gdImagePtr im, const unsigned int mode);


/**
 * @brief Crop an image using a given color
 *
 * The _threshold_ defines the tolerance to be used while comparing the image
 * color and the color to crop. The method used to calculate the color
 * difference is based on the color distance in the RGB(A) cube.
 *
 * @param im The image.
 * @param color The crop color.
 * @param threshold The crop threshold.
 *
 * @returns The newly created cropped image, or NULL on failure.
 *
 * @see gdImageCrop gdImageCropThreshold gdImageAutoCropWithOptions
 */
BGD_DECLARE(gdImagePtr)
gdImageCropThreshold(gdImagePtr im, const unsigned int color, const float threshold);

/**
 * @brief Options for automatic cropping
 * 
 * This structure defines the options for automatic cropping.
 */
typedef struct {
    enum gdCropMode mode; /**< The cropping mode, @see gdCropMode. */
    float threshold; /**< The crop threshold. */
    int color; /**< The crop color. */
} gdAutoCropOptions;

/**
 * @brief Crop an image automatically with options
 * 
 * This function detects the cropping area according to the given options.
 * 
 * @param src The image.
 * @param options The cropping options, @ref gdAutoCropOptions.
 * 
 * @returns The newly created cropped image, or NULL on failure.
 */
BGD_DECLARE(gdImagePtr)
gdImageAutoCropWithOptions(gdImagePtr src, const gdAutoCropOptions *options);

/**
 * @brief Set the interpolation method stored on an image.
 *
 * Scaling, rotation and affine transformation functions use this value when
 * they sample pixels from the image. Newly-created images default to
 * @ref GD_BILINEAR_FIXED. Passing @ref GD_DEFAULT is accepted and stores
 * @ref GD_LINEAR.
 *
 * Some transform APIs have optimized paths for specific methods. In
 * particular, @ref gdImageScale uses @ref GD_TRIANGLE when downscaling or doing a
 * mixed-axis scale with the fixed compatibility methods.
 *
 * Parameters:
 *   im - The image.
 *   id - The interpolation method.
 *
 * Returns:
 *   Non-zero on success, zero on failure.
 *
 * See also:
 *   - @see gdInterpolationMethod
 *   - @see gdImageGetInterpolationMethod
 */
BGD_DECLARE(int)
gdImageSetInterpolationMethod(gdImagePtr im, gdInterpolationMethod id);

/**
 * @brief Return the interpolation method currently stored on an image.
 *
 * Parameters:
 *   im - The image.
 *
 * Returns:
 *   The current interpolation method.
 *
 * See also:
 *   - @see gdInterpolationMethod
 *   - @see gdImageSetInterpolationMethod
 */
BGD_DECLARE(gdInterpolationMethod) gdImageGetInterpolationMethod(gdImagePtr im);

/**
 * @brief Scale an image to an exact width and height using the source image's current
 * @ref gdInterpolationMethod.
 *
 * The returned image is newly allocated and must be destroyed with
 * @ref gdImageDestroy. If the requested dimensions match the source dimensions,
 * this function returns a clone of the source image. Width and height must be
 * greater than zero.
 *
 * Notes:
 *   @ref GD_WEIGHTED4 is not supported by this function. For downscales and
 *   mixed-axis scales, the fixed compatibility methods are sampled with
 *   @ref GD_TRIANGLE for better filtering.
 *
 * Parameters:
 *   src        - The source image.
 *   new_width  - The requested output width.
 *   new_height - The requested output height.
 *
 * Returns:
 *   The scaled image on success, or NULL on failure.
 *
 * See also:
 *   - @see gdImageSetInterpolationMethod
 *   - @see gdImageScaleWithOptions
 *   - @see gdImageCopyResampled
 *   - @see gdImageCopyResized
 */
BGD_DECLARE(gdImagePtr)
gdImageScale(const gdImagePtr src, const unsigned int new_width, const unsigned int new_height);

/**
 * Constants: gdScaleFit
 *
 * Controls how @ref gdImageScaleWithOptions maps the source aspect ratio into the
 * requested output size.
 *
  * Defaults:
 *   When @ref gdImageScaleWithOptions receives NULL options, the fit defaults to
 *   @ref GD_SCALE_FIT_COVER.
 */
typedef enum {
    GD_SCALE_FIT_COVER,   /**< Preserve aspect ratio, fill requested size, crop overflow. */
    GD_SCALE_FIT_CONTAIN, /**< Preserve aspect ratio, fit inside requested size, pad the rest. */
    GD_SCALE_FIT_FILL,    /**< Stretch to requested size without preserving aspect ratio. */
    GD_SCALE_FIT_INSIDE,  /**< Preserve aspect ratio; output is no larger than requested size. */
    GD_SCALE_FIT_OUTSIDE  /**< Preserve aspect ratio; output is no smaller than requested size. */
} gdScaleFit;

/**
 * Chooses the anchor used when @ref gdImageScaleWithOptions pads or crops an
 * image. North and south refer to the top and bottom of the output; west and
 * east refer to the left and right.
 *
 * Defaults:
 *   When @ref gdImageScaleWithOptions receives NULL options, gravity defaults to
 *   @ref GD_SCALE_GRAVITY_CENTER.
 */
typedef enum {

    GD_SCALE_GRAVITY_NORTHWEST, /**< Anchor to the top-left corner. */
    GD_SCALE_GRAVITY_NORTH,     /**< Anchor to the top edge. */
    GD_SCALE_GRAVITY_NORTHEAST, /**< Anchor to the top-right corner. */
    GD_SCALE_GRAVITY_WEST,      /**< Anchor to the left edge. */
    GD_SCALE_GRAVITY_CENTER,    /**< Anchor to the center. */
    GD_SCALE_GRAVITY_EAST,      /**< Anchor to the right edge. */
    GD_SCALE_GRAVITY_SOUTHWEST, /**< Anchor to the bottom-left corner. */
    GD_SCALE_GRAVITY_SOUTH,     /**< Anchor to the bottom edge. */
    GD_SCALE_GRAVITY_SOUTHEAST  /**< Anchor to the bottom-right corner. */
} gdScaleGravity;

/**
 * Constants: gdScaleStrategy
 *
 * Optional crop strategy for @ref GD_SCALE_FIT_COVER in
 * @ref gdImageScaleWithOptions.
 *
 *
 * Notes:
 *   Entropy and attention strategies are valid only with
 *   @ref GD_SCALE_FIT_COVER. If a strategy cannot find an interesting crop,
 *   @ref gdImageScaleWithOptions falls back to the normal gravity-based cover
 *   crop.
 *
 * Defaults:
 *   When @ref gdImageScaleWithOptions receives NULL options, strategy defaults to
 *   @ref GD_SCALE_STRATEGY_NONE.
 */
typedef enum {
    GD_SCALE_STRATEGY_NONE,      /**< Crop using gravity only. */
    GD_SCALE_STRATEGY_ENTROPY,   /**< Prefer a high-entropy crop region. */
    GD_SCALE_STRATEGY_ATTENTION  /**< Prefer a likely visual-attention crop region. */
} gdScaleStrategy;

/**
 * Struct: gdScaleOptions
 *
 * Options for @ref gdImageScaleWithOptions.
 *
 * Members:
 *   fit              - Aspect-ratio behavior. See @ref gdScaleFit.
 *   gravity          - Crop or padding anchor. See @ref gdScaleGravity.
 *   strategy         - Optional cover-crop strategy. See @ref gdScaleStrategy.
 *   background_color - Truecolor ARGB background used for padding and for transparent palette pixels.
 *   interpolation    - A gdInterpolationMethod value (see @ref gdImageSetInterpolationMethod),
 *                      or @ref GD_SCALE_INTERPOLATION_AUTO.
 *
 * Defaults:
 *   If the options pointer passed to @ref gdImageScaleWithOptions is NULL, gd uses
 *   @ref GD_SCALE_FIT_COVER, @ref GD_SCALE_GRAVITY_CENTER,
 *   @ref GD_SCALE_STRATEGY_NONE, background color 0x7f000000 and
 *   @ref GD_SCALE_INTERPOLATION_AUTO.
 *
 * Notes:
 *   For palette sources, gd prepares a truecolor working copy. Transparent
 *   pixels are replaced with background_color before scaling; if
 *   background_color is a valid palette index, that palette entry is converted
 *   to truecolor.
 */
typedef struct {
    gdScaleFit fit;            /**< Aspect-ratio behavior. */
    gdScaleGravity gravity;    /**< Crop or padding anchor. */
    gdScaleStrategy strategy;  /**< Optional cover-crop strategy. */
    int background_color;      /**< Truecolor ARGB background used for padding and palette transparency. */
    int interpolation;         /**< Interpolation method, or @ref GD_SCALE_INTERPOLATION_AUTO. */
} gdScaleOptions;

/**
 * @brief Scale an image using aspect-ratio, gravity, crop-strategy and interpolation options.
 *
 * Scale an image using aspect-ratio, gravity, crop-strategy and interpolation
 * options.
 *
 * This is the higher-level scaling API. It can stretch, contain, cover, pad or
 * return an inside/outside size while preserving the source aspect ratio. The
 * returned image is newly allocated and must be destroyed with  @ref gdImageDestroy.
 * 
 * @param src        - The source image.
 * @param new_width  - The requested width.
 * @param new_height - The requested height.
 * @param options    - Scaling options, or NULL for the default options.
 *
 * @returns The scaled image on success, or NULL on failure. Caller owns the returned image and must call @ref gdImageDestroy when done.
 *          Returns NULL if src is NULL, either dimension is zero, interpolation is
 *          invalid, or an entropy/attention strategy is used with a fit other than @ref GD_SCALE_FIT_COVER.
 *
 * @see gdScaleOptions gdImageScale gdImageInterestingCropRegion
 */
BGD_DECLARE(gdImagePtr)
gdImageScaleWithOptions(const gdImagePtr src, const unsigned int new_width,
                        const unsigned int new_height, const gdScaleOptions *options);

/**
 * @brief  Methods used to find an interesting crop region.
 *  @see gdImageInterestingCropRegion gdImageScaleWithOptions
 */
typedef enum {
    GD_INTERESTING_ENTROPY,    /**< Prefer regions with higher image entropy. */
    GD_INTERESTING_ATTENTION   /**< Prefer regions with likely visual attention. */
} gdInterestingMethod;

/**
 * @brief Find a source crop region with the requested aspect ratio using an
 * interesting-region method.
 *
 * @param src           - The source image.
 * @param target_width  - The target width used to compute the crop aspect ratio.
 * @param target_height - The target height used to compute the crop aspect ratio.
 * @param method        - The interesting-region method.
 * @param crop          - Receives the crop rectangle on success.
 *
 * @returns GD_TRUE on success, or GD_FALSE on failure.
 *
 * @see gdInterestingMethod gdImageEntropyCropRegion gdImageScaleWithOptions
 */
BGD_DECLARE(int)
gdImageInterestingCropRegion(const gdImagePtr src, unsigned int target_width,
                             unsigned int target_height, gdInterestingMethod method,
                             gdRectPtr crop);

/**
 * @brief Find a high-entropy source crop region with the requested aspect ratio.
 *
 * @param src           - The source image.
 * @param target_width  - The target width used to compute the crop aspect ratio.
 * @param target_height - The target height used to compute the crop aspect ratio.
 * @param crop          - Receives the crop rectangle on success.
 *
 * @returns GD_TRUE on success, or GD_FALSE on failure.
 *
 * @see gdImageInterestingCropRegion
 */
BGD_DECLARE(int)
gdImageEntropyCropRegion(const gdImagePtr src, unsigned int target_width,
                         unsigned int target_height, gdRectPtr crop);

/**
 * @brief Rotate an image by an arbitrary angle using the source image's current @ref gdInterpolationMethod.
 *
 * The returned image is newly allocated and must be destroyed with
 * @ref gdImageDestroy. Palette sources are converted to truecolor internally for
 * arbitrary-angle rotation. Angles that are multiples of 90 degrees use the
 * optimized rotate paths.
 *
 * @param src     - The source image.
 * @param angle   - Rotation angle in degrees.
 * @param bgcolor - Background color for uncovered pixels. For palette sources, this may be a palette index.
 *
 * @returns The rotated image on success, or NULL on failure.
 *
 * @note bgcolor must be non-negative. The source interpolation method must be a valid concrete method; @ref GD_DEFAULT is not accepted by this function.
 *
 * @see gdImageSetInterpolationMethod gdImageRotate90 gdImageRotate180 gdImageRotate270
  */
BGD_DECLARE(gdImagePtr)
gdImageRotateInterpolated(const gdImagePtr src, const float angle, int bgcolor);

/**
 * @brief Standard affine matrix operations.
 */
typedef enum {
    GD_AFFINE_TRANSLATE = 0,   /**< Translation matrix. */
    GD_AFFINE_SCALE,           /**< Scale matrix. */
    GD_AFFINE_ROTATE,          /**< Rotation matrix. */
    GD_AFFINE_SHEAR_HORIZONTAL,/**< Horizontal shear matrix. */
    GD_AFFINE_SHEAR_VERTICAL   /**< Vertical shear matrix. */
} gdAffineStandardMatrix;

/**
 * @brief Apply an affine matrix to a floating-point point.
 *
 * @param dst    - Receives the transformed point.
 * @param src    - Source point.
 * @param affine - Matrix in gd's six-value affine form.
 *
 * @returns GD_TRUE on success, or GD_FALSE on failure.
 */
BGD_DECLARE(int)
gdAffineApplyToPointF(gdPointFPtr dst, const gdPointFPtr src, const double affine[6]);

/**
 * @brief Invert an affine matrix.
 *
 * @param dst - Receives the inverse matrix.
 * @param src - Source matrix.
 *
 * @returns GD_TRUE on success, or GD_FALSE if the matrix cannot be inverted.
 */
BGD_DECLARE(int) gdAffineInvert(double dst[6], const double src[6]);

/**
 * @brief Build a horizontal and/or vertical flip from an affine matrix.
 *
 * @param dst_affine - Receives the flipped matrix.
 * @param src_affine - Source matrix.
 * @param flip_h     - Non-zero to flip horizontally.
 * @param flip_v     - Non-zero to flip vertically.
 *
 * @returns GD_TRUE on success, or GD_FALSE on failure.
 */
BGD_DECLARE(int)
gdAffineFlip(double dst_affine[6], const double src_affine[6], const int flip_h, const int flip_v);

/**
 * @brief Concatenate two affine matrices.
 *
 * The result is equivalent to applying m1 and then m2. The destination may be
 * the same array as either input.
 *
 * @param dst - Receives the concatenated matrix.
 * @param m1  - First matrix.
 * @param m2  - Second matrix.
 *
 * @returns GD_TRUE on success, or GD_FALSE on failure.
 */
BGD_DECLARE(int)
gdAffineConcat(double dst[6], const double m1[6], const double m2[6]);

/**
 * @brief Store an identity affine matrix.
 *
 * @param dst - Receives the identity matrix.
 * @returns GD_TRUE on success, or GD_FALSE on failure.
 */
BGD_DECLARE(int) gdAffineIdentity(double dst[6]);

/**
 * @brief Store a scale affine matrix.
 *
 * @param dst     - Receives the scale matrix.
 * @param scale_x - Horizontal scale factor.
 * @param scale_y - Vertical scale factor.
 *
 * @returns GD_TRUE on success, or GD_FALSE on failure.
 */
BGD_DECLARE(int)
gdAffineScale(double dst[6], const double scale_x, const double scale_y);

/**
 * @brief Store a rotation affine matrix.
 *
 * In gd's image coordinate system, increasing y moves downward; positive
 * angles rotate counterclockwise in that system.
 *
 * @param dst   - Receives the rotation matrix.
 * @param angle - Rotation angle in degrees.
 *
 * @returns GD_TRUE on success, or GD_FALSE on failure.
 */
BGD_DECLARE(int) gdAffineRotate(double dst[6], const double angle);

/**
 * @brief Store a horizontal shear affine matrix.
 *
 * @param dst   - Receives the shear matrix.
 * @param angle - Shear angle in degrees.
 *
 * @returns GD_TRUE on success, or GD_FALSE on failure.
 */
BGD_DECLARE(int) gdAffineShearHorizontal(double dst[6], const double angle);

/**
 * @brief Store a vertical shear affine matrix.
 *
 * @param dst   - Receives the shear matrix.
 * @param angle - Shear angle in degrees.
 *
 * @returns GD_TRUE on success, or GD_FALSE on failure.
 */
BGD_DECLARE(int) gdAffineShearVertical(double dst[6], const double angle);

/**
 * @brief Store a translation affine matrix.
 *
 * @param dst      - Receives the translation matrix.
 * @param offset_x - Horizontal offset.
 * @param offset_y - Vertical offset.
 *
 * @returns GD_TRUE on success, or GD_FALSE on failure.
 */
BGD_DECLARE(int)
gdAffineTranslate(double dst[6], const double offset_x, const double offset_y);

/**
 * @brief Return the linear expansion factor of an affine matrix.
 *
 * This is the square root of the factor by which the matrix changes area.
 *
 * @param src - Source matrix.
 *
 * @returns The expansion factor.
 */
BGD_DECLARE(double) gdAffineExpansion(const double src[6]);

/**
 * @brief Test whether an affine matrix preserves axis-aligned rectangles.
 *
 * @param src - Source matrix.
 *
 * @returns GD_TRUE if the matrix is rectilinear, otherwise GD_FALSE.
 */
BGD_DECLARE(int) gdAffineRectilinear(const double src[6]);

/**
 * @brief Compare two affine matrices.
 *
 * @param matrix1 - First matrix.
 * @param matrix2 - Second matrix.
 *
 * @returns GD_TRUE if the matrices are equal within gd's affine tolerance, otherwise
 *          GD_FALSE.
 */
BGD_DECLARE(int)
gdAffineEqual(const double matrix1[6], const double matrix2[6]);

/**
 * @brief Apply an affine transform to a source region and create an image containing
 * the complete transformed result.
 *
 * The new image is truecolor with alpha saving enabled. Areas not covered by
 * the transformed source are transparent. The source image's current
 * @ref gdInterpolationMethod controls sampling. Palette sources may be converted
 * to truecolor internally.
 *
 * @param dst      - Receives the newly-created destination image.
 * @param src      - Source image.
 * @param src_area - Source rectangle, or NULL to transform the full image.
 * @param affine   - Matrix in gd's six-value affine form.
 *
 * @returns GD_TRUE on success, or GD_FALSE on failure. On failure, *dst is NULL.
 *
 * @see gdTransformAffineCopy gdTransformAffineBoundingBox gdImageSetInterpolationMethod
 */
BGD_DECLARE(int)
gdTransformAffineGetImage(gdImagePtr *dst, const gdImagePtr src, gdRectPtr src_area,
                          const double affine[6]);

/**
 * @brief Apply an affine transform to a source region and copy the transformed pixels
 * into an existing destination image.
 *
 * The source image's current @ref gdInterpolationMethod controls sampling.
 * Transparent samples are skipped. Destination bounds and alpha-blending
 * settings are honored.
 *
 * @param dst        - Destination image.
 * @param dst_x      - Destination x offset.
 * @param dst_y      - Destination y offset.
 * @param src        - Source image.
 * @param src_region - Source rectangle to transform.
 * @param affine     - Matrix in gd's six-value affine form.
 *
 * @returns GD_TRUE on success, or GD_FALSE on failure.
 *
 * @see gdTransformAffineGetImage  gdTransformAffineBoundingBox gdImageSetInterpolationMethod
 */
BGD_DECLARE(int)
gdTransformAffineCopy(gdImagePtr dst, int dst_x, int dst_y, const gdImagePtr src,
                      gdRectPtr src_region, const double affine[6]);
/*
gdTransformAffineCopy(gdImagePtr dst, int x0, int y0, int x1, int y1,
                          const gdImagePtr src, int src_width, int src_height,
                          const double affine[6]);
*/
/**
 * @brief Compute the bounding box of a source rectangle after applying an affine
 * transform.
 *
 * @param src    - Source rectangle.
 * @param affine - Matrix in gd's six-value affine form.
 * @param bbox   - Receives the transformed bounding box.
 *
 * @returns GD_TRUE on success, or GD_FALSE on failure.
 */
BGD_DECLARE(int)
gdTransformAffineBoundingBox(gdRectPtr src, const double affine[6], gdRectPtr bbox);

/** @} */

/* resolution affects ttf font rendering, particularly hinting */
#define GD_RESOLUTION 96 /* pixels per inch */

/* Version information functions */
BGD_DECLARE(int) gdMajorVersion(void);
BGD_DECLARE(int) gdMinorVersion(void);
BGD_DECLARE(int) gdReleaseVersion(void);
BGD_DECLARE(const char *) gdExtraVersion(void);
BGD_DECLARE(const char *) gdVersionString(void);

/* newfangled special effects */
#include "gdfx.h"

#ifdef __cplusplus
}
#endif

#endif /* GD_H */
