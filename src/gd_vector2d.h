#ifndef GD_VECTOR2D_H
#define GD_VECTOR2D_H

#include "gd.h"
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Experimental API: source and ABI compatibility are not yet guaranteed. */
#define GD_VECTOR2D_EXPERIMENTAL 1
#define GD_VECTOR2D_VERSION 1

/**
 * @defgroup Vector2D Vector 2D
 *
 * Experimental 2D vector drawing API for libgd.
 *
 * @{
 */

/**
 * @brief Opaque 2D drawing context.
 *
 * Opaque drawing context for the 2D API. A context owns the current path and a
 * stack of graphics state, and draws into an ARGB surface backed by a truecolor
 * @ref gdImage.
 *
 * See also:
 *  - @see gdContextCreateForImage
 *  - @see gdContextDestroy
 */
typedef struct gdContextStruct gdContext;

/**
 * @brief Opaque 2D drawing context pointer.
 *
 * Pointer to an opaque drawing context for the 2D API.
 *
 * See also:
 *  - @see gdContext gdContextCreateForImage gdContextDestroy
 */
typedef gdContext *gdContextPtr;

/**
 * @brief Opaque text shaping options.
 *
 * Opaque vector path made of contours, lines, curves, arcs, and rectangles.
 *
 * See also:
 *  - @see gdPathPtr gdPathCreate gdContextAppendPath
 */
typedef struct gdPathStruct gdPath;

/**
 * @brief Opaque vector path pointer.
 *
 * Pointer to an opaque vector path made of contours, lines, curves, arcs, and
 * rectangles.
 *
 * See also:
 *  - @see gdPath gdPathCreate gdContextAppendPath
 */
typedef gdPath *gdPathPtr;

/**
 * @brief Opaque paint source.
 *
 * Opaque paint source. A paint may be a solid color, image pattern, or gradient.
 *
 * See also:
 *  - @see gdContextSetSource gdPaintCreateFromPattern gdPaintCreateLinear
 */
typedef struct gdPaintStruct gdPaint;

/**
 * @brief Opaque paint source pointer.
 *
 * Pointer to an opaque paint source. A paint may be a solid color, image
 * pattern, or gradient.
 *
 * See also:
 *  - @see gdPaint gdContextSetSource gdPaintCreateFromPattern gdPaintCreateLinear
 */
typedef gdPaint *gdPaintPtr;

/**
 * @brief Opaque image pattern.
 *
 * Opaque image pattern used as a paint source.
 *
 * See also:
 *  - @see gdPathPatternPtr gdPathPatternCreateForImage gdPathPatternSetFilter
 */
typedef struct gdPathPatternStruct gdPathPattern;

/**
 * @brief Opaque image pattern pointer.
 *
 * Pointer to an opaque image pattern used as a paint source.
 *
 * See also:
 *  - @see gdPathPattern gdPathPatternCreateForImage gdPathPatternSetFilter
 */
typedef gdPathPattern *gdPathPatternPtr;

/**
 * @brief Opaque font face.
 *
 * Opaque font face used by text APIs.
 *
 * See also:
 *  - @see gdFontFacePtr gdFontFaceCreateFromFile gdContextSetFontFace
 */
typedef struct gdFontFaceStruct gdFontFace;

/**
 * @brief Opaque font face pointer.
 *
 * Pointer to an opaque font face used by text APIs.
 *
 * See also:
 *  - @see gdFontFace gdFontFaceCreateFromFile gdContextSetFontFace
 */
typedef gdFontFace *gdFontFacePtr;

/**
 * @brief Affine transformation matrix.
 *
 * Affine transformation matrix used by paths, contexts, paints, and patterns.
 *
 * Points are mapped as:
 * (x * m00 + y * m01 + m02, x * m10 + y * m11 + m12).
 *
 * See also:
 *  - @see gdPathMatrixPtr gdPathMatrixInit gdContextTransform
 */
typedef struct gdPathMatrixStruct {
    double m00, m10, m01, m11, m02, m12;
} gdPathMatrix;

/**
 * @brief Affine transformation matrix pointer.
 */
typedef gdPathMatrix *gdPathMatrixPtr;

/**
 * @brief Floating-point rectangle.
 *
 * Floating-point rectangle.
 *
 * Members:
 *   x - Left coordinate.
 *   y - Top coordinate.
 *   w - Width.
 *   h - Height.
  *  - @see gdRectFPtr
 */
typedef struct gdRectFStruct {
    double x, y, w, h;
} gdRectF;

/**
 * @brief Floating-point rectangle pointer.
 *
 * Pointer to a floating-point rectangle.
 *
 *  - @see gdRectF
 */
typedef gdRectF *gdRectFPtr;

/**
 * Constants: gdExtendMode
 *
 * Describes how paints are sampled outside their natural bounds.
 *
 * See also:
 *  - @see gdPaintSetExtend gdPathPatternSetExtend
 */
typedef enum {
    GD_EXTEND_NONE, /*< Outside samples are transparent. */
    GD_EXTEND_REPEAT, /*< Repeat the source. */
    GD_EXTEND_REFLECT, /*< Repeat the source with alternating mirrored copies. */
    GD_EXTEND_PAD /*< Clamp to the nearest edge sample. */
} gdExtendMode;

/**
 * Constants: gdLineCap
 *
 * Stroke endpoint style.
 *
  * See also:
 *  - @see gdContextSetLineCap
 */
typedef enum {
    gdLineCapButt, /**< End the stroke at the endpoint. */
    gdLineCapRound, /**< Add a round cap. */
    gdLineCapSquare /**< Add a square cap. */
} gdLineCap;

/**
 * Constants: gdLineJoin
 *
 * Stroke corner style.
 *
 * See also:
 *  - @see gdContextSetLineJoin
 */
typedef enum {
    gdLineJoinMiter, /**< Join segments with a miter when possible. */
    gdLineJoinRound, /**< Join segments with a round corner. */
    gdLineJoinBevel /**< Join segments with a bevel. */
} gdLineJoin;

/**
 * Constants: gdFillRule
 *
 * Rule used to decide which parts of a path are inside the fill.
 *
 * See also:
 *  - @see gdContextSetFillRule
 */
typedef enum {
    gdFillRuleNonZero, /**< Non-zero winding rule. */
    gdFillRuleEvenOdd /**< Even-odd rule. */
} gdFillRule;

