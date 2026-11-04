/* Internal header for random common utility functions. */

#ifndef GD_INTERN_H
#define GD_INTERN_H

#include "gd.h"

#include <limits.h>
#include <string.h>

#ifndef MAXPATHLEN
#ifdef PATH_MAX
#define MAXPATHLEN PATH_MAX
#elif defined(MAX_PATH)
#define MAXPATHLEN MAX_PATH
#else
#if defined(__GNU__)
#define MAXPATHLEN 4096
#else
#define MAXPATHLEN 256 /* Should be safe for any weird systems that do not define it */
#endif
#endif
#endif

#ifdef HAVE_STDINT_H
#include <stdint.h>
#else
#if defined(HAVE_INTTYPES_H)
#include <inttypes.h>
#else
#include "msinttypes/inttypes.h"
#endif
#endif

#ifdef _MSC_VER
#define ssize_t SSIZE_T
#define MAXSIZE_T ((SIZE_T) ~((SIZE_T)0))
#define MAXSSIZE_T ((SSIZE_T)(MAXSIZE_T >> 1))
#define MINSSIZE_T ((SSIZE_T)~MAXSSIZE_T)
#define SSIZE_MAX MAXSSIZE_T
#endif
#if defined(_MSC_VER)
#define UNUSED_PARAM(x) x
#elif defined(__GNUC__) || defined(__clang__)
#define UNUSED_PARAM(x) x __attribute__((unused))
#else
#define UNUSED_PARAM(x) x
#endif
#ifndef MIN
#define MIN(a, b) ((a) < (b) ? (a) : (b))
#endif
#define MIN3(a, b, c) ((a) < (b) ? (MIN(a, c)) : (MIN(b, c)))
#ifndef MAX
#define MAX(a, b) ((a) < (b) ? (b) : (a))
#endif
#define MAX3(a, b, c) ((a) < (b) ? (MAX(b, c)) : (MAX(a, c)))
#define CLAMP(x, low, high) (((x) > (high)) ? (high) : (((x) < (low)) ? (low) : (x)))
/* 2.0.12: this now checks the clipping rectangle */
#define gdImageBoundsSafeMacro(im, x, y)                                                           \
    (!((((y) < (im)->cy1) || ((y) > (im)->cy2)) || (((x) < (im)->cx1) || ((x) > (im)->cx2))))

#ifdef _MSC_VER
#define gd_strcasecmp _stricmp
#else
#define gd_strcasecmp strcasecmp
#endif

typedef enum {
    HORIZONTAL,
    VERTICAL,
} gdAxis;

/* Convert a double to an unsigned char, rounding to the nearest
 * integer and clamping the result between 0 and max.  The absolute
 * value of clr must be less than the maximum value of an unsigned
 * short. */
static inline unsigned char uchar_clamp(double clr, unsigned char max)
{
    unsigned short result;

    // assert(fabs(clr) <= SHRT_MAX);

    /* Casting a negative float to an unsigned short is undefined.
     * However, casting a float to a signed truncates toward zero and
     * casting a negative signed value to an unsigned of the same size
     * results in a bit-identical value (assuming twos-complement
     * arithmetic).	 This is what we want: all legal negative values
     * for clr will be greater than 255. */

    /* Convert and clamp. */
    result = (unsigned short)(short)(clr + 0.5);
    if (result > max) {
        result = (clr < 0) ? 0 : max;
    } /* if */

    return result;
} /* uchar_clamp*/

/* Internal prototypes: */

/* gd_metadata.c */
static inline int gdMetadataGetExifTiff(const unsigned char *data, size_t size,
                                        const unsigned char **tiff, size_t *tiff_size)
{
    static const unsigned char exif_signature[] = {'E', 'x', 'i', 'f', '\0', '\0'};

    if (tiff == NULL || tiff_size == NULL || (data == NULL && size != 0)) {
        return GD_META_ERR_INVALID;
    }
    if (size >= sizeof(exif_signature) &&
        memcmp(data, exif_signature, sizeof(exif_signature)) == 0) {
        data += sizeof(exif_signature);
        size -= sizeof(exif_signature);
    }
    if (size < 8 || data == NULL ||
        !((data[0] == 'I' && data[1] == 'I' && data[2] == 42 && data[3] == 0) ||
          (data[0] == 'M' && data[1] == 'M' && data[2] == 0 && data[3] == 42))) {
        return GD_META_ERR_PARSE;
    }
    *tiff = data;
    *tiff_size = size;
    return GD_META_OK;
}

