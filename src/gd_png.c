#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "gd.h"
#include "gd_errors.h"
#include "gd_intern.h"
#include <ctype.h>
#include <math.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* JCE: Arrange HAVE_LIBPNG so that it can be set in gd.h */
#ifdef HAVE_LIBPNG

#include "gdhelpers.h"
#include "png.h" /* includes zlib.h and setjmp.h */
#include "zlib.h"
#include <limits.h>

#define TRUE 1
#define FALSE 0

static const unsigned char gdPngSignature[8] = {137, 80, 78, 71, 13, 10, 26, 10};

BGD_DECLARE(const char *) gdPngGetVersionString(void) { return PNG_LIBPNG_VER_STRING; }

static unsigned int gdPngGetUint32(const unsigned char *data)
{
    return ((unsigned int)data[0] << 24) | ((unsigned int)data[1] << 16) |
           ((unsigned int)data[2] << 8) | (unsigned int)data[3];
}

static int gdPngChunkIs(const unsigned char *type, const char *name)
{
    return memcmp(type, name, 4) == 0;
}

static int gdPngIsRawProfile(const unsigned char *data, size_t size, const char *profile_type)
{
    static const char prefix[] = "Raw profile type ";
    size_t prefix_size = sizeof(prefix) - 1;
    size_t profile_type_size;

    if (data == NULL) {
        return FALSE;
    }

    profile_type_size = strlen(profile_type);
    return size > prefix_size + profile_type_size && memcmp(data, prefix, prefix_size) == 0 &&
           memcmp(data + prefix_size, profile_type, profile_type_size) == 0 &&
           data[prefix_size + profile_type_size] == 0;
}

static int gdPngDecodeRawProfile(const unsigned char *data, size_t size,
                                 const char *profile_type, unsigned char **decoded,
                                 size_t *decoded_size)
{
    const unsigned char *nul;
    const unsigned char *line;
    const unsigned char *end;
    unsigned char *inflated = NULL;
    unsigned char *result = NULL;
    size_t compressed_size;
    size_t capacity;
    size_t length = 0;
    size_t hex_count = 0;
    size_t i;
    uLongf inflated_size;
    int zstatus;

    if (decoded == NULL || decoded_size == NULL || data == NULL ||
        !gdPngIsRawProfile(data, size, profile_type)) {
        return GD_META_ERR_INVALID;
    }
    *decoded = NULL;
    *decoded_size = 0;

    nul = (const unsigned char *)memchr(data, 0, size);
    if (nul == NULL || (size_t)(nul - data) > size - 2 || data[nul - data + 1] != 0) {
        return GD_META_ERR_PARSE;
    }
    compressed_size = size - (size_t)(nul - data) - 2;
    if (compressed_size == 0) {
        return GD_META_ERR_PARSE;
    }

    capacity = compressed_size > (SIZE_MAX - 64) / 2 ? SIZE_MAX : compressed_size * 2 + 64;
    if (capacity == SIZE_MAX || capacity > ULONG_MAX) {
        capacity = ULONG_MAX;
    }
    for (;;) {
        unsigned char *new_inflated = (unsigned char *)gdRealloc(inflated, capacity);
        if (new_inflated == NULL) {
            gdFree(inflated);
            return GD_META_ERR_NOMEM;
        }
        inflated = new_inflated;
        inflated_size = (uLongf)capacity;
        zstatus = uncompress(inflated, &inflated_size, nul + 2, (uLong)compressed_size);
        if (zstatus == Z_OK) {
            break;
        }
        if (zstatus != Z_BUF_ERROR || capacity > SIZE_MAX / 2 || capacity >= ULONG_MAX) {
            gdFree(inflated);
            return GD_META_ERR_PARSE;
        }
        capacity *= 2;
    }

    line = (const unsigned char *)memchr(inflated, '\n', (size_t)inflated_size);
    if (line == NULL || line + 1 >= inflated + inflated_size) {
        gdFree(inflated);
        return GD_META_ERR_PARSE;
    }
    line++;
    end = (const unsigned char *)memchr(line, '\n', (size_t)(inflated + inflated_size - line));
    if (end == NULL) {
        gdFree(inflated);
        return GD_META_ERR_PARSE;
    }
    line = end + 1;
    end = (const unsigned char *)memchr(line, '\n', (size_t)(inflated + inflated_size - line));
    if (end == NULL) {
        gdFree(inflated);
        return GD_META_ERR_PARSE;
    }
    for (i = (size_t)(line - inflated); i < (size_t)(end - inflated); i++) {
        if (isspace(inflated[i])) {
            continue;
        }
        if (inflated[i] < '0' || inflated[i] > '9' ||
            length > (SIZE_MAX - (inflated[i] - '0')) / 10) {
            gdFree(inflated);
            return GD_META_ERR_PARSE;
        }
        length = length * 10 + (inflated[i] - '0');
    }
    if (length == 0 || length > SIZE_MAX / 2) {
        gdFree(inflated);
        return GD_META_ERR_LIMIT;
    }
    result = (unsigned char *)gdMalloc(length);
    if (result == NULL) {
        gdFree(inflated);
        return GD_META_ERR_NOMEM;
    }

    line = end + 1;
    for (i = 0; line + i < inflated + inflated_size; i++) {
        unsigned char c = line[i];
        if ((c >= '0' && c <= '9') || (c >= 'a' && c <= 'f') || (c >= 'A' && c <= 'F')) {
            hex_count++;
            if (hex_count > length * 2) {
                gdFree(result);
                gdFree(inflated);
                return GD_META_ERR_PARSE;
            }
            if ((hex_count & 1) != 0) {
                result[hex_count / 2] = (unsigned char)((c <= '9') ? c - '0' :
                    (c >= 'a' ? c - 'a' + 10 : c - 'A' + 10)) << 4;
            } else {
                result[hex_count / 2 - 1] |= (unsigned char)((c <= '9') ? c - '0' :
                    (c >= 'a' ? c - 'a' + 10 : c - 'A' + 10));
            }
        } else if (!isspace(c)) {
            gdFree(result);
            gdFree(inflated);
            return GD_META_ERR_PARSE;
        }
    }
    gdFree(inflated);
    if (hex_count != length * 2) {
        gdFree(result);
        return GD_META_ERR_PARSE;
    }
    *decoded = result;
    *decoded_size = length;
    return GD_META_OK;
}

static int gdPngSetTextProfile(gdImageMetadata *metadata, const unsigned char *data, size_t size)
{
    const unsigned char *nul;
    size_t keyword_size;
    char *key;
    int status;

    nul = (const unsigned char *)memchr(data, 0, size);
    if (nul == NULL || nul == data) {
        return GD_META_OK;
    }

    keyword_size = (size_t)(nul - data);
    key = (char *)gdMalloc(sizeof("png:text:") + keyword_size);
    if (key == NULL) {
        return GD_META_ERR_NOMEM;
    }
    memcpy(key, "png:text:", sizeof("png:text:") - 1);
    memcpy(key + sizeof("png:text:") - 1, data, keyword_size);
    key[sizeof("png:text:") - 1 + keyword_size] = '\0';

    status = gdImageMetadataSetProfile(metadata, key, data, size);
    gdFree(key);
    return status;
}

static int gdPngInfoValidBitDepth(int color_type, int bit_depth)
{
    switch (color_type) {
    case PNG_COLOR_TYPE_GRAY:
        return bit_depth == 1 || bit_depth == 2 || bit_depth == 4 || bit_depth == 8 ||
               bit_depth == 16;
    case PNG_COLOR_TYPE_RGB:
    case PNG_COLOR_TYPE_GRAY_ALPHA:
    case PNG_COLOR_TYPE_RGB_ALPHA:
        return bit_depth == 8 || bit_depth == 16;
    case PNG_COLOR_TYPE_PALETTE:
        return bit_depth == 1 || bit_depth == 2 || bit_depth == 4 || bit_depth == 8;
    default:
        return FALSE;
    }
}