/**
 * Constants: gdPatternFilter
 *
 * Selects the image sampling quality used when an image pattern is transformed
 * while filling or painting through the 2D API.
 *
 * The constants describe the requested quality level, not a fixed low-level
 * kernel. The exact sampler used for GOOD or BEST may change between releases
 * to improve output while keeping the public API stable.
 *
 *  @see gdContextSetPatternFilter gdPathPatternSetFilter
 */
typedef enum {
    GD_PATTERN_FILTER_FAST, /**< Fastest sampling. Uses nearest-neighbour sampling. */
    GD_PATTERN_FILTER_GOOD, /**< Balanced quality. Uses bilinear sampling and mipmapped sampling for minification. */
    GD_PATTERN_FILTER_BEST /**< Highest quality. Uses Mitchell bicubic sampling where appropriate and trilinear mipmapped sampling for minification. */
} gdPatternFilter;

/**
 * Constants: gdCompositeOperator
 *
 * Porter-Duff and blend operators used when painting source pixels over the
 * destination.
  * See also:
 *  - @see gdContextSetOperator
 */
typedef enum {
    GD_OP_CLEAR,  /**< Clear the destination. */
    GD_OP_SOURCE, /**< Replace with the source. */
    GD_OP_OVER, /**< Draw source over destination. */
    GD_OP_IN, /**< Keep source only where destination exists. */
    GD_OP_OUT, /**< Keep source only where destination is transparent. */
    GD_OP_ATOP, /**< Source atop destination. */
    GD_OP_DEST, /**< Keep destination unchanged. */
    GD_OP_DEST_OVER, /**< Draw destination over source. */
    GD_OP_DEST_IN, /**< Keep destination only where source exists. */
    GD_OP_DEST_OUT, /**< Keep destination only where source is transparent. */
    GD_OP_DEST_ATOP, /**< Destination atop source. */
    GD_OP_XOR, /**< Source and destination outside their overlap. */
    GD_OP_ADD, /**< Add source and destination. */
    GD_OP_SATURATE, /**< Saturating source-over operation. */
    GD_OP_MULTIPLY, /**< Multiply blend mode. */
    GD_OP_SCREEN, /**< Screen blend mode. */
    GD_OP_OVERLAY, /**< Overlay blend mode. */
    GD_OP_DARKEN, /**< Darken blend mode. */
    GD_OP_LIGHTEN, /**< Lighten blend mode. */
    GD_OP_COLOR_DODGE, /**< Color dodge blend mode. */
    GD_OP_COLOR_BURN, /**< Color burn blend mode. */
    GD_OP_HARD_LIGHT, /**< Hard light blend mode. */
    GD_OP_SOFT_LIGHT, /**< Soft light blend mode. */
    GD_OP_DIFFERENCE, /**< Difference blend mode. */
    GD_OP_EXCLUSION, /**< Exclusion blend mode. */
    GD_OP_HSL_HUE, /**< HSL hue blend mode. */
    GD_OP_HSL_SATURATION, /**< HSL saturation blend mode. */
    GD_OP_HSL_COLOR, /**< HSL color blend mode. */
    GD_OP_HSL_LUMINOSITY, /**< HSL luminosity blend mode. */
    GD_OP_COUNT
} gdCompositeOperator;
typedef gdCompositeOperator gdImageOp;
#define gdImageOpsSrc GD_OP_SOURCE
#define gdImageOpsSrcOver GD_OP_OVER
#define gdImageOpsDstIn GD_OP_DEST_IN
#define gdImageOpsDstOut GD_OP_DEST_OUT

/**
 * @brief Composite a source image into a truecolor destination image.
 *
 * @param dst         Destination image.
 * @param src         Source image.
 * @param dst_x       Destination x offset.
 * @param dst_y       Destination y offset.
 * @param op          Composite operator.
 * @param opacity     Global source opacity, from 0.0 to 1.0.
 * @param src_region  Optional source rectangle.
 * @param clip        Optional destination clip rectangle.
 * @returns GD_TRUE on success, or GD_FALSE for invalid arguments.
 */
BGD_DECLARE(int)
gdImageComposite(gdImagePtr dst, const gdImagePtr src, int dst_x, int dst_y,
                 gdCompositeOperator op, double opacity, gdRectPtr src_region,
                 gdRectPtr clip);

/**
 * Constants: gdTextStatus
 *
 * Status values returned by 2D text functions.
 *
 * @see gdTextError
 */
typedef enum {
    GD_TEXT_OK = 0, /**< The operation succeeded. */
    GD_TEXT_E_INVALID_ARGUMENT, /**< An argument was invalid. */
    GD_TEXT_E_UNAVAILABLE, /**< Required text support is unavailable. */
    GD_TEXT_E_FONT, /**< A font provider error occurred. */
    GD_TEXT_E_LAYOUT, /**< Text layout or shaping failed. */
    GD_TEXT_E_MEMORY /**< Memory allocation failed. */
} gdTextStatus;

#define GD_TEXT_ERROR_MESSAGE_SIZE 128

/**
 * @brief Optional detailed error information for 2D text APIs.
 */
typedef struct {
    gdTextStatus code; /**< A @see gdTextStatus value. */
    int provider_code; /**< Provider-specific error code, such as a FreeType error. */
    char message[GD_TEXT_ERROR_MESSAGE_SIZE]; /**< Human-readable error message. */
} gdTextError;

/**
 * struct: gdTextExtents
 *
 * Text metrics returned by @ref gdContextTextExtents.
 */
typedef struct {
    double x_bearing, y_bearing; /**< Horizontal and vertical bearing of the text bounds. */
    double width, height; /**< Bounds width and height. */
    double x_advance, y_advance; /**< Horizontal and vertical advance. */
} gdTextExtents;

/**
 * Constants: gdTextShaping
 *
 * Text shaping mode.
 *
 * See also:
 *  - @see gdTextOptions
 */
typedef enum {
    GD_TEXT_SHAPING_NONE = 0, /**< Basic glyph loading without complex shaping. */
    GD_TEXT_SHAPING_RAQM = 1 /**< Use libraqm shaping when available. */
} gdTextShaping;

/**
 * struct: gdTextOptions
 *
 * Options for text path, drawing, and measurement functions.
 *
 * Initialize this structure with @ref gdTextOptionsInit before use.
 */