/* gd_jpeg.c */

/* gd_rotate.c */
gdImagePtr gdImageRotate90(gdImagePtr src, int ignoretransparent);
gdImagePtr gdImageRotate180(gdImagePtr src, int ignoretransparent);
gdImagePtr gdImageRotate270(gdImagePtr src, int ignoretransparent);

/**
 * Clip a copy operation to the destination image bounds.
 * Adjusts srcX/srcY to match any clipping applied to dstX/dstY.
 * Returns 0 if the operation is entirely outside dst (nothing to do),
 * 1 if there is work to do.
 */
static inline int gdImageClipCopy(gdImagePtr dst, int *dstX, int *dstY, int *srcX, int *srcY,
                                  int *w, int *h)
{
    int x1, y1, x2, y2;

    /* overflow-safe dst rect: [dstX, dstY] to [dstX+w, dstY+h] */
    x1 = *dstX;
    y1 = *dstY;

    /* check w/h are positive */
    if (*w <= 0 || *h <= 0) {
        return 0;
    }

    /* overflow check for dstX+w and dstY+h */
    if (*dstX > 0 && *w > INT_MAX - *dstX) {
        x2 = INT_MAX;
    } else {
        x2 = *dstX + *w;
    }
    if (*dstY > 0 && *h > INT_MAX - *dstY) {
        y2 = INT_MAX;
    } else {
        y2 = *dstY + *h;
    }

    /* entirely outside dst? */
    if (x1 >= gdImageSX(dst) || y1 >= gdImageSY(dst) || x2 <= 0 || y2 <= 0) {
        return 0;
    }

    /* clip left */
    if (x1 < 0) {
        *srcX -= x1; /* advance srcX by the same amount */
        *w += x1;    /* reduce width */
        *dstX = 0;
    }

    /* clip top */
    if (y1 < 0) {
        *srcY -= y1;
        *h += y1;
        *dstY = 0;
    }

    /* clip right */
    if (*dstX + *w > gdImageSX(dst)) {
        *w = gdImageSX(dst) - *dstX;
    }

    /* clip bottom */
    if (*dstY + *h > gdImageSY(dst)) {
        *h = gdImageSY(dst) - *dstY;
    }

    /* sanity: clipping may have reduced w/h to zero */
    if (*w <= 0 || *h <= 0) {
        return 0;
    }

    return 1;
}
static inline int gdImageClipCopyResized(gdImagePtr dst, int *dstX, int *dstY, int *dstW, int *dstH,
                                         int *srcX, int *srcY, int *srcW, int *srcH)
{
    int orig_dstW = *dstW;
    int orig_dstH = *dstH;

    if (*dstW <= 0 || *dstH <= 0 || *srcW == 0 || *srcH == 0) {
        return 0;
    }
    if (*dstX >= gdImageSX(dst) || *dstY >= gdImageSY(dst) || *dstX + *dstW <= 0 ||
        *dstY + *dstH <= 0) {
        return 0;
    }

    /* clip left — adjust srcX proportionally */
    if (*dstX < 0) {
        *srcX += (-*dstX) * *srcW / orig_dstW;
        *srcW -= (-*dstX) * *srcW / orig_dstW;
        *dstW += *dstX;
        *dstX = 0;
    }
    /* clip top */
    if (*dstY < 0) {
        *srcY += (-*dstY) * *srcH / orig_dstH;
        *srcH -= (-*dstY) * *srcH / orig_dstH;
        *dstH += *dstY;
        *dstY = 0;
    }
    /* clip right */
    if (*dstX + *dstW > gdImageSX(dst)) {
        int clip = *dstX + *dstW - gdImageSX(dst);
        *srcW -= clip * *srcW / orig_dstW;
        *dstW = gdImageSX(dst) - *dstX;
    }
    /* clip bottom */
    if (*dstY + *dstH > gdImageSY(dst)) {
        int clip = *dstY + *dstH - gdImageSY(dst);
        *srcH -= clip * *srcH / orig_dstH;
        *dstH = gdImageSY(dst) - *dstY;
    }

    if (*dstW <= 0 || *dstH <= 0)
        return 0;
    return 1;
}
#endif /* GD_INTERN_H */