static void *gdPngReadCtxToMemory(gdIOCtx *infile, int *size)
{
    enum { GD_PNG_ALLOC_STEP = 8192 };
    unsigned char *data = NULL;
    int logical_size = 0;
    int real_size = 0;

    if (size != NULL) {
        *size = 0;
    }
    if (infile == NULL || size == NULL) {
        return NULL;
    }

    for (;;) {
        int n;

        if (real_size - logical_size < GD_PNG_ALLOC_STEP) {
            unsigned char *temp;
            int new_size;

            if (real_size > INT_MAX - GD_PNG_ALLOC_STEP) {
                gdFree(data);
                return NULL;
            }
            new_size = real_size + GD_PNG_ALLOC_STEP;
            temp = (unsigned char *)gdRealloc(data, new_size);
            if (temp == NULL) {
                gdFree(data);
                return NULL;
            }
            data = temp;
            real_size = new_size;
        }

        n = gdGetBuf(data + logical_size, GD_PNG_ALLOC_STEP, infile);
        if (n <= 0) {
            break;
        }
        logical_size += n;
    }

    *size = logical_size;
    return data;
}

/*---------------------------------------------------------------------------

  gd_png.c                 Copyright 1999 Greg Roelofs and Thomas Boutell

  The routines in this file, gdImagePng*() and gdImageCreateFromPng*(),
  are drop-in replacements for gdImageGif*() and gdImageCreateFromGif*(),
  except that these functions are noisier in the case of errors (comment
  out all fprintf() statements to disable that).

  GD 2.0 supports RGBA truecolor and will read and write truecolor PNGs.
  GD 2.0 supports 8 bits of color resolution per channel and
  7 bits of alpha channel resolution. Images with more than 8 bits
  per channel are reduced to 8 bits. Images with an alpha channel are
  only able to resolve down to '1/128th opaque' instead of '1/256th',
  and this conversion is also automatic. I very much doubt you can see it.
  Both tRNS and true alpha are supported.

  Gamma is ignored, and there is no support for text annotations.

  Last updated:  9 February 2001

  ---------------------------------------------------------------------------*/

/**
 * File: PNG IO
 *
 * Read and write PNG images.
 */

#ifdef PNG_SETJMP_SUPPORTED
typedef struct _jmpbuf_wrapper {
    jmp_buf jmpbuf;
} jmpbuf_wrapper;

static void gdPngErrorHandler(png_structp png_ptr, png_const_charp msg)
{
    jmpbuf_wrapper *jmpbuf_ptr;

    /* This function, aside from the extra step of retrieving the "error
     * pointer" (below) and the fact that it exists within the application
     * rather than within libpng, is essentially identical to libpng's
     * default error handler.  The second point is critical:  since both
     * setjmp() and longjmp() are called from the same code, they are
     * guaranteed to have compatible notions of how big a jmp_buf is,
     * regardless of whether _BSD_SOURCE or anything else has (or has not)
     * been defined. */

    gd_error_ex(GD_WARNING, "gd-png: fatal libpng error: %s\n", msg);

    jmpbuf_ptr = png_get_error_ptr(png_ptr);
    if (jmpbuf_ptr == NULL) { /* we are completely hosed now */
        gd_error_ex(GD_ERROR, "gd-png: EXTREMELY fatal error: jmpbuf "
                              "unrecoverable; terminating.\n");
        exit(99);
    }

    longjmp(jmpbuf_ptr->jmpbuf, 1);
}

static void gdPngWarningHandler(UNUSED_PARAM(png_structp png_ptr), png_const_charp msg)
{
    gd_error_ex(GD_WARNING, "gd-png: libpng warning: %s", msg);
}
#endif

static void gdPngReadData(png_structp png_ptr, png_bytep data, png_size_t length)
{
    int check;
    check = gdGetBuf(data, length, (gdIOCtx *)png_get_io_ptr(png_ptr));
    if (check != (int)length) {
        png_error(png_ptr, "Read Error: truncated data");
    }
}

static void gdPngWriteData(png_structp png_ptr, png_bytep data, png_size_t length)
{
    gdPutBuf(data, length, (gdIOCtx *)png_get_io_ptr(png_ptr));
}

static void gdPngFlushData(png_structp png_ptr) { (void)png_ptr; }

BGD_DECLARE(gdImagePtr) gdImageCreateFromPng(FILE *inFile)
{
    gdImagePtr im;
    gdIOCtx *in = gdNewFileCtx(inFile);
    if (in == NULL)
        return NULL;
    im = gdImageCreateFromPngCtx(in);
    in->gd_free(in);
    return im;
}

BGD_DECLARE(gdImagePtr) gdImageCreateFromPngPtr(int size, void *data)
{
    gdImagePtr im;
    gdIOCtx *in = gdNewDynamicCtxEx(size, data, 0);
    if (!in)
        return 0;
    im = gdImageCreateFromPngCtx(in);
    in->gd_free(in);
    return im;
}

/* This routine is based in part on the Chapter 13 demo code in
 * "PNG: The Definitive Guide" (http://www.libpng.org/pub/png/book/).
 */