typedef struct {
    gdTextShaping shaping; /**< Text shaping mode. */
    double line_spacing; /**< Line spacing multiplier. */
    unsigned int reserved_flags; /**< Reserved for future use. Initialize to zero. */
    double reserved_double; /**< Reserved for future use. Initialize to zero. */
} gdTextOptions;

/**
 * @brief Initialize text options to defaults.
 *
 * Defaults are basic shaping, line spacing of 1.0, and zeroed reserved fields.
 *
 * @param options The options structure to initialize.
 */
BGD_DECLARE(void) gdTextOptionsInit(gdTextOptions *options);

/**
 * @brief Create a 2D drawing context for a truecolor image.
 *
 * The context keeps an internal premultiplied ARGB surface. Existing image
 * pixels are loaded into the context when it is created. Use
 * @ref gdContextFlushImage or @ref gdContextDestroy to write drawing results back to
 * the image.
 *
 * @param image A truecolor image.
 *
 * @returns @ref gdContextPtr a new drawing context , or NULL if image is NULL, not truecolor, or
 *   allocation fails.
 *
 * See also:
 *  - @see gdContextFlushImage gdContextDestroy gdContextDestroyNoFlush
 */
BGD_DECLARE(gdContextPtr) gdContextCreateForImage(gdImagePtr image);

/**
 * @brief Copy the context's drawing surface back into its image.
 *
 * @param context The drawing context.
 *
 * @see gdContextCreateForImage gdContextReloadImage
 */
BGD_DECLARE(void) gdContextFlushImage(gdContextPtr context);

/**
 * @brief Reload the context drawing surface from its backing image.
 *
 * This discards unflushed drawing changes in the context.
 *
 * @param context The drawing context.
 *
 * @returns Non-zero on success, zero on failure.
 *
 * @see gdContextFlushImage
 */
BGD_DECLARE(int) gdContextReloadImage(gdContextPtr context);

/**
 * @brief Get the image associated with a context.
 *
 * @param context The drawing context.
 *
 * @returns The backing image, or NULL if context is NULL or has no backing image.
 */
BGD_DECLARE(gdImagePtr) gdContextGetImage(gdContextPtr context);

/**
 * @brief Destroy a context and flush drawing changes to the backing image.
 *
 * Passing NULL has no effect.
 *
 * @param context The context to destroy.
 *
 * @see gdContextDestroyNoFlush
 */
BGD_DECLARE(void) gdContextDestroy(gdContextPtr context);

/**
 * @brief Destroy a context without flushing drawing changes to the backing image.
 *
 * Passing NULL has no effect.
 *
 * @param context The context to destroy.
 *
 * @see gdContextDestroy
 */
BGD_DECLARE(void) gdContextDestroyNoFlush(gdContextPtr context);

/**
 * @brief Save the current graphics state.
 *
 * The saved state includes source, operator, opacity, transform, clip, stroke
 * settings, fill rule, font settings, and pattern filter.
 *
 * @param context The drawing context.
 *
 * @returns Non-zero on success, zero on failure.
 *
 * @see gdContextRestore
 */
BGD_DECLARE(int) gdContextSave(gdContextPtr context);

/**
 * @brief Restore the most recently saved graphics state.
 *
 * @param context The drawing context.
 *
 * @returns Non-zero on success, zero if there is no saved state or context is NULL.
 *
 * @see gdContextSave
 */
BGD_DECLARE(int) gdContextRestore(gdContextPtr context);

/**
 * @brief Intersect the current clip with the current path and clear the path.
 *
 * @returns Non-zero on success, zero on allocation failure.
 *
 * See also:
 *  - @see gdContextClipPreserve
 */
BGD_DECLARE(int) gdContextClip(gdContextPtr context);

/**
 * @brief Intersect the current clip with the current path without clearing the path.
 *
 * @returns Non-zero on success, zero on allocation failure.
 *
 * @see gdContextClip
 */
BGD_DECLARE(int) gdContextClipPreserve(gdContextPtr context);

/**
 * @brief Clear the current path.
 */
BGD_DECLARE(void) gdContextNewPath(gdContextPtr context);

/**
 * @brief Append a path to the context's current path.
 *
 * @param context The drawing context.
 * @param path    The path to append.
 */
BGD_DECLARE(void) gdContextAppendPath(gdContextPtr context, gdPathPtr path);

/**
 * @brief Set the current source paint to a solid RGBA color.
 *
 * Color components use the range 0.0 to 1.0.
 *
 * @param context The drawing context.
 * @param r       Red component.
 * @param g       Green component.
 * @param b       Blue component.
 * @param a       Alpha component.
 */
BGD_DECLARE(void)
gdContextSetSourceRgba(gdContextPtr context, double r, double g, double b, double a);

/**
 * @brief Set the current source paint to an opaque RGB color.
 *
 * Color components use the range 0.0 to 1.0.
 *
 * @param context The drawing context.
 * @param r       Red component.
 * @param g       Green component.
 * @param b       Blue component.
 */
BGD_DECLARE(void) gdContextSetSourceRgb(gdContextPtr context, double r, double g, double b);

/**
 * @brief Set the current source paint to an image pattern.
 *
 * The image is snapshotted when this function is called, so later changes to
 * the source image do not affect the pattern. The source image is positioned by
 * translating the pattern by (x, y). The pattern inherits the context's current
 * pattern filter; use @ref gdContextSetPatternFilter before this call to choose
 * FAST, GOOD, or BEST sampling for transformed image fills.
 *
 * @param context The drawing context.
 * @param image   The image to use as the source pattern.
 * @param x       The pattern x offset in user space.
 * @param y       The pattern y offset in user space.
 *
 * See also:
 *   - @see gdContextSetPatternFilter
 *   - @see gdPathPatternCreateForImage
 *   - @see gdPathPatternSetFilter
 */
BGD_DECLARE(void)
gdContextSetSourceImage(gdContextPtr context, gdImagePtr image, double x, double y);

/**
 * @brief Set the current source paint.
 *
 * The context keeps its own reference to the paint. The caller may destroy its
 * reference after this call.
 *
 * @param context The drawing context.
 * @param source  The paint to use for subsequent drawing.
 */
BGD_DECLARE(void) gdContextSetSource(gdContextPtr context, gdPaintPtr source);

/**
 * @brief Set the compositing operator used for subsequent drawing.
 *
 * @param context The drawing context.
 * @param op      The compositing operator.
 *
 * @see gdCompositeOperator
 */