BGD_DECLARE(gdImagePtr) gdImageCreateFromPngCtx(gdIOCtx *infile)
{
    png_byte sig[8];
#ifdef PNG_SETJMP_SUPPORTED
    jmpbuf_wrapper jbw;
#endif
    png_structp png_ptr;
    png_infop info_ptr;
    png_uint_32 width, height, rowbytes, w, h, res_x, res_y;
    int bit_depth, color_type, interlace_type, unit_type;
    int num_palette = 0, num_trans;
    png_colorp palette;
    png_color_16p trans_gray_rgb;
    png_color_16p trans_color_rgb;
    png_bytep trans;
    png_bytep image_data = NULL;
    png_bytepp row_pointers = NULL;
    gdImagePtr im = NULL;
    int i, j, *open = NULL;
    volatile int transparent = -1;
    volatile int palette_allocated = FALSE;

    /* Make sure the signature can't match by dumb luck -- TBB */
    /* GRR: isn't sizeof(infile) equal to the size of the pointer? */
    memset(sig, 0, sizeof(sig));

    /* first do a quick check that the file really is a PNG image; could
     * have used slightly more general png_sig_cmp() function instead */
    if (gdGetBuf(sig, 8, infile) < 8) {
        return NULL;
    }

    if (png_sig_cmp(sig, 0, 8) != 0) { /* bad signature */
        return NULL;                   /* bad signature */
    }

#ifdef PNG_SETJMP_SUPPORTED
    png_ptr =
        png_create_read_struct(PNG_LIBPNG_VER_STRING, &jbw, gdPngErrorHandler, gdPngWarningHandler);
#else
    png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
#endif
    if (png_ptr == NULL) {
        gd_error("gd-png error: cannot allocate libpng main struct\n");
        return NULL;
    }

    info_ptr = png_create_info_struct(png_ptr);
    if (info_ptr == NULL) {
        gd_error("gd-png error: cannot allocate libpng info struct\n");
        png_destroy_read_struct(&png_ptr, NULL, NULL);

        return NULL;
    }

    /* we could create a second info struct here (end_info), but it's only
     * useful if we want to keep pre- and post-IDAT chunk info separated
     * (mainly for PNG-aware image editors and converters)
     */

    /* setjmp() must be called in every non-callback function that calls a
     * PNG-reading libpng function.  We must reset it every time we get a
     * new allocation that we save in a stack variable.
     */
#ifdef PNG_SETJMP_SUPPORTED
    if (setjmp(jbw.jmpbuf)) {
        gd_error("gd-png error: setjmp returns error condition 1\n");
        png_destroy_read_struct(&png_ptr, &info_ptr, NULL);

        return NULL;
    }
#endif

    png_set_sig_bytes(png_ptr, 8); /* we already read the 8 signature bytes */

    png_set_read_fn(png_ptr, (void *)infile, gdPngReadData);
    png_set_user_limits(png_ptr, 0x7fffffffL, 0x7fffffffL);
    png_read_info(png_ptr, info_ptr); /* read all PNG info up to image data */

    png_get_IHDR(png_ptr, info_ptr, &width, &height, &bit_depth, &color_type, &interlace_type, NULL,
                 NULL);
    if ((color_type == PNG_COLOR_TYPE_RGB) || (color_type == PNG_COLOR_TYPE_RGB_ALPHA) ||
        color_type == PNG_COLOR_TYPE_GRAY_ALPHA) {
        im = gdImageCreateTrueColor((int)width, (int)height);
    } else {
        im = gdImageCreate((int)width, (int)height);
    }
    if (im == NULL) {
        gd_error("gd-png error: cannot allocate gdImage struct\n");
        goto error;
    }

    if (bit_depth == 16) {
        png_set_strip_16(png_ptr);
    } else if (bit_depth < 8) {
        png_set_packing(png_ptr); /* expand to 1 byte per pixel */
    }

    /* setjmp() must be called in every non-callback function that calls a
     * PNG-reading libpng function.  We must reset it every time we get a
     * new allocation that we save in a stack variable.
     */
#ifdef PNG_SETJMP_SUPPORTED
    if (setjmp(jbw.jmpbuf)) {
        gd_error("gd-png error: setjmp returns error condition 2\n");
        goto error;
    }
#endif

#ifdef PNG_pHYs_SUPPORTED
    /* check if the resolution is specified */
    if (png_get_valid(png_ptr, info_ptr, PNG_INFO_pHYs)) {
        if (png_get_pHYs(png_ptr, info_ptr, &res_x, &res_y, &unit_type)) {
            switch (unit_type) {
            case PNG_RESOLUTION_METER:
                im->res_x = DPM2DPI(res_x);
                im->res_y = DPM2DPI(res_y);
                break;
            }
        }
    }
#endif

    switch (color_type) {
    case PNG_COLOR_TYPE_PALETTE:
        png_get_PLTE(png_ptr, info_ptr, &palette, &num_palette);
#ifdef DEBUG
        gd_error("gd-png color_type is palette, colors: %d\n", num_palette);
#endif /* DEBUG */
        if (png_get_valid(png_ptr, info_ptr, PNG_INFO_tRNS)) {
            /* gd 2.0: we support this rather thoroughly now. Grab the
             * first fully transparent entry, if any, as the value of
             * the simple-transparency index, mostly for backwards
             * binary compatibility. The alpha channel is where it's
             * really at these days.
             */
            int firstZero = 1;
            png_get_tRNS(png_ptr, info_ptr, &trans, &num_trans, NULL);
            for (i = 0; i < num_trans; ++i) {
                im->alpha[i] = gdAlphaMax - (trans[i] >> 1);
                if ((trans[i] == 0) && (firstZero)) {
                    /* 2.0.5: long-forgotten patch from Wez Furlong */
                    transparent = i;
                    firstZero = 0;
                }
            }
        }
        break;

    case PNG_COLOR_TYPE_GRAY:
        /* create a fake palette and check for single-shade transparency */
        if ((palette = (png_colorp)gdMalloc(256 * sizeof(png_color))) == NULL) {
            gd_error("gd-png error: cannot allocate gray palette\n");
            goto error;
        }
        palette_allocated = TRUE;
        if (bit_depth < 8) {
            num_palette = 1 << bit_depth;
            for (i = 0; i < 256; ++i) {
                j = (255 * i) / (num_palette - 1);
                palette[i].red = palette[i].green = palette[i].blue = j;
            }
        } else {
            num_palette = 256;
            for (i = 0; i < 256; ++i) {
                palette[i].red = palette[i].green = palette[i].blue = i;
            }
        }
        if (png_get_valid(png_ptr, info_ptr, PNG_INFO_tRNS)) {
            png_get_tRNS(png_ptr, info_ptr, NULL, NULL, &trans_gray_rgb);
            if (bit_depth == 16) { /* png_set_strip_16() not yet in effect */
                transparent = trans_gray_rgb->gray >> 8;
            } else {
                transparent = trans_gray_rgb->gray;
            }
            /* Note slight error in 16-bit case:  up to 256 16-bit shades
             * may get mapped to a single 8-bit shade, and only one of them
             * is supposed to be transparent.  IOW, both opaque pixels and
             * transparent pixels will be mapped into the transparent entry.
             * There is no particularly good way around this in the case
             * that all 256 8-bit shades are used, but one could write some
             * custom 16-bit code to handle the case where there are gdFree
             * palette entries.  This error will be extremely rare in
             * general, though.  (Quite possibly there is only one such
             * image in existence.) */
        }
        break;

    case PNG_COLOR_TYPE_GRAY_ALPHA:
        png_set_gray_to_rgb(png_ptr);
        // fall through
        // Keep above comment, gcc recognizes it and silent its warning about
        // fall through case here
    case PNG_COLOR_TYPE_RGB:
    case PNG_COLOR_TYPE_RGB_ALPHA:
        /* gd 2.0: we now support truecolor. See the comment above
           for a rare situation in which the transparent pixel may not
           work properly with 16-bit channels. */
        if (png_get_valid(png_ptr, info_ptr, PNG_INFO_tRNS)) {
            png_get_tRNS(png_ptr, info_ptr, NULL, NULL, &trans_color_rgb);
            if (bit_depth == 16) { /* png_set_strip_16() not yet in effect */
                transparent = gdTrueColor(trans_color_rgb->red >> 8, trans_color_rgb->green >> 8,
                                          trans_color_rgb->blue >> 8);
            } else {
                transparent = gdTrueColor(trans_color_rgb->red, trans_color_rgb->green,
                                          trans_color_rgb->blue);
            }
        }
        break;
    default:
        gd_error("gd-png color_type is unknown: %d\n", color_type);
        goto error;
    }

    /* enable the interlace transform if supported */
#ifdef PNG_READ_INTERLACING_SUPPORTED
    (void)png_set_interlace_handling(png_ptr);
#endif

    png_read_update_info(png_ptr, info_ptr);

    /* allocate space for the PNG image data */
    rowbytes = png_get_rowbytes(png_ptr, info_ptr);
    if (overflow2(rowbytes, height))
        goto error;
    image_data = (png_bytep)gdMalloc(rowbytes * height);
    if (!image_data) {
        gd_error("gd-png error: cannot allocate image data\n");
        goto error;
    }
    if (overflow2(height, sizeof(png_bytep)))
        goto error;

    row_pointers = (png_bytepp)gdMalloc(height * sizeof(png_bytep));
    if (!row_pointers) {
        gd_error("gd-png error: cannot allocate row pointers\n");
        goto error;
    }

    /* setjmp() must be called in every non-callback function that calls a
     * PNG-reading libpng function.  We must reset it every time we get a
     * new allocation that we save in a stack variable.
     */
#ifdef PNG_SETJMP_SUPPORTED
    if (setjmp(jbw.jmpbuf)) {
        gd_error("gd-png error: setjmp returns error condition 3\n");
        goto error;
    }
#endif

    /* set the individual row_pointers to point at the correct offsets */
    for (h = 0; h < height; ++h) {
        row_pointers[h] = image_data + h * rowbytes;
    }

    png_read_image(png_ptr, row_pointers); /* read whole image... */
    png_read_end(png_ptr, NULL);           /* ...done! */

    if (!im->trueColor) {
        im->colorsTotal = num_palette;
        /* load the palette and mark all entries "open" (unused) for now */
        open = im->open;
        for (i = 0; i < num_palette; ++i) {
            im->red[i] = palette[i].red;
            im->green[i] = palette[i].green;
            im->blue[i] = palette[i].blue;
            open[i] = 1;
        }
        for (i = num_palette; i < gdMaxColors; ++i) {
            open[i] = 1;
        }
    }
    /* 2.0.12: Slaven Rezic: palette images are not the only images
       with a simple transparent color setting */
    im->transparent = transparent;
    im->interlace = (interlace_type == PNG_INTERLACE_ADAM7);

    /* can't nuke structs until done with palette */
    png_destroy_read_struct(&png_ptr, &info_ptr, NULL);
    switch (color_type) {
    case PNG_COLOR_TYPE_RGB:
        for (h = 0; h < height; h++) {
            int boffset = 0;
            for (w = 0; w < width; w++) {
                register png_byte r = row_pointers[h][boffset++];
                register png_byte g = row_pointers[h][boffset++];
                register png_byte b = row_pointers[h][boffset++];
                im->tpixels[h][w] = gdTrueColor(r, g, b);
            }
        }
        break;

    case PNG_COLOR_TYPE_GRAY_ALPHA:
    case PNG_COLOR_TYPE_RGB_ALPHA:
        for (h = 0; h < height; h++) {
            int boffset = 0;
            for (w = 0; w < width; w++) {
                register png_byte r = row_pointers[h][boffset++];
                register png_byte g = row_pointers[h][boffset++];
                register png_byte b = row_pointers[h][boffset++];

                /* gd has only 7 bits of alpha channel resolution, and
                 * 127 is transparent, 0 opaque. A moment of convenience,
                 *  a lifetime of compatibility.
                 */

                register png_byte a = gdAlphaMax - (row_pointers[h][boffset++] >> 1);
                im->tpixels[h][w] = gdTrueColorAlpha(r, g, b, a);
            }
        }
        break;
    default:
        if (!im->trueColor) {
            /* Palette image, or something coerced to be one */
            for (h = 0; h < height; ++h) {
                for (w = 0; w < width; ++w) {
                    register png_byte idx = row_pointers[h][w];
                    im->pixels[h][w] = idx;
                    open[idx] = 0;
                }
            }
        }
    }
#ifdef DEBUG
    if (!im->trueColor) {
        for (i = num_palette; i < gdMaxColors; ++i) {
            if (!open[i]) {
                fprintf(stderr,
                        "gd-png warning: image data references out-of-range"
                        " color index (%d)\n",
                        i);
            }
        }
    }
#endif

done:
    if (palette_allocated) {
        gdFree(palette);
    }
    if (image_data)
        gdFree(image_data);
    if (row_pointers)
        gdFree(row_pointers);

    return im;

error:
    png_destroy_read_struct(&png_ptr, &info_ptr, NULL);
    if (im) {
        gdImageDestroy(im);
        im = NULL;
    }
    goto done;
}

BGD_DECLARE(void) gdImagePngEx(gdImagePtr im, FILE *outFile, int level)
{
    gdIOCtx *out = gdNewFileCtx(outFile);
    if (out == NULL)
        return;
    gdImagePngCtxEx(im, out, level);
    out->gd_free(out);
}

BGD_DECLARE(void) gdImagePng(gdImagePtr im, FILE *outFile) { gdImagePngEx(im, outFile, -1); }

static int _gdImagePngCtxWithOptions(gdImagePtr im, gdIOCtx *outfile,
                                     const gdPngWriteOptions *options);

BGD_DECLARE(void) gdPngWriteOptionsInit(gdPngWriteOptions *options)
{
    if (options == NULL)
        return;
    memset(options, 0, sizeof(*options));
    options->compression_level = -1;
    options->filters = GD_PNG_FILTER_AUTO;
    options->compression_strategy = GD_PNG_COMPRESSION_STRATEGY_DEFAULT;
}

BGD_DECLARE(void) gdPngInfoInit(gdPngInfo *info)
{
    gdImageMetadata *metadata;

    if (info == NULL)
        return;
    metadata = info->metadata;
    memset(info, 0, sizeof(*info));
    info->palette_entries = -1;
    info->x_pixels_per_unit = -1;
    info->y_pixels_per_unit = -1;
    info->physical_unit = -1;
    info->metadata = metadata;
    info->resolution_x = -1;
    info->resolution_y = -1;
}

BGD_DECLARE(int) gdPngGetInfoPtr(int size, const void *data, gdPngInfo *info)
{
    const unsigned char *png = (const unsigned char *)data;
    size_t png_size;
    size_t pos;
    gdImageMetadata *metadata;
    int seen_ihdr = FALSE;
    int seen_iend = FALSE;

    if (info == NULL || data == NULL || size < 0) {
        return 1;
    }

    metadata = info->metadata;
    memset(info, 0, sizeof(*info));
    info->palette_entries = -1;
    info->x_pixels_per_unit = -1;
    info->y_pixels_per_unit = -1;
    info->physical_unit = -1;
    info->metadata = metadata;
    info->resolution_x = -1;
    info->resolution_y = -1;

    png_size = (size_t)size;
    if (png_size < 8 || memcmp(png, gdPngSignature, 8) != 0) {
        return 1;
    }

    pos = 8;
    while (pos + 12 <= png_size) {
        unsigned int chunk_size = gdPngGetUint32(png + pos);
        const unsigned char *type = png + pos + 4;
        const unsigned char *chunk_data = png + pos + 8;
        size_t chunk_total;

        if ((size_t)chunk_size > png_size - pos - 12) {
            return 1;
        }
        chunk_total = (size_t)chunk_size + 12;

        if (!seen_ihdr) {
            png_uint_32 width, height;

            if (!gdPngChunkIs(type, "IHDR") || chunk_size != 13) {
                return 1;
            }
            width = gdPngGetUint32(chunk_data);
            height = gdPngGetUint32(chunk_data + 4);
            if (width == 0 || height == 0 || width > INT_MAX || height > INT_MAX) {
                return 1;
            }
            info->width = (int)width;
            info->height = (int)height;
            info->bit_depth = chunk_data[8];
            info->color_type = chunk_data[9];
            info->interlace_method = chunk_data[12];
            if (!gdPngInfoValidBitDepth(info->color_type, info->bit_depth) ||
                chunk_data[10] != PNG_COMPRESSION_TYPE_BASE ||
                chunk_data[11] != PNG_FILTER_TYPE_BASE) {
                return 1;
            }
            switch (info->color_type) {
            case PNG_COLOR_TYPE_RGB:
            case PNG_COLOR_TYPE_RGB_ALPHA:
            case PNG_COLOR_TYPE_GRAY_ALPHA:
                info->decoded_truecolor = TRUE;
                break;
            case PNG_COLOR_TYPE_PALETTE:
            case PNG_COLOR_TYPE_GRAY:
                info->decoded_truecolor = FALSE;
                break;
            default:
                return 1;
            }
            info->has_alpha = (info->color_type == PNG_COLOR_TYPE_RGB_ALPHA ||
                               info->color_type == PNG_COLOR_TYPE_GRAY_ALPHA);
            if (info->interlace_method != PNG_INTERLACE_NONE &&
                info->interlace_method != PNG_INTERLACE_ADAM7) {
                return 1;
            }
            seen_ihdr = TRUE;
        } else if (gdPngChunkIs(type, "PLTE")) {
            if (chunk_size % 3 != 0 || chunk_size / 3 > 256) {
                return 1;
            }
            info->palette_entries = (int)(chunk_size / 3);
        } else if (gdPngChunkIs(type, "tRNS")) {
            info->has_transparency = TRUE;
        } else if (gdPngChunkIs(type, "pHYs")) {
            unsigned int x_pixels_per_unit;
            unsigned int y_pixels_per_unit;

            if (chunk_size != 9) {
                return 1;
            }
            x_pixels_per_unit = gdPngGetUint32(chunk_data);
            y_pixels_per_unit = gdPngGetUint32(chunk_data + 4);
            if (x_pixels_per_unit > INT_MAX || y_pixels_per_unit > INT_MAX) {
                return 1;
            }
            info->x_pixels_per_unit = (int)x_pixels_per_unit;
            info->y_pixels_per_unit = (int)y_pixels_per_unit;
            info->physical_unit = chunk_data[8];
            if (info->physical_unit == PNG_RESOLUTION_METER) {
                info->resolution_x = (int)DPM2DPI(x_pixels_per_unit);
                info->resolution_y = (int)DPM2DPI(y_pixels_per_unit);
            }
        } else if (gdPngChunkIs(type, "tEXt")) {
            if (metadata != NULL && gdPngSetTextProfile(metadata, chunk_data, chunk_size) != GD_META_OK) {
                return 1;
            }
        } else if (gdPngChunkIs(type, "eXIf")) {
            if (metadata != NULL) {
                int status;
                if (chunk_size >= 6 && memcmp(chunk_data, "Exif\0\0", 6) == 0) {
                    status = gdImageMetadataSetProfile(metadata, "exif", chunk_data,
                                                       chunk_size);
                } else if (sizeof(size_t) < sizeof(uint64_t) && chunk_size > UINT32_MAX - 6) {
                    return 1;
                } else {
                    unsigned char *exif = gdMalloc((size_t)chunk_size + 6);
                    if (exif == NULL) {
                        return 1;
                    }
                    memcpy(exif, "Exif\0\0", 6);
                    memcpy(exif + 6, chunk_data, chunk_size);
                    status = gdImageMetadataSetProfile(metadata, "exif", exif,
                                                       (size_t)chunk_size + 6);
                    gdFree(exif);
                }
                if (status != GD_META_OK) {
                    return 1;
                }
            }
        } else if (gdPngChunkIs(type, "zTXt") &&
                   (gdPngIsRawProfile(chunk_data, chunk_size, "exif") ||
                    gdPngIsRawProfile(chunk_data, chunk_size, "xmp"))) {
            const char *key = gdPngIsRawProfile(chunk_data, chunk_size, "exif") ? "exif" : "xmp";
            unsigned char *decoded = NULL;
            size_t decoded_size = 0;
            int status = gdPngDecodeRawProfile(chunk_data, chunk_size, key, &decoded, &decoded_size);
            if (metadata != NULL && status == GD_META_OK &&
                gdImageMetadataSetProfile(metadata, key, decoded, decoded_size) != GD_META_OK) {
                status = GD_META_ERR_LIMIT;
            }
            gdFree(decoded);
            if (status != GD_META_OK) {
                return 1;
            }
        } else if (gdPngChunkIs(type, "iTXt")) {
            if (metadata != NULL && gdImageMetadataSetProfile(metadata, "xmp", chunk_data,
                                                               chunk_size) != GD_META_OK) {
                return 1;
            }
        } else if (gdPngChunkIs(type, "IEND")) {
            if (chunk_size != 0) {
                return 1;
            }
            seen_iend = TRUE;
            break;
        }

        pos += chunk_total;
    }

    return seen_ihdr && seen_iend ? 0 : 1;
}