BGD_DECLARE(void) gdContextSetOperator(gdContextPtr context, gdCompositeOperator op);

/**
 * @brief Set a global opacity multiplier for subsequent drawing.
 *
 * @param context The drawing context.
 * @param opacity Opacity value. Values are clamped to the range 0.0 to 1.0.
 */
BGD_DECLARE(void) gdContextSetOpacity(gdContextPtr context, double opacity);

/**
 * @brief Set the default image-pattern filter for patterns created by context source
 * helpers such as @ref gdContextSetSourceImage.
 *
 * This value is part of the graphics state and is saved and restored by
 * @ref gdContextSave and @ref gdContextRestore. It does not modify patterns that have
 * already been created; use @ref gdPathPatternSetFilter for explicit pattern
 * objects.
 *
 * @param context The drawing context.
 * @param filter  The requested pattern sampling quality.
 *
 * @see gdPatternFilter gdContextGetPatternFilter gdPathPatternSetFilter
 */
BGD_DECLARE(void) gdContextSetPatternFilter(gdContextPtr context, gdPatternFilter filter);

/**
 * @brief Get the current default image-pattern filter from a context.
 *
 * @param context The drawing context.
 *
 * @returns The current pattern filter. If context is NULL, @ref GD_PATTERN_FILTER_GOOD is returned.
 *
 * @see gdContextSetPatternFilter
 */
BGD_DECLARE(gdPatternFilter) gdContextGetPatternFilter(gdContextPtr context);

/**
 * @brief Set the current font face for text operations.
 *
 * The context keeps its own reference to the font face. Passing NULL clears the
 * current font face.
 *
 * @param context The drawing context.
 * @param face    The font face to use, or NULL.
 *
 * @see gdFontFaceCreateFromFile gdContextShowText
 */
BGD_DECLARE(void) gdContextSetFontFace(gdContextPtr context, gdFontFacePtr face);

/**
 * @brief Set the current font size for text operations.
 *
 * @param context The drawing context.
 * @param size    Font size in user-space units. Non-positive or non-finite values
 *                are ignored.
 */
BGD_DECLARE(void) gdContextSetFontSize(gdContextPtr context, double size);

/**
 * @brief Convert UTF-8 text to path contours and append them to the current path.
 *
 * Text is positioned with its origin at (x, y). The current font face and font
 * size are taken from the context.
 *
 * @param context The drawing context.
 * @param utf8    UTF-8 text.
 * @param x       Text origin x coordinate.
 * @param y       Text origin y coordinate.
 * @param options Optional text options, or NULL for defaults.
 * @param err     Optional error output.
 *
 * @returns @ref GD_TEXT_OK on success, or another @ref gdTextStatus value on failure.
 *
 * @see gdContextShowText gdContextTextExtents
 */
BGD_DECLARE(gdTextStatus)
gdContextTextPath(gdContextPtr context, const char *utf8, double x, double y,
                  const gdTextOptions *options, gdTextError *err);

/**
 * @brief Draw UTF-8 text using the current source paint.
 *
 * This appends the text path, fills it, and clears the current path as part of
 * @ref gdContextFill.
 *
 * @param context The drawing context.
 * @param utf8    UTF-8 text.
 * @param x       Text origin x coordinate.
 * @param y       Text origin y coordinate.
 * @param options Optional text options, or NULL for defaults.
 * @param err     Optional error output.
 *
 * @returns @ref GD_TEXT_OK on success, or another @ref gdTextStatus value on failure.
 */
BGD_DECLARE(gdTextStatus)
gdContextShowText(gdContextPtr context, const char *utf8, double x, double y,
                  const gdTextOptions *options, gdTextError *err);

/**
 * @brief Measure UTF-8 text using the current font face and font size.
 *
 * @param context The drawing context.
 * @param utf8    UTF-8 text.
 * @param options Optional text options, or NULL for defaults.
 * @param extents Where to store text metrics.
 * @param err     Optional error output.
 *
 * @returns @ref GD_TEXT_OK on success, or another @ref gdTextStatus value on failure.
 */
BGD_DECLARE(gdTextStatus)
gdContextTextExtents(gdContextPtr context, const char *utf8, const gdTextOptions *options,
                     gdTextExtents *extents, gdTextError *err);

/**
 * @brief Move the current point in the context path.
 */
BGD_DECLARE(void) gdContextMoveTo(gdContextPtr context, double x, double y);

/**
 * @brief Move the current point by an offset in the context path.
 */
BGD_DECLARE(void) gdContextRelMoveTo(gdContextPtr context, double dx, double dy);

/**
 * @brief Add a line to the context path.
 */
BGD_DECLARE(void) gdContextLineTo(gdContextPtr context, double x, double y);

/**
 * @brief Add a relative line to the context path.
 */
BGD_DECLARE(void) gdContextRelLineTo(gdContextPtr context, double dx, double dy);

/**
 * @brief Add a cubic Bezier curve to the context path.
 */
BGD_DECLARE(void)
gdContextCurveTo(gdContextPtr context, double x1, double y1, double x2, double y2, double x3,
                 double y3);

/**
 * @brief Add a relative cubic Bezier curve to the context path.
 */
BGD_DECLARE(void)
gdContextRelCurveTo(gdContextPtr context, double dx1, double dy1, double dx2, double dy2,
                    double dx3, double dy3);

/**
 * @brief Add a quadratic Bezier curve to the context path.
 */
BGD_DECLARE(void) gdContextQuadTo(gdContextPtr context, double x1, double y1, double x2, double y2);

/**
 * @brief Add a relative quadratic Bezier curve to the context path.
 */
BGD_DECLARE(void)
gdContextRelQuadTo(gdContextPtr context, double dx1, double dy1, double dx2, double dy2);

/**
 * @brief Add a positive-direction circular arc to the context path.
 */
BGD_DECLARE(void)
gdContextArc(gdContextPtr context, double cx, double cy, double radius, double a0, double a1);

/**
 * @brief Add a negative-direction circular arc to the context path.
 */
BGD_DECLARE(void)
gdContextNegativeArc(gdContextPtr context, double cx, double cy, double radius, double a0,
                     double a1);

/**
 * @brief Add a rectangle to the context path.
 */
BGD_DECLARE(void)
gdContextRectangle(gdContextPtr context, double x, double y, double width, double height);

/**
 * @brief Close the current contour in the context path.
 */
BGD_DECLARE(void) gdContextClosePath(gdContextPtr context);