BGD_DECLARE(int) gdPngGetInfoCtx(gdIOCtx *infile, gdPngInfo *info)
{
    void *data;
    int size;
    int status;

    data = gdPngReadCtxToMemory(infile, &size);
    if (data == NULL) {
        return 1;
    }
    status = gdPngGetInfoPtr(size, data, info);
    gdFree(data);
    return status;
}

BGD_DECLARE(int) gdPngGetInfo(FILE *inFile, gdPngInfo *info)
{
    gdIOCtx *in;
    int status;

    if (inFile == NULL) {
        return 1;
    }
    in = gdNewFileCtx(inFile);
    if (in == NULL) {
        return 1;
    }
    status = gdPngGetInfoCtx(in, info);
    in->gd_free(in);
    return status;
}

static int gdPngWriteOptionsValid(const gdPngWriteOptions *options)
{
    if (options->compression_level < -1 || options->compression_level > 9) {
        gd_error("gd-png error: compression level must be -1 through 9\n");
        return FALSE;
    }
    if ((options->filters & ~GD_PNG_FILTER_ALL) != 0) {
        gd_error("gd-png error: invalid filter mask\n");
        return FALSE;
    }
    if (options->compression_strategy < GD_PNG_COMPRESSION_STRATEGY_DEFAULT ||
        options->compression_strategy > GD_PNG_COMPRESSION_STRATEGY_FIXED) {
        gd_error("gd-png error: invalid compression strategy\n");
        return FALSE;
    }
    return TRUE;
}

static unsigned int gdPngWriteOptionResolutionX(gdImagePtr im,
                                                const gdPngWriteOptions *options)
{
    if (options->resolution_x != 0) {
        return options->resolution_x;
    }
    return im->res_x;
}

static unsigned int gdPngWriteOptionResolutionY(gdImagePtr im,
                                                const gdPngWriteOptions *options)
{
    if (options->resolution_y != 0) {
        return options->resolution_y;
    }
    return im->res_y;
}

static void gdPngFreeOwnedMetadata(unsigned char **owned_data, size_t count)
{
    size_t i;
    if (owned_data == NULL) {
        return;
    }
    for (i = 0; i < count; i++) {
        gdFree(owned_data[i]);
    }
    gdFree(owned_data);
}

static int gdPngSetMetadata(png_structp png_ptr, png_infop info_ptr,
                            const gdImageMetadata *metadata)
{
#ifdef PNG_STORE_UNKNOWN_CHUNKS_SUPPORTED
    png_unknown_chunk *chunks;
    unsigned char **owned_data;
    size_t count;
    size_t chunk_count = 0;
    size_t i;

    if (metadata == NULL) {
        return GD_META_OK;
    }

    count = gdImageMetadataGetProfileCount(metadata);
    if (count > INT_MAX) {
        return GD_META_ERR_LIMIT;
    }
    chunks = gdCalloc(count == 0 ? 1 : count, sizeof(*chunks));
    if (chunks == NULL) {
        return GD_META_ERR_NOMEM;
    }
    owned_data = gdCalloc(count == 0 ? 1 : count, sizeof(*owned_data));
    if (owned_data == NULL) {
        gdFree(chunks);
        return GD_META_ERR_NOMEM;
    }

    for (i = 0; i < count; i++) {
        const char *key = NULL;
        const unsigned char *data = NULL;
        size_t size = 0;
        const char *chunk_name = NULL;

        if (gdImageMetadataGetProfileAt(metadata, i, &key, &data, &size) != GD_META_OK ||
            key == NULL || (data == NULL && size != 0)) {
            gdPngFreeOwnedMetadata(owned_data, chunk_count);
            gdFree(chunks);
            return GD_META_ERR_INVALID;
        }

        if (strcmp(key, "exif") == 0) {
            const unsigned char *tiff_data;
            size_t tiff_size;

            if (gdPngIsRawProfile(data, size, "exif")) {
                unsigned char *decoded = NULL;
                size_t decoded_size = 0;
                if (gdPngDecodeRawProfile(data, size, "exif", &decoded, &decoded_size) != GD_META_OK) {
                    gdPngFreeOwnedMetadata(owned_data, chunk_count);
                    gdFree(chunks);
                    return GD_META_ERR_PARSE;
                }
                owned_data[chunk_count] = decoded;
                data = decoded;
                size = decoded_size;
            }
            if (gdMetadataGetExifTiff(data, size, &tiff_data, &tiff_size) != GD_META_OK) {
                gdPngFreeOwnedMetadata(owned_data, chunk_count);
                gdFree(chunks);
                return GD_META_ERR_PARSE;
            }
            data = tiff_data;
            size = tiff_size;
            chunk_name = "eXIf";
        } else if (strcmp(key, "xmp") == 0) {
            chunk_name = gdPngIsRawProfile(data, size, "xmp") ? "zTXt" : "iTXt";
        } else if (strncmp(key, "png:text:", 9) == 0) {
            chunk_name = "tEXt";
        } else {
            continue;
        }

        memcpy(chunks[chunk_count].name, chunk_name, 4);
        chunks[chunk_count].name[4] = '\0';
        chunks[chunk_count].data = (png_byte *)data;
        chunks[chunk_count].size = size;
        chunks[chunk_count].location = PNG_HAVE_IHDR;
        chunk_count++;
    }

    if (chunk_count != 0) {
        png_set_unknown_chunks(png_ptr, info_ptr, chunks, (int)chunk_count);
        png_set_keep_unknown_chunks(png_ptr, PNG_HANDLE_CHUNK_ALWAYS, NULL, 0);
        for (i = 0; i < chunk_count; i++) {
            png_set_unknown_chunk_location(png_ptr, info_ptr, (int)i, PNG_HAVE_IHDR);
        }
    }
    gdPngFreeOwnedMetadata(owned_data, chunk_count);
    gdFree(chunks);
    return GD_META_OK;
#else
    return metadata == NULL || gdImageMetadataGetProfileCount(metadata) == 0
        ? GD_META_OK : GD_META_ERR_UNSUPPORTED;
#endif
}

BGD_DECLARE(int)
gdImagePngWithOptions(gdImagePtr im, FILE *outFile, const gdPngWriteOptions *options)
{
    gdIOCtx *out;
    int status;
    if (im == NULL || outFile == NULL)
        return 1;
    out = gdNewFileCtx(outFile);
    if (out == NULL)
        return 1;
    status = gdImagePngCtxWithOptions(im, out, options);
    out->gd_free(out);
    return status;
}

BGD_DECLARE(void *) gdImagePngPtr(gdImagePtr im, int *size)
{
    return gdImagePngPtrEx(im, size, -1);
}

BGD_DECLARE(void *) gdImagePngPtrEx(gdImagePtr im, int *size, int level)
{
    void *rv;
    gdIOCtx *out = gdNewDynamicCtx(2048, NULL);
    gdPngWriteOptions options;
    if (out == NULL)
        return NULL;
    gdPngWriteOptionsInit(&options);
    options.compression_level = level;
    if (!_gdImagePngCtxWithOptions(im, out, &options))
        rv = gdDPExtractData(out, size);
    else
        rv = NULL;
    out->gd_free(out);
    return rv;
}

BGD_DECLARE(void *)
gdImagePngPtrWithOptions(gdImagePtr im, int *size, const gdPngWriteOptions *options)
{
    gdPngWriteOptions defaults;
    gdIOCtx *out;
    void *rv = NULL;
    if (size != NULL)
        *size = 0;
    if (im == NULL || size == NULL)
        return NULL;
    if (options == NULL) {
        gdPngWriteOptionsInit(&defaults);
        options = &defaults;
    }
    if (!gdPngWriteOptionsValid(options))
        return NULL;
    out = gdNewDynamicCtx(2048, NULL);
    if (out == NULL)
        return NULL;
    if (!_gdImagePngCtxWithOptions(im, out, options))
        rv = gdDPExtractData(out, size);
    out->gd_free(out);
    return rv;
}

BGD_DECLARE(void) gdImagePngCtx(gdImagePtr im, gdIOCtx *outfile)
{
    /* 2.0.13: 'return' here was an error, thanks to Kevin Smith */
    gdImagePngCtxEx(im, outfile, -1);
}

BGD_DECLARE(void) gdImagePngCtxEx(gdImagePtr im, gdIOCtx *outfile, int level)
{
    gdPngWriteOptions options;
    gdPngWriteOptionsInit(&options);
    options.compression_level = level;
    (void)_gdImagePngCtxWithOptions(im, outfile, &options);
}

BGD_DECLARE(int)
gdImagePngCtxWithOptions(gdImagePtr im, gdIOCtx *outfile, const gdPngWriteOptions *options)
{
    gdPngWriteOptions defaults;

    if (im == NULL || outfile == NULL)
        return 1;
    if (options == NULL) {
        gdPngWriteOptionsInit(&defaults);
        options = &defaults;
    }
    if (!gdPngWriteOptionsValid(options))
        return 1;
    return _gdImagePngCtxWithOptions(im, outfile, options);
}

/* This routine is based in part on code from Dale Lutz (Safe Software Inc.)
 *  and in part on demo code from Chapter 15 of "PNG: The Definitive Guide"
 *  (http://www.libpng.org/pub/png/book/).
 */
/* returns 0 on success, 1 on failure */
static int _gdImagePngCtxWithOptions(gdImagePtr im, gdIOCtx *outfile,
                                     const gdPngWriteOptions *options)
{
    int i, j, bit_depth = 0, interlace_type;
    int width = im->sx;
    int height = im->sy;
    int colors = im->colorsTotal;
    int *open = im->open;
    int mapping[gdMaxColors]; /* mapping[gd_index] == png_index */
    png_byte trans_values[256];
    png_color_16 trans_rgb_value;
    png_color palette[gdMaxColors];
    png_structp png_ptr;
    png_infop info_ptr;
    png_bytep *row_pointers = NULL;
    volatile int transparent = im->transparent;
    volatile int remap = FALSE;
#ifdef PNG_SETJMP_SUPPORTED
    jmpbuf_wrapper jbw;
#endif
    int ret = 0;

    /* width or height of value 0 is invalid in IHDR;
       see http://www.w3.org/TR/PNG-Chunks.html */
    if (width == 0 || height == 0)
        return 1;

#ifdef PNG_SETJMP_SUPPORTED
    png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING, &jbw, gdPngErrorHandler,
                                      gdPngWarningHandler);
#else
    png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
#endif
    if (png_ptr == NULL) {
        gd_error("gd-png error: cannot allocate libpng main struct\n");
        return 1;
    }

    info_ptr = png_create_info_struct(png_ptr);
    if (info_ptr == NULL) {
        gd_error("gd-png error: cannot allocate libpng info struct\n");
        png_destroy_write_struct(&png_ptr, (png_infopp)NULL);
        return 1;
    }

#ifdef PNG_SETJMP_SUPPORTED
    if (setjmp(jbw.jmpbuf)) {
        gd_error("gd-png error: setjmp returns error condition\n");
        png_destroy_write_struct(&png_ptr, &info_ptr);

        if (row_pointers) {
            for (i = 0; i < height; ++i)
                gdFree(row_pointers[i]);
            gdFree(row_pointers);
        }

        return 1;
    }