/**
 * @brief Apply scaling to the current transformation matrix.
 */
BGD_DECLARE(void) gdContextScale(gdContextPtr context, double x, double y);

/**
 * @brief Apply translation to the current transformation matrix.
 */
BGD_DECLARE(void) gdContextTranslate(gdContextPtr context, double x, double y);

/**
 * @brief Apply rotation to the current transformation matrix.
 *
 * @param context The drawing context.
 * @param radians Rotation angle in radians.
 */
BGD_DECLARE(void) gdContextRotate(gdContextPtr context, double radians);

/**
 * @brief Apply an affine transformation to the current transformation matrix.
 *
 * @param context The drawing context.
 * @param matrix  The transform to apply.
 */
BGD_DECLARE(void) gdContextTransform(gdContextPtr context, const gdPathMatrixPtr matrix);

/**
 * @brief Set the stroke line width.
 */
BGD_DECLARE(void) gdContextSetLineWidth(gdContextPtr context, double width);

/**
 * @brief Set the stroke line cap style.
 */
BGD_DECLARE(void) gdContextSetLineCap(gdContextPtr context, gdLineCap cap);

/**
 * @brief Set the stroke line join style.
 */
BGD_DECLARE(void) gdContextSetLineJoin(gdContextPtr context, gdLineJoin join);

/**
 * @brief Set the stroke dash pattern.
 *
 * @param context The drawing context.
 * @param offset  Dash phase offset.
 * @param data    Dash segment lengths.
 * @param size    Number of dash segment lengths.
 */
BGD_DECLARE(void)
gdContextSetDash(gdContextPtr context, double offset, const double *data, int size);

/**
 * @brief Set the fill rule used by subsequent fill and clip operations.
 */
BGD_DECLARE(void) gdContextSetFillRule(gdContextPtr context, gdFillRule rule);

/**
 * @brief Stroke the current path and then clear it.
 *
 * @see gdContextStrokePreserve
 */
BGD_DECLARE(void) gdContextStroke(gdContextPtr context);

/**
 * @brief Stroke the current path without clearing it.
  */
BGD_DECLARE(void) gdContextStrokePreserve(gdContextPtr context);

/**
 * @brief Fill the current path and then clear it.
 *
 * @see gdContextFillPreserve
 */
BGD_DECLARE(void) gdContextFill(gdContextPtr context);

/**
 * @brief Fill the current path without clearing it.
 */
BGD_DECLARE(void) gdContextFillPreserve(gdContextPtr context);

/**
 * @brief Paint the current source over the entire current clip.
 */
BGD_DECLARE(void) gdContextPaint(gdContextPtr context);

/**
 * @brief Load a font face from a file.
 *
 * @param path       Font file path.
 * @param face_index Face index within the font file.
 * @param err        Optional error output.
 *
 * @returns A new font face, or NULL on failure.
 */
BGD_DECLARE(gdFontFacePtr)
gdFontFaceCreateFromFile(const char *path, int face_index, gdTextError *err);

/**
 * @brief Load a font face from memory.
 *
 * The caller must keep the data buffer alive for the lifetime of the font face.
 *
 * @param data       Font data.
 * @param size       Font data size in bytes.
 * @param face_index Face index within the font data.
 * @param err        Optional error output.
 *
 * @returns A new font face, or NULL on failure.
 */
BGD_DECLARE(gdFontFacePtr)
gdFontFaceCreateFromData(const unsigned char *data, size_t size, int face_index, gdTextError *err);

/**
 * @brief Add a reference to a font face.
 *
 * @param face The font face to add a reference to.
 *
 * @returns The same font face, or NULL if face is NULL.
 */
BGD_DECLARE(gdFontFacePtr) gdFontFaceAddRef(gdFontFacePtr face);

/**
 * @brief Release a font face reference.  Passing NULL has no effect.
 */
BGD_DECLARE(void) gdFontFaceDestroy(gdFontFacePtr face);

/**
 * @brief Create a paint object from an image pattern.
 *
 * Create a paint object from an image pattern.
 *
 * The paint holds a reference to the pattern. The caller may destroy its own
 * pattern reference after creating the paint.
 *
 * @param pattern The pattern to use as a paint source.
 *
 * @returns A new paint object, or NULL on allocation failure.
 *
 * @see gdPathPatternCreateForImage gdPathPatternSetFilter gdContextSetSource
 */
BGD_DECLARE(gdPaintPtr) gdPaintCreateFromPattern(gdPathPatternPtr pattern);

/**
 * @brief Create a linear gradient paint.
 *
 * The gradient parameter runs from (x0, y0) to (x1, y1). Add color stops with
 * @ref gdPaintAddColorStopRgb or @ref gdPaintAddColorStopRgba.
 *
 * @param x0 Start point x coordinate.
 * @param y0 Start point y coordinate.
 * @param x1 End point x coordinate.
 * @param y1 End point y coordinate.
 *
 * @returns A new paint object, or NULL on invalid input or allocation failure.
 */
BGD_DECLARE(gdPaintPtr) gdPaintCreateLinear(double x0, double y0, double x1, double y1);

/**
 * @brief Create a radial gradient paint between two circles.
 *
 * Create a radial gradient paint between two circles.
 *
 * Add color stops with @ref gdPaintAddColorStopRgb or
 * @ref gdPaintAddColorStopRgba.
 *
 * @param x0 First circle center x coordinate.
 * @param y0 First circle center y coordinate.
 * @param r0 First circle radius. Must be non-negative.
 * @param x1 Second circle center x coordinate.
 * @param y1 Second circle center y coordinate.
 * @param r1 Second circle radius. Must be non-negative.
 *
 * @returns A new paint object, or NULL on invalid input or allocation failure.
 */
BGD_DECLARE(gdPaintPtr)
gdPaintCreateRadial(double x0, double y0, double r0, double x1, double y1, double r1);

/**
 * @brief Add an opaque color stop to a gradient paint.
 *
 * @param paint  - A linear or radial gradient paint.
 * @param offset - Stop offset in the range 0.0 to 1.0.
 * @param r      - Red component in the range 0.0 to 1.0.
 * @param g      - Green component in the range 0.0 to 1.0.
 * @param b      - Blue component in the range 0.0 to 1.0.
 *
 * @returns Non-zero on success, zero on invalid input or allocation failure.
 *
 * See also:
 *   - @see gdPaintAddColorStopRgba
 */
BGD_DECLARE(int)
gdPaintAddColorStopRgb(gdPaintPtr paint, double offset, double r, double g, double b);

/**
 * @brief Add a color stop with opacity to a gradient paint.
 *
 * Add a color stop with opacity to a gradient paint.
 *
 * Multiple stops may have the same offset. Stops are kept in insertion order
 * for equal offsets.
 *
 * @param paint  - A linear or radial gradient paint.
 * @param offset - Stop offset in the range 0.0 to 1.0.
 * @param r      - Red component in the range 0.0 to 1.0.
 * @param g      - Green component in the range 0.0 to 1.0.
 * @param b      - Blue component in the range 0.0 to 1.0.
 * @param a      - Alpha component in the range 0.0 to 1.0.
 *
 * @returns Non-zero on success, zero on invalid input or allocation failure.
 *
 * @see gdPaintAddColorStopRgb
 */
BGD_DECLARE(int)
gdPaintAddColorStopRgba(gdPaintPtr paint, double offset, double r, double g, double b, double a);

/**
 * @brief Set the extend mode for a gradient or pattern paint.
 *
 * @param paint  - The paint to update.
 * @param extend - The extend mode.
 *
 * @returns Non-zero on success, zero if the paint type or extend mode is invalid.
 *
 * See also:
 *   - @see gdExtendMode
 */
BGD_DECLARE(int) gdPaintSetExtend(gdPaintPtr paint, gdExtendMode extend);

/**
 * @brief Set the paint's pattern or gradient transform.
 *
 * The matrix must be finite and invertible.
 *
 * @param paint  - The paint to update.
 * @param matrix - The paint matrix.
 *
 * @returns Non-zero on success, zero on invalid input.
 */
BGD_DECLARE(int) gdPaintSetMatrix(gdPaintPtr paint, const gdPathMatrixPtr matrix);

/**
 * @brief Release a paint object. Passing NULL has no effect.
 */
BGD_DECLARE(void) gdPaintDestroy(gdPaintPtr paint);

/**
 * @brief Create an image pattern from a gd image.
 *
 * The image contents are copied into an internal surface when the pattern is
 * created. The new pattern uses @ref GD_EXTEND_NONE, identity matrix, full opacity,
 * and @ref GD_PATTERN_FILTER_GOOD by default.
 *
 * @param image - The image to snapshot into a pattern.
 *
 * @returns A new pattern, or NULL on failure.
 *
 * See also:
 *   - @see gdPathPatternDestroy gdPathPatternSetFilter gdPaintCreateFromPattern
 */
BGD_DECLARE(gdPathPatternPtr) gdPathPatternCreateForImage(gdImagePtr image);

/**
 * @brief Release a pattern object. Passing NULL has no effect.
 */
BGD_DECLARE(void) gdPathPatternDestroy(gdPathPatternPtr pattern);

/**
 * @brief Set how an image pattern is sampled outside its image bounds.
 *
 * Set how an image pattern is sampled outside its image bounds.
 *
 * @param pattern - The pattern to update.
 * @param extend  - The extend mode.
 *
 * @see gdExtendMode
 */
BGD_DECLARE(void) gdPathPatternSetExtend(gdPathPatternPtr pattern, gdExtendMode extend);

/**
 * @brief Set the pattern-to-user-space transform for an image pattern.
 *
 * Set the pattern-to-user-space transform for an image pattern.
 *
 * The matrix controls how the pattern image is positioned, scaled, rotated, or
 * sheared before it is used as a fill source.
 *
 * @param pattern - The pattern to update.
 * @param matrix  - The pattern matrix.
 *
 * @see gdPathMatrix gdContextSetSourceImage
 */
BGD_DECLARE(void) gdPathPatternSetMatrix(gdPathPatternPtr pattern, gdPathMatrixPtr matrix);

/**
 * @brief Set the opacity multiplier for an image pattern.
 *
 * Set the opacity multiplier for an image pattern.
 *
 * @param pattern - The pattern to update.
 * @param opacity - The opacity value. Values are clamped to the range 0.0 to 1.0.
 */
BGD_DECLARE(void) gdPathPatternSetOpacity(gdPathPatternPtr pattern, double opacity);

/**
 * @brief Set the sampling quality for an image pattern.
 *
 * Set the sampling quality for an image pattern.
 *
 * The filter is used when the pattern image is transformed during painting.
 * FAST requests nearest-neighbour sampling, GOOD requests balanced quality, and
 * BEST requests the highest practical software quality. The exact kernel used
 * for GOOD or BEST is intentionally an implementation detail.
 *
 * @param pattern - The pattern to update.
 * @param filter  - The requested pattern sampling quality.
 *
 * @see gdPatternFilter gdPathPatternGetFilter gdContextSetPatternFilter
 */
BGD_DECLARE(void) gdPathPatternSetFilter(gdPathPatternPtr pattern, gdPatternFilter filter);

/**
 * @brief Get the sampling quality for an image pattern.
 *
 * Get the sampling quality for an image pattern.
 *
 * @param pattern - The pattern to query.
 *
 * @returns The pattern filter. If pattern is NULL, @ref GD_PATTERN_FILTER_GOOD is returned.
 *
 * @see gdPathPatternSetFilter
 */
BGD_DECLARE(gdPatternFilter) gdPathPatternGetFilter(gdPathPatternPtr pattern);

/**
 * @brief Initialize an affine transformation matrix from its six coefficients.
 *
 * A point is mapped to (x * m00 + y * m01 + m02,
 * x * m10 + y * m11 + m12).
 *
 * @param matrix - The matrix to initialize.
 * @param m00    - The horizontal x coefficient.
 * @param m10    - The vertical x coefficient.
 * @param m01    - The horizontal y coefficient.
 * @param m11    - The vertical y coefficient.
 * @param m02    - The horizontal translation.
 * @param m12    - The vertical translation.
 *
 * @see gdPathMatrixInitIdentity gdPathMatrixMap
 */
BGD_DECLARE(void)
gdPathMatrixInit(gdPathMatrixPtr matrix, double m00, double m10, double m01, double m11, double m02,
                 double m12);
/**
 * @brief Initialize a matrix to the identity transformation.
 *
 * @param matrix - The matrix to initialize.
 */
BGD_DECLARE(void) gdPathMatrixInitIdentity(gdPathMatrixPtr matrix);