#endif

    png_set_write_fn(png_ptr, (void *)outfile, gdPngWriteData, gdPngFlushData);

    png_set_user_limits(png_ptr, 0x7fffffffL, 0x7fffffffL);

    /* This is best for palette images, and libpng defaults to it for
       palette images anyway, so we don't need to do it explicitly.
       What to ideally do for truecolor images depends, alas, on the image.
       gd is intentionally imperfect and doesn't spend a lot of time
       fussing with such things. */

    /* 2.0.12: this is finally a parameter */
    png_set_compression_level(png_ptr, options->compression_level);
    if (options->filters != GD_PNG_FILTER_AUTO) {
        int filters = 0;
        if (options->filters & GD_PNG_FILTER_NONE)
            filters |= PNG_FILTER_NONE;
        if (options->filters & GD_PNG_FILTER_SUB)
            filters |= PNG_FILTER_SUB;
        if (options->filters & GD_PNG_FILTER_UP)
            filters |= PNG_FILTER_UP;
        if (options->filters & GD_PNG_FILTER_AVERAGE)
            filters |= PNG_FILTER_AVG;
        if (options->filters & GD_PNG_FILTER_PAETH)
            filters |= PNG_FILTER_PAETH;
        png_set_filter(png_ptr, PNG_FILTER_TYPE_BASE, filters);
    }
    if (options->compression_strategy != GD_PNG_COMPRESSION_STRATEGY_DEFAULT) {
        int strategy = Z_DEFAULT_STRATEGY;
        switch (options->compression_strategy) {
        case GD_PNG_COMPRESSION_STRATEGY_FILTERED:
            strategy = Z_FILTERED;
            break;
        case GD_PNG_COMPRESSION_STRATEGY_HUFFMAN_ONLY:
            strategy = Z_HUFFMAN_ONLY;
            break;
        case GD_PNG_COMPRESSION_STRATEGY_RLE:
            strategy = Z_RLE;
            break;
        case GD_PNG_COMPRESSION_STRATEGY_FIXED:
            strategy = Z_FIXED;
            break;
        }
        png_set_compression_strategy(png_ptr, strategy);
    }

#ifdef PNG_pHYs_SUPPORTED
    /* 2.1.0: specify the resolution */
    png_set_pHYs(png_ptr, info_ptr, DPI2DPM(gdPngWriteOptionResolutionX(im, options)),
                 DPI2DPM(gdPngWriteOptionResolutionY(im, options)), PNG_RESOLUTION_METER);
#endif

    /* can set this to a smaller value without compromising compression if all
     * image data is 16K or less; will save some decoder memory [min == 8] */
    /*  png_set_compression_window_bits(png_ptr, 15);  */

    if (!im->trueColor) {
        if (transparent >= im->colorsTotal || (transparent >= 0 && open[transparent]))
            transparent = -1;
    }
    if (!im->trueColor) {
        for (i = 0; i < gdMaxColors; ++i)
            mapping[i] = -1;
    }
    if (!im->trueColor) {
        /* count actual number of colors used (colorsTotal == high-water mark)
         */
        colors = 0;
        for (i = 0; i < im->colorsTotal; ++i) {
            if (!open[i]) {
                mapping[i] = colors;
                ++colors;
            }
        }
        if (colors == 0) {
            gd_error("gd-png error: no colors in palette\n");
            ret = 1;
            goto bail;
        }
        if (colors < im->colorsTotal) {
            remap = TRUE;
        }
        if (colors <= 2)
            bit_depth = 1;
        else if (colors <= 4)
            bit_depth = 2;
        else if (colors <= 16)
            bit_depth = 4;
        else
            bit_depth = 8;
    }
    interlace_type = im->interlace ? PNG_INTERLACE_ADAM7 : PNG_INTERLACE_NONE;

    if (im->trueColor) {
        if (im->saveAlphaFlag) {
            png_set_IHDR(png_ptr, info_ptr, width, height, 8, PNG_COLOR_TYPE_RGB_ALPHA,
                         interlace_type, PNG_COMPRESSION_TYPE_DEFAULT, PNG_FILTER_TYPE_DEFAULT);
        } else {
            png_set_IHDR(png_ptr, info_ptr, width, height, 8, PNG_COLOR_TYPE_RGB, interlace_type,
                         PNG_COMPRESSION_TYPE_DEFAULT, PNG_FILTER_TYPE_DEFAULT);
        }
    } else {
        png_set_IHDR(png_ptr, info_ptr, width, height, bit_depth, PNG_COLOR_TYPE_PALETTE,
                     interlace_type, PNG_COMPRESSION_TYPE_DEFAULT, PNG_FILTER_TYPE_DEFAULT);
    }
    if (im->trueColor && (!im->saveAlphaFlag) && (transparent >= 0)) {
        /* 2.0.9: fixed by Thomas Winzig */
        trans_rgb_value.red = gdTrueColorGetRed(im->transparent);
        trans_rgb_value.green = gdTrueColorGetGreen(im->transparent);
        trans_rgb_value.blue = gdTrueColorGetBlue(im->transparent);
        png_set_tRNS(png_ptr, info_ptr, 0, 0, &trans_rgb_value);
    }
    if (!im->trueColor) {
        /* Oy veh. Remap the PNG palette to put the
           entries with interesting alpha channel
           values first. This minimizes the size
           of the tRNS chunk and thus the size
           of the PNG file as a whole. */
        int tc = 0;
        int i;
        int j;
        int k;
        for (i = 0; (i < im->colorsTotal); i++) {
            if ((!im->open[i]) && (im->alpha[i] != gdAlphaOpaque)) {
                tc++;
            }
        }
        if (tc) {
#if 0
			for (i = 0; (i < im->colorsTotal); i++) {
				trans_values[i] = 255 -
				                  ((im->alpha[i] << 1) + (im->alpha[i] >> 6));
			}
			png_set_tRNS (png_ptr, info_ptr, trans_values, 256, NULL);
#endif
            if (!remap) {
                remap = TRUE;
            }
            /* (Semi-)transparent indexes come up from the bottom
               of the list of real colors; opaque
               indexes come down from the top */
            j = 0;
            k = colors - 1;
            for (i = 0; (i < im->colorsTotal); i++) {
                if (!im->open[i]) {
                    if (im->alpha[i] != gdAlphaOpaque) {
                        /* Andrew Hull: >> 6, not >> 7! (gd 2.0.5) */
                        trans_values[j] = 255 - ((im->alpha[i] << 1) + (im->alpha[i] >> 6));
                        mapping[i] = j++;
                    } else {
                        mapping[i] = k--;
                    }
                }
            }
            png_set_tRNS(png_ptr, info_ptr, trans_values, tc, NULL);
        }
    }

    /* convert palette to libpng layout */
    if (!im->trueColor) {
        if (remap)
            for (i = 0; i < im->colorsTotal; ++i) {
                if (mapping[i] < 0)
                    continue;
                palette[mapping[i]].red = im->red[i];
                palette[mapping[i]].green = im->green[i];
                palette[mapping[i]].blue = im->blue[i];
            }
        else
            for (i = 0; i < colors; ++i) {
                palette[i].red = im->red[i];
                palette[i].green = im->green[i];
                palette[i].blue = im->blue[i];
            }
        png_set_PLTE(png_ptr, info_ptr, palette, colors);
    }

    if (gdPngSetMetadata(png_ptr, info_ptr, options->metadata) != GD_META_OK) {
        ret = 1;
        goto bail;
    }

    /* write out the PNG header info (everything up to first IDAT) */
    png_write_info(png_ptr, info_ptr);

    /* make sure < 8-bit images are packed into pixels as tightly as possible */
    png_set_packing(png_ptr);

    /* This code allocates a set of row buffers and copies the gd image data
     * into them only in the case that remapping is necessary; in gd 1.3 and
     * later, the im->pixels array is laid out identically to libpng's row
     * pointers and can be passed to png_write_image() function directly.
     * The remapping case could be accomplished with less memory for non-
     * interlaced images, but interlacing causes some serious complications. */
    if (im->trueColor) {
        /* performance optimizations by Phong Tran */
        int channels = im->saveAlphaFlag ? 4 : 3;
        /* Our little 7-bit alpha channel trick costs us a bit here. */
        unsigned char *pOutputRow;
        int **ptpixels = im->tpixels;
        int *pThisRow;
        unsigned char a;
        int thisPixel;
        png_bytep *prow_pointers;
        int saveAlphaFlag = im->saveAlphaFlag;
        if (overflow2(sizeof(png_bytep), height)) {
            ret = 1;
            goto bail;
        }
        /* Need to use calloc so we can clean it up sanely in the error handler.
         */
        row_pointers = gdCalloc(height, sizeof(png_bytep));
        if (row_pointers == NULL) {
            gd_error("gd-png error: unable to allocate row_pointers\n");
            ret = 1;
            goto bail;
        }
        prow_pointers = row_pointers;
        for (j = 0; j < height; ++j) {
            if (overflow2(width, channels) ||
                ((*prow_pointers = (png_bytep)gdMalloc(width * channels)) == NULL)) {
                gd_error("gd-png error: unable to allocate rows\n");
                for (i = 0; i < j; ++i)
                    gdFree(row_pointers[i]);
                /* 2.0.29: memory leak TBB */
                gdFree(row_pointers);
                ret = 1;
                goto bail;
            }
            pOutputRow = *prow_pointers++;
            pThisRow = *ptpixels++;
            for (i = 0; i < width; ++i) {
                thisPixel = *pThisRow++;
                *pOutputRow++ = gdTrueColorGetRed(thisPixel);
                *pOutputRow++ = gdTrueColorGetGreen(thisPixel);
                *pOutputRow++ = gdTrueColorGetBlue(thisPixel);

                if (saveAlphaFlag) {
                    /* convert the 7-bit alpha channel to an 8-bit alpha
                       channel. We do a little bit-flipping magic, repeating the
                       MSB as the LSB, to ensure that 0 maps to 0 and 127 maps
                       to 255. We also have to invert to match PNG's convention
                       in which 255 is opaque. */
                    a = gdTrueColorGetAlpha(thisPixel);
                    /* Andrew Hull: >> 6, not >> 7! (gd 2.0.5) */
                    *pOutputRow++ = 255 - ((a << 1) + (a >> 6));
                }
            }
        }

        png_write_image(png_ptr, row_pointers);
        png_write_end(png_ptr, info_ptr);

        for (j = 0; j < height; ++j)
            gdFree(row_pointers[j]);
        gdFree(row_pointers);
    } else {
        if (remap) {
            png_bytep *row_pointers;
            if (overflow2(sizeof(png_bytep), height)) {
                ret = 1;
                goto bail;
            }
            row_pointers = gdMalloc(sizeof(png_bytep) * height);
            if (row_pointers == NULL) {
                gd_error("gd-png error: unable to allocate row_pointers\n");
                ret = 1;
                goto bail;
            }
            for (j = 0; j < height; ++j) {
                if ((row_pointers[j] = (png_bytep)gdMalloc(width)) == NULL) {
                    gd_error("gd-png error: unable to allocate rows\n");
                    for (i = 0; i < j; ++i)
                        gdFree(row_pointers[i]);
                    /* TBB: memory leak */
                    gdFree(row_pointers);
                    ret = 1;
                    goto bail;
                }
                for (i = 0; i < width; ++i)
                    row_pointers[j][i] = mapping[im->pixels[j][i]];
            }

            png_write_image(png_ptr, row_pointers);

            for (j = 0; j < height; ++j)
                gdFree(row_pointers[j]);
            gdFree(row_pointers);

            png_write_end(png_ptr, info_ptr);
        } else {
            png_write_image(png_ptr, im->pixels);
            png_write_end(png_ptr, info_ptr);
        }
    }
    /* 1.6.3: maybe we should give that memory BACK! TBB */