/**
 * @brief Initialize a translation matrix.
 *
 * @param matrix - The matrix to initialize.
 * @param x      - The horizontal translation.
 * @param y      - The vertical translation.
 */
BGD_DECLARE(void) gdPathMatrixInitTranslate(gdPathMatrixPtr matrix, double x, double y);

/**
 * @brief Initialize a scaling matrix.
 *
 * @param matrix - The matrix to initialize.
 * @param x      - The horizontal scale factor.
 * @param y      - The vertical scale factor.
 */
BGD_DECLARE(void) gdPathMatrixInitScale(gdPathMatrixPtr matrix, double x, double y);

/**
 * @brief Initialize a shear matrix. The shear factors are the tangents of the
 * supplied angles.
 *
 * @param matrix - The matrix to initialize.
 * @param x      - The horizontal shear angle, in radians.
 * @param y      - The vertical shear angle, in radians.
 */
BGD_DECLARE(void) gdPathMatrixInitShear(gdPathMatrixPtr matrix, double x, double y);

/**
 * @brief Initialize a rotation matrix about the origin.
 *
 * @param matrix  - The matrix to initialize.
 * @param radians - The rotation angle, in radians.
 */
BGD_DECLARE(void) gdPathMatrixInitRotate(gdPathMatrixPtr matrix, double radians);

/**
 * @brief Initialize a rotation matrix about a specified point. The point (x, y)
 * remains unchanged by the resulting transformation.
 *
 * @param matrix  - The matrix to initialize.
 * @param radians - The rotation angle, in radians.
 * @param x       - The horizontal coordinate of the rotation center.
 * @param y       - The vertical coordinate of the rotation center.
 */
BGD_DECLARE(void)
gdPathMatrixInitRotateTranslate(gdPathMatrixPtr matrix, double radians, double x, double y);

/**
 * @brief Apply a translation before the transformation already in a matrix.
 *
 * @param matrix - The matrix to modify.
 * @param x      - The horizontal translation.
 * @param y      - The vertical translation.
 */
BGD_DECLARE(void) gdPathMatrixTranslate(gdPathMatrixPtr matrix, double x, double y);

/**
 * @brief Apply scaling before the transformation already in a matrix.
 *
 * @param matrix - The matrix to modify.
 * @param x      - The horizontal scale factor.
 * @param y      - The vertical scale factor.
 */
BGD_DECLARE(void) gdPathMatrixScale(gdPathMatrixPtr matrix, double x, double y);

/**
 * @brief Apply a shear before the transformation already in a matrix.
 *
 * @param matrix - The matrix to modify.
 * @param x      - The horizontal shear angle, in radians.
 * @param y      - The vertical shear angle, in radians.
 */
BGD_DECLARE(void) gdPathMatrixShear(gdPathMatrixPtr matrix, double x, double y);

/**
 * @brief Apply a rotation about the origin before the transformation already in a
 * matrix.
 *
 * @param matrix  - The matrix to modify.
 * @param radians - The rotation angle, in radians.
 */
 BGD_DECLARE(void) gdPathMatrixRotate(gdPathMatrixPtr matrix, double radians);

/**
 * @brief Apply a rotation about a specified point before the transformation already
 * in a matrix.
 *
 * @param matrix  - The matrix to modify.
 * @param radians - The rotation angle, in radians.
 * @param x       - The horizontal coordinate of the rotation center.
 * @param y       - The vertical coordinate of the rotation center.
 */
BGD_DECLARE(void)
gdPathMatrixRotateTranslate(gdPathMatrixPtr matrix, double radians, double x, double y);

/**
 * @brief Compose two affine transformations. The result maps through a first and
 * then through b. matrix may alias a or b.
 *
 * @param matrix - The destination matrix.
 * @param a      - The transformation applied first.
 * @param b      - The transformation applied second.
 */
BGD_DECLARE(void)
gdPathMatrixMultiply(gdPathMatrixPtr matrix, const gdPathMatrixPtr a, const gdPathMatrixPtr b);

/**
 * @brief Invert an affine transformation in place. A singular matrix is left
 * unchanged.
 *
 * @param matrix - The matrix to invert.
 *
 * @return Non-zero on success, or zero if the matrix is singular.
 */
BGD_DECLARE(int) gdPathMatrixInvert(gdPathMatrixPtr matrix);

/**
 * @brief Transform a pair of coordinates.
 *
 * @param matrix   - The transformation matrix.
 * @param x        - The source horizontal coordinate.
 * @param y        - The source vertical coordinate.
 * @param result_x - Where to store the transformed horizontal coordinate.
 * @param result_y - Where to store the transformed vertical coordinate.
 */
BGD_DECLARE(void)
gdPathMatrixMap(const gdPathMatrixPtr matrix, double x, double y, double *result_x,
                double *result_y);

/**
 * @brief Transform a point. src and dst may point to the same object.
 *
 * Transform a point. src and dst may point to the same object.
 *
 * @param matrix - The transformation matrix.
 * @param src    - The source point.
 * @param dst    - Where to store the transformed point.
 */
BGD_DECLARE(void)
gdPathMatrixMapPoint(const gdPathMatrixPtr matrix, const gdPointFPtr src, gdPointFPtr dst);

/**
 * @brief Transform all four corners of a rectangle and calculate their axis-aligned
 * bounding box. src and dst may point to the same object.
 *
 * @param matrix - The transformation matrix.
 * @param src    - The source rectangle.
 * @param dst    - Where to store the transformed bounding rectangle.
 */
BGD_DECLARE(void)
gdPathMatrixMapRect(const gdPathMatrixPtr matrix, const gdRectFPtr src, gdRectFPtr dst);

/**
 * @brief Create an empty path.
 *
 * @return A new path, or NULL if allocation fails. Destroy it with @ref gdPathDestroy.
 */
 BGD_DECLARE(gdPathPtr) gdPathCreate(void);

/**
 * @brief Create an independent copy of a path.
 *
 * @param path - The path to copy. Must not be NULL.
 * @return A new path, or NULL if path is NULL or allocation fails. Destroy the copy
 *         with @ref gdPathDestroy.
 */
BGD_DECLARE(gdPathPtr) gdPathDuplicate(const gdPathPtr path);

/**
 * @brief Release a path. Passing NULL has no effect.
 *
 * @param path - The path to release.
 */
BGD_DECLARE(void) gdPathDestroy(gdPathPtr path);

/**
 * @brief Append all contours from one path to another.
 *
 * @param path   - The destination path.
 * @param source - The path to append.
 */
BGD_DECLARE(void) gdPathAppendPath(gdPathPtr path, const gdPathPtr source);

/**
 * @brief Transform every point in a path in place.
 *
 * @param path   - The path to transform.
 * @param matrix - The transformation matrix.
 */
BGD_DECLARE(void) gdPathTransform(gdPathPtr path, const gdPathMatrixPtr matrix);

/**
 * @brief Start a new contour at an absolute position.
 *
 * @param path - The path to modify.
 * @param x    - The horizontal coordinate.
 * @param y    - The vertical coordinate.
 */
BGD_DECLARE(void) gdPathMoveTo(gdPathPtr path, double x, double y);

/**
 * @brief Start a new contour at an offset from the current point. For an empty path,
 * the offset is relative to (0, 0).
 *
 * @param path - The path to modify.
 * @param dx   - The horizontal offset.
 * @param dy   - The vertical offset.
 */
BGD_DECLARE(void) gdPathRelMoveTo(gdPathPtr path, double dx, double dy);

/**
 * @brief Add a straight line to an absolute position.
 *
 * @param path - The path to modify.
 * @param x    - The endpoint's horizontal coordinate.
 * @param y    - The endpoint's vertical coordinate.
 */
BGD_DECLARE(void) gdPathLineTo(gdPathPtr path, double x, double y);

/**
 * @brief Add a straight line using an offset from the current point.
 *
 * @param path - The path to modify.
 * @param dx   - The horizontal offset.
 * @param dy   - The vertical offset.
 */
BGD_DECLARE(void) gdPathRelLineTo(gdPathPtr path, double dx, double dy);

/**
 * @brief Add a quadratic Bezier curve.
 *
 * @param path - The path to modify.
 * @param x1   - The control point's horizontal coordinate.
 * @param y1   - The control point's vertical coordinate.
 * @param x2   - The endpoint's horizontal coordinate.
 * @param y2   - The endpoint's vertical coordinate.
 */
BGD_DECLARE(void) gdPathQuadTo(gdPathPtr path, double x1, double y1, double x2, double y2);

/**
 * @brief Add a quadratic Bezier curve using offsets from the current point.
 *
 * Add a quadratic Bezier curve using offsets from the current point.
 *
 * @param path - The path to modify.
 * @param dx1  - The control point's horizontal offset.
 * @param dy1  - The control point's vertical offset.
 * @param dx2  - The endpoint's horizontal offset.
 * @param dy2  - The endpoint's vertical offset.
 */
BGD_DECLARE(void) gdPathRelQuadTo(gdPathPtr path, double dx1, double dy1, double dx2, double dy2);

/**
 * @brief Add a cubic Bezier curve.
 *
 * @param path - The path to modify.
 * @param x1   - The first control point's horizontal coordinate.
 * @param y1   - The first control point's vertical coordinate.
 * @param x2   - The second control point's horizontal coordinate.
 * @param y2   - The second control point's vertical coordinate.
 * @param x3   - The endpoint's horizontal coordinate.
 * @param y3   - The endpoint's vertical coordinate.
 */
BGD_DECLARE(void)
gdPathCurveTo(gdPathPtr path, double x1, double y1, double x2, double y2, double x3, double y3);

/**
 * @brief Add a cubic Bezier curve using offsets from the current point.
 *
 * @param path - The path to modify.
 * @param dx1  - The first control point's horizontal offset.
 * @param dy1  - The first control point's vertical offset.
 * @param dx2  - The second control point's horizontal offset.
 * @param dy2  - The second control point's vertical offset.
 * @param dx3  - The endpoint's horizontal offset.
 * @param dy3  - The endpoint's vertical offset.
 */
BGD_DECLARE(void)
gdPathRelCurveTo(gdPathPtr path, double dx1, double dy1, double dx2, double dy2, double dx3,
                 double dy3);

/**
 * @brief Add a circular arc in the positive-angle direction. Angles are in radians.
 * A line is added from the current point to the beginning of the arc when
 * necessary.
 *
 * @param path   - The path to modify.
 * @param cx     - The center's horizontal coordinate.
 * @param cy     - The center's vertical coordinate.
 * @param radius - The arc radius.
 * @param angle1 - The starting angle in radians.
 * @param angle2 - The ending angle in radians.
 */
BGD_DECLARE(void)
gdPathArc(gdPathPtr path, double cx, double cy, double radius, double angle1, double angle2);

/**
 * @brief Add a circular arc in the negative-angle direction. Angles are in radians.
 * A line is added from the current point to the beginning of the arc when
 * necessary.
 *
 * @param path   - The path to modify.
 * @param cx     - The center's horizontal coordinate.
 * @param cy     - The center's vertical coordinate.
 * @param radius - The arc radius.
 * @param angle1 - The starting angle in radians.
 * @param angle2 - The ending angle in radians.
 */
BGD_DECLARE(void)
gdPathNegativeArc(gdPathPtr path, double cx, double cy, double radius, double angle1,
                  double angle2);

/**
 * @brief Connect the current point to (x1, y1) and (x2, y2) with a circular arc
 * tangent to both line segments. Degenerate geometry or a non-positive radius
 * adds a line to (x1, y1).
 *
 * @param path   - The path to modify.
 * @param x1     - The corner's horizontal coordinate.
 * @param y1     - The corner's vertical coordinate.
 * @param x2     - The second tangent line's horizontal endpoint.
 * @param y2     - The second tangent line's vertical endpoint.
 * @param radius - The arc radius.
 */
BGD_DECLARE(void)
gdPathArcTo(gdPathPtr path, double x1, double y1, double x2, double y2, double radius);

/**
 * @brief Add a closed rectangular contour.
 *
 * @param path   - The path to modify.
 * @param x      - The rectangle's left coordinate.
 * @param y      - The rectangle's top coordinate.
 * @param width  - The rectangle width.
 * @param height - The rectangle height.
  */
BGD_DECLARE(void) gdPathRectangle(gdPathPtr path, double x, double y, double width, double height);

/**
 * @brief Close the current contour with a line to its starting point. An empty path
 * or an already closed contour is unchanged.
 *
 * @param path - The path to modify.
 */
BGD_DECLARE(void) gdPathClose(gdPathPtr path);

/** @} */

#ifdef __cplusplus
}
#endif
#endif