bail:
    png_destroy_write_struct(&png_ptr, &info_ptr);
    return ret;
}

#else /* !HAVE_LIBPNG */

static void _noPngError(void) { gd_error("PNG image support has been disabled\n"); }

BGD_DECLARE(void) gdPngWriteOptionsInit(gdPngWriteOptions *options)
{
    if (options == NULL)
        return;
    memset(options, 0, sizeof(*options));
    options->compression_level = -1;
}

BGD_DECLARE(void) gdPngInfoInit(gdPngInfo *info)
{
    gdImageMetadata *metadata;

    if (info == NULL)
        return;
    metadata = info->metadata;
    memset(info, 0, sizeof(*info));
    info->palette_entries = -1;
    info->x_pixels_per_unit = -1;
    info->y_pixels_per_unit = -1;
    info->physical_unit = -1;
    info->metadata = metadata;
    info->resolution_x = -1;
    info->resolution_y = -1;
}

BGD_DECLARE(int) gdPngGetInfo(FILE *inFile, gdPngInfo *info)
{
    ARG_NOT_USED(inFile);
    ARG_NOT_USED(info);
    _noPngError();
    return 1;
}

BGD_DECLARE(int) gdPngGetInfoCtx(gdIOCtx *infile, gdPngInfo *info)
{
    ARG_NOT_USED(infile);
    ARG_NOT_USED(info);
    _noPngError();
    return 1;
}

BGD_DECLARE(int) gdPngGetInfoPtr(int size, const void *data, gdPngInfo *info)
{
    ARG_NOT_USED(size);
    ARG_NOT_USED(data);
    ARG_NOT_USED(info);
    _noPngError();
    return 1;
}

BGD_DECLARE(int)
gdImagePngWithOptions(gdImagePtr im, FILE *outFile, const gdPngWriteOptions *options)
{
    ARG_NOT_USED(im);
    ARG_NOT_USED(outFile);
    ARG_NOT_USED(options);
    _noPngError();
    return 1;
}

BGD_DECLARE(int)
gdImagePngCtxWithOptions(gdImagePtr im, gdIOCtx *outfile, const gdPngWriteOptions *options)
{
    ARG_NOT_USED(im);
    ARG_NOT_USED(outfile);
    ARG_NOT_USED(options);
    _noPngError();
    return 1;
}

BGD_DECLARE(void *)
gdImagePngPtrWithOptions(gdImagePtr im, int *size, const gdPngWriteOptions *options)
{
    ARG_NOT_USED(im);
    ARG_NOT_USED(options);
    if (size != NULL)
        *size = 0;
    _noPngError();
    return NULL;
}

BGD_DECLARE(gdImagePtr) gdImageCreateFromPng(FILE *inFile)
{
    ARG_NOT_USED(inFile);
    _noPngError();
    return NULL;
}

BGD_DECLARE(gdImagePtr) gdImageCreateFromPngPtr(int size, void *data)
{
    ARG_NOT_USED(size);
    ARG_NOT_USED(data);
    return NULL;
}

BGD_DECLARE(gdImagePtr) gdImageCreateFromPngCtx(gdIOCtx *infile)
{
    ARG_NOT_USED(infile);
    return NULL;
}

BGD_DECLARE(void) gdImagePngEx(gdImagePtr im, FILE *outFile, int level)
{
    ARG_NOT_USED(im);
    ARG_NOT_USED(outFile);
    ARG_NOT_USED(level);
    _noPngError();
}

BGD_DECLARE(void) gdImagePng(gdImagePtr im, FILE *outFile)
{
    ARG_NOT_USED(im);
    ARG_NOT_USED(outFile);
    _noPngError();
}

BGD_DECLARE(void *) gdImagePngPtr(gdImagePtr im, int *size)
{
    ARG_NOT_USED(im);
    ARG_NOT_USED(size);
    return NULL;
}

BGD_DECLARE(void *) gdImagePngPtrEx(gdImagePtr im, int *size, int level)
{
    ARG_NOT_USED(im);
    ARG_NOT_USED(size);
    ARG_NOT_USED(level);
    return NULL;
}

BGD_DECLARE(void) gdImagePngCtx(gdImagePtr im, gdIOCtx *outfile)
{
    ARG_NOT_USED(im);
    ARG_NOT_USED(outfile);
    _noPngError();
}

BGD_DECLARE(void) gdImagePngCtxEx(gdImagePtr im, gdIOCtx *outfile, int level)
{
    ARG_NOT_USED(im);
    ARG_NOT_USED(outfile);
    ARG_NOT_USED(level);
    _noPngError();
}

#endif /* HAVE_LIBPNG */
