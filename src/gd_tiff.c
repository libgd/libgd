/*
   TIFF - Tagged Image File Format Encapsulation for GD Library

   gd_tiff.c
   Copyright (C) Pierre-A. Joye, M. Retallack

   ---------------------------------------------------------------------------
   **
   ** Permission to use, copy, modify, and distribute this software and its
   ** documentation for any purpose and without fee is hereby granted, provided
   ** that the above copyright notice appear in all copies and that both that
   ** copyright notice and this permission notice appear in supporting
   ** documentation.  This software is provided "as is" without express or
   ** implied warranty.
   **
   ---------------------------------------------------------------------------
   Ctx code written by M. Retallack

   Todo:

   If we fail - cleanup
   Writer: Use gd error function, overflow check may not be necessary as
         we write our own data (check already done)

   Implement 2 color black/white saving using group4 fax compression
   Implement function to specify encoding to use when writing tiff data

   ----------------------------------------------------------------------------
 */

/**
 * File: TIFF IO
 *
 * Read and write TIFF images.
 *
 * Multi-page TIFF reading is supported via the gdTiffRead* API.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "gd.h"
#include "gd_errors.h"
#include "gd_intern.h"
#include "gdfonts.h"
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "gdhelpers.h"

#ifdef HAVE_LIBTIFF

#include "tiff.h"
#include "tiffio.h"

#define GD_TIFF_COMPILE_ASSERT(name, condition) typedef char gd_tiff_assert_##name[(condition) ? 1 : -1]

GD_TIFF_COMPILE_ASSERT(compression_none, GD_TIFF_COMPRESSION_NONE == COMPRESSION_NONE);
GD_TIFF_COMPILE_ASSERT(compression_ccitt_rle, GD_TIFF_COMPRESSION_CCITT_RLE == COMPRESSION_CCITTRLE);
GD_TIFF_COMPILE_ASSERT(compression_ccitt_fax3, GD_TIFF_COMPRESSION_CCITT_FAX3 == COMPRESSION_CCITTFAX3);
GD_TIFF_COMPILE_ASSERT(compression_ccitt_fax4, GD_TIFF_COMPRESSION_CCITT_FAX4 == COMPRESSION_CCITTFAX4);
GD_TIFF_COMPILE_ASSERT(compression_lzw, GD_TIFF_COMPRESSION_LZW == COMPRESSION_LZW);
GD_TIFF_COMPILE_ASSERT(compression_jpeg, GD_TIFF_COMPRESSION_JPEG == COMPRESSION_JPEG);
GD_TIFF_COMPILE_ASSERT(compression_adobe_deflate,
                       GD_TIFF_COMPRESSION_ADOBE_DEFLATE == COMPRESSION_ADOBE_DEFLATE);
GD_TIFF_COMPILE_ASSERT(compression_deflate, GD_TIFF_COMPRESSION_DEFLATE == COMPRESSION_DEFLATE);
GD_TIFF_COMPILE_ASSERT(compression_packbits, GD_TIFF_COMPRESSION_PACKBITS == COMPRESSION_PACKBITS);
GD_TIFF_COMPILE_ASSERT(photometric_miniswhite,
                       GD_TIFF_PHOTOMETRIC_MINISWHITE == PHOTOMETRIC_MINISWHITE);
GD_TIFF_COMPILE_ASSERT(photometric_minisblack,
                       GD_TIFF_PHOTOMETRIC_MINISBLACK == PHOTOMETRIC_MINISBLACK);
GD_TIFF_COMPILE_ASSERT(photometric_rgb, GD_TIFF_PHOTOMETRIC_RGB == PHOTOMETRIC_RGB);
GD_TIFF_COMPILE_ASSERT(photometric_palette, GD_TIFF_PHOTOMETRIC_PALETTE == PHOTOMETRIC_PALETTE);
GD_TIFF_COMPILE_ASSERT(photometric_mask,
                       GD_TIFF_PHOTOMETRIC_TRANSPARENCY_MASK == PHOTOMETRIC_MASK);
GD_TIFF_COMPILE_ASSERT(photometric_separated,
                       GD_TIFF_PHOTOMETRIC_SEPARATED == PHOTOMETRIC_SEPARATED);

#define GD_TIFF_META_MAGIC "GDTF"
#define GD_TIFF_META_HEADER_SIZE 16

static TIFFExtendProc gd_tiff_metadata_parent_extender;
static const TIFFFieldInfo gd_tiff_metadata_fields[] = {
    {33550, 3, 3, TIFF_DOUBLE, FIELD_CUSTOM, 1, 0, "ModelPixelScaleTag"},
    {33922, 6, 6, TIFF_DOUBLE, FIELD_CUSTOM, 1, 0, "ModelTiepointTag"},
    {34735, TIFF_VARIABLE2, TIFF_VARIABLE2, TIFF_SHORT, FIELD_CUSTOM, 1, 1, "GeoKeyDirectoryTag"},
    {34736, TIFF_VARIABLE2, TIFF_VARIABLE2, TIFF_DOUBLE, FIELD_CUSTOM, 1, 1, "GeoDoubleParamsTag"},
    {34737, TIFF_VARIABLE2, TIFF_VARIABLE2, TIFF_ASCII, FIELD_CUSTOM, 1, 1, "GeoAsciiParamsTag"},
};

static void gd_tiff_metadata_extender(TIFF *tif)
{
    if (gd_tiff_metadata_parent_extender != NULL) {
        gd_tiff_metadata_parent_extender(tif);
    }
    TIFFMergeFieldInfo(tif, gd_tiff_metadata_fields,
                       sizeof(gd_tiff_metadata_fields) / sizeof(gd_tiff_metadata_fields[0]));
}

static void gd_tiff_metadata_register_fields(void)
{
    static int registered;
    if (!registered) {
        gd_tiff_metadata_parent_extender = TIFFSetTagExtender(gd_tiff_metadata_extender);
        registered = 1;
    }
}

static void gd_tiff_metadata_put16(unsigned char *p, uint16_t value)
{
    p[0] = (unsigned char) value;
    p[1] = (unsigned char) (value >> 8);
}

static void gd_tiff_metadata_put32(unsigned char *p, uint32_t value)
{
    p[0] = (unsigned char) value;
    p[1] = (unsigned char) (value >> 8);
    p[2] = (unsigned char) (value >> 16);
    p[3] = (unsigned char) (value >> 24);
}

static uint16_t gd_tiff_metadata_get16(const unsigned char *p)
{
    return (uint16_t) p[0] | ((uint16_t) p[1] << 8);
}

static uint32_t gd_tiff_metadata_get32(const unsigned char *p)
{
    return (uint32_t) p[0] | ((uint32_t) p[1] << 8) | ((uint32_t) p[2] << 16) | ((uint32_t) p[3] << 24);
}

static int gd_tiff_metadata_is_little_endian(void)
{
    const uint16_t value = 1;
    return *(const unsigned char *) &value == 1;
}

static void gd_tiff_metadata_copy_numeric(unsigned char *dst, const unsigned char *src,
                                           size_t element_size, size_t count)
{
    size_t i;
    if (gd_tiff_metadata_is_little_endian() && dst != src) {
        memcpy(dst, src, element_size * count);
        return;
    }
    for (i = 0; i < count; i++) {
        size_t j;
        if (dst == src) {
            for (j = 0; j < element_size / 2; j++) {
                unsigned char temp = dst[i * element_size + j];
                dst[i * element_size + j] = dst[i * element_size + element_size - j - 1];
                dst[i * element_size + element_size - j - 1] = temp;
            }
        } else {
            for (j = 0; j < element_size; j++) {
                dst[i * element_size + j] = src[i * element_size + element_size - j - 1];
            }
        }
    }
}

static int gd_tiff_metadata_profile(uint32_t tag, TIFFDataType type,
                                    uint32_t count, const void *data, size_t element_size,
                                    gdImageMetadata *metadata)
{
    size_t payload_size;
    size_t total_size;
    unsigned char *encoded;
    char profile_key[32];

    if (count > SIZE_MAX / element_size) {
        return GD_META_ERR_LIMIT;
    }
    payload_size = element_size * (size_t) count;
    if (payload_size > SIZE_MAX - GD_TIFF_META_HEADER_SIZE) {
        return GD_META_ERR_LIMIT;
    }
    total_size = GD_TIFF_META_HEADER_SIZE + payload_size;
    if (total_size > INT_MAX) {
        return GD_META_ERR_LIMIT;
    }
    encoded = (unsigned char *) gdMalloc(total_size);
    if (encoded == NULL) {
        return GD_META_ERR_NOMEM;
    }
    memcpy(encoded, GD_TIFF_META_MAGIC, 4);
    encoded[4] = 1;
    encoded[5] = 1;
    gd_tiff_metadata_put16(encoded + 6, (uint16_t) type);
    gd_tiff_metadata_put32(encoded + 8, count);
    gd_tiff_metadata_put32(encoded + 12, (uint32_t) payload_size);
    if (type == TIFF_ASCII) {
        memcpy(encoded + GD_TIFF_META_HEADER_SIZE, data, payload_size);
    } else {
        gd_tiff_metadata_copy_numeric(encoded + GD_TIFF_META_HEADER_SIZE, (const unsigned char *) data,
                                      element_size, count);
    }
    snprintf(profile_key, sizeof(profile_key), "tiff:tag:%u", tag);
    if (gdImageMetadataSetProfile(metadata, profile_key, encoded, total_size) != GD_META_OK) {
        gdFree(encoded);
        return GD_META_ERR_LIMIT;
    }
    gdFree(encoded);
    return GD_META_OK;
}

static int gd_tiff_metadata_collect(TIFF *tif, gdImageMetadata *metadata)
{
    static const uint32_t tags[] = {33550, 33922, 34735, 34736, 34737};
    size_t i;

    if (tif == NULL || metadata == NULL) return GD_META_ERR_INVALID;
    for (i = 0; i < sizeof(tags) / sizeof(tags[0]); i++) {
        uint32_t tag = tags[i];
        if (tag == 33550 || tag == 33922) {
            double *data = NULL;
            const TIFFField *field = TIFFFieldWithTag(tif, tag);
            if (field != NULL && TIFFGetField(tif, tag, &data)) {
                int status = gd_tiff_metadata_profile(tag, TIFF_DOUBLE,
                    (uint32_t) TIFFFieldReadCount(field), data, sizeof(double), metadata);
                if (status != GD_META_OK) return status;
            }
        } else if (tag == 34735) {
            uint32_t count = 0;
            uint16_t *data = NULL;
            if (TIFFGetField(tif, tag, &count, &data)) {
                int status = gd_tiff_metadata_profile(tag, TIFF_SHORT, count, data, sizeof(uint16_t), metadata);
                if (status != GD_META_OK) return status;
            }
        } else if (tag == 34736) {
            uint32_t count = 0;
            double *data = NULL;
            if (TIFFGetField(tif, tag, &count, &data)) {
                int status = gd_tiff_metadata_profile(tag, TIFF_DOUBLE, count, data, sizeof(double), metadata);
                if (status != GD_META_OK) return status;
            }
        } else if (tag == 34737) {
            uint32_t count = 0;
            char *data = NULL;
            if (TIFFGetField(tif, tag, &count, &data)) {
                int status = gd_tiff_metadata_profile(tag, TIFF_ASCII, count, data, sizeof(char), metadata);
                if (status != GD_META_OK) return status;
            }
        }
    }
    return GD_META_OK;
}

static int gd_tiff_metadata_apply(TIFF *tif, const gdImageMetadata *metadata)
{
    size_t i;
    if (tif == NULL || metadata == NULL) return GD_META_OK;
    for (i = 0; i < gdImageMetadataGetProfileCount(metadata); i++) {
        const char *key, *tag_text;
        const unsigned char *encoded;
        size_t size;
        uint32_t tag, count, payload_size;
        TIFFDataType type;
        unsigned char *payload;

        if (gdImageMetadataGetProfileAt(metadata, i, &key, &encoded, &size) != GD_META_OK ||
            strncmp(key, "tiff:tag:", 9) != 0 || size < GD_TIFF_META_HEADER_SIZE ||
            memcmp(encoded, GD_TIFF_META_MAGIC, 4) != 0 || encoded[4] != 1) continue;
        tag_text = key + 9;
        tag = (uint32_t) strtoul(tag_text, NULL, 10);
        type = (TIFFDataType) gd_tiff_metadata_get16(encoded + 6);
        count = gd_tiff_metadata_get32(encoded + 8);
        payload_size = gd_tiff_metadata_get32(encoded + 12);
        if (payload_size != size - GD_TIFF_META_HEADER_SIZE) return GD_META_ERR_PARSE;
        payload = (unsigned char *) gdMalloc(payload_size ? payload_size : 1);
        if (payload == NULL) return GD_META_ERR_NOMEM;
        memcpy(payload, encoded + GD_TIFF_META_HEADER_SIZE, payload_size);
        if (type == TIFF_DOUBLE || type == TIFF_SHORT) {
            size_t element_size = type == TIFF_DOUBLE ? sizeof(double) : sizeof(uint16_t);
            if (gd_tiff_metadata_is_little_endian() == 0) {
                gd_tiff_metadata_copy_numeric(payload, payload, element_size, count);
            }
            if (tag == 33550 || tag == 33922) {
                if (!TIFFSetField(tif, tag, payload)) { gdFree(payload); return GD_META_ERR_INVALID; }
            } else if (!TIFFSetField(tif, tag, count, payload)) {
                gdFree(payload); return GD_META_ERR_INVALID;
            }
        } else if (type == TIFF_ASCII) {
            if (!TIFFSetField(tif, tag, count, payload)) { gdFree(payload); return GD_META_ERR_INVALID; }
        } else {
            gdFree(payload);
            continue;
        }
        gdFree(payload);
    }
    return GD_META_OK;
}
GD_TIFF_COMPILE_ASSERT(photometric_ycbcr, GD_TIFF_PHOTOMETRIC_YCBCR == PHOTOMETRIC_YCBCR);
GD_TIFF_COMPILE_ASSERT(photometric_cielab, GD_TIFF_PHOTOMETRIC_CIELAB == PHOTOMETRIC_CIELAB);
GD_TIFF_COMPILE_ASSERT(planar_contig, GD_TIFF_PLANARCONFIG_CONTIG == PLANARCONFIG_CONTIG);
GD_TIFF_COMPILE_ASSERT(planar_separate, GD_TIFF_PLANARCONFIG_SEPARATE == PLANARCONFIG_SEPARATE);
GD_TIFF_COMPILE_ASSERT(resunit_none, GD_TIFF_RESUNIT_NONE == RESUNIT_NONE);
GD_TIFF_COMPILE_ASSERT(resunit_inch, GD_TIFF_RESUNIT_INCH == RESUNIT_INCH);
GD_TIFF_COMPILE_ASSERT(resunit_centimeter, GD_TIFF_RESUNIT_CENTIMETER == RESUNIT_CENTIMETER);

#define GD_SUCCESS 1
#define GD_FAILURE 0

#define TRUE 1
#define FALSE 0

/* I define those here until the new formats
 * are committed. We can then rely on the global
 * def
 */
#define GD_PALETTE 1
#define GD_TRUECOLOR 2
#define GD_GRAY 3
#define GD_INDEXED 4
#define GD_RGB 5

typedef struct tiff_handle {
    int size;
    int pos;
    gdIOCtx *ctx;
    int written;
} tiff_handle;

/*
   Functions for reading, writing and seeking in gdIOCtx
   This allows for non-file i/o operations with no
   explicit use of libtiff fileio wrapper functions

   Note: because libtiff requires random access, but gdIOCtx
                 only supports streams, all writes are buffered
                 into memory and written out on close, also all
                 reads are done from a memory mapped version of the
                 tiff (assuming one already exists)
*/

static tiff_handle *new_tiff_handle(gdIOCtx *g, int initial_size)
{
    tiff_handle *t;

    if (!g) {
        gd_error("Cannot create a new tiff handle, missing Ctx argument");
        return NULL;
    }

    t = (tiff_handle *)gdMalloc(sizeof(tiff_handle));
    if (!t) {
        gd_error("Failed to allocate a new tiff handle");
        return NULL;
    }

    t->size = initial_size;
    t->pos = 0;
    t->ctx = g;
    t->written = 0;

    return t;
}

/* TIFFReadWriteProc tiff_readproc - Will use gdIOCtx procs to read required
   (previously written) TIFF file content */
static tsize_t tiff_readproc(thandle_t clientdata, tdata_t data, tsize_t size)
{
    tiff_handle *th = (tiff_handle *)clientdata;
    gdIOCtx *ctx = th->ctx;

    size = (ctx->getBuf)(ctx, data, size);
    if (size > 0) {
        th->pos += size;
    }

    return size;
}

/* TIFFReadWriteProc tiff_writeproc - Will use gdIOCtx procs to write out
   TIFF data */
static tsize_t tiff_writeproc(thandle_t clientdata, tdata_t data, tsize_t size)
{
    tiff_handle *th = (tiff_handle *)clientdata;
    gdIOCtx *ctx = th->ctx;

    size = (ctx->putBuf)(ctx, data, size);
    if (size > 0) {
        th->pos += size;
    }
    if (th->pos > th->size) {
        th->size = th->pos;
    }

    return size;
}

/* TIFFSeekProc tiff_seekproc
 * used to move around the partially written TIFF */
static toff_t tiff_seekproc(thandle_t clientdata, toff_t offset, int from)
{
    tiff_handle *th = (tiff_handle *)clientdata;
    gdIOCtx *ctx = th->ctx;
    int result;

    switch (from) {
    default:
    case SEEK_SET:
        /* just use offset */
        break;

    case SEEK_END:
        /* invert offset, so that it is from start, not end as supplied */
        offset = th->size + offset;
        break;

    case SEEK_CUR:
        /* add current position to translate it to 'from start',
         * not from durrent as supplied
         */
        offset += th->pos;
        break;
    }

    /* now, move pos in both io context and buf */
    if ((result = (ctx->seek)(ctx, offset))) {
        th->pos = offset;
    }

    return result ? offset : (toff_t)-1;
}

/* TIFFCloseProc tiff_closeproc - used to finally close the TIFF file */
static int tiff_closeproc(thandle_t clientdata)
{
    (void)clientdata;
    /*tiff_handle *th = (tiff_handle *)clientdata;
    gdIOCtx *ctx = th->ctx;

    (ctx->gd_free)(ctx);*/

    return 0;
}

/* TIFFSizeProc tiff_sizeproc */
static toff_t tiff_sizeproc(thandle_t clientdata)
{
    tiff_handle *th = (tiff_handle *)clientdata;
    return th->size;
}

/* TIFFMapFileProc tiff_mapproc() */
static int tiff_mapproc(thandle_t h, tdata_t *d, toff_t *o)
{
    (void)h;
    (void)d;
    (void)o;
    return 0;
}

/* TIFFUnmapFileProc tiff_unmapproc */
static void tiff_unmapproc(thandle_t h, tdata_t d, toff_t o)
{
    (void)h;
    (void)d;
    (void)o;
}

static int tiff_file_size(FILE *fp)
{
    long current_pos;
    long end_pos;

    if (!fp) {
        return 0;
    }

    current_pos = ftell(fp);
    if (current_pos < 0) {
        return 0;
    }

    if (fseek(fp, 0, SEEK_END) != 0) {
        (void)fseek(fp, current_pos, SEEK_SET);
        return 0;
    }

    end_pos = ftell(fp);
    (void)fseek(fp, current_pos, SEEK_SET);

    if (end_pos < 0) {
        return 0;
    }

    if (end_pos > INT_MAX) {
        return INT_MAX;
    }

    return (int)end_pos;
}

static int tiff_ctx_size(gdIOCtx *ctx)
{
    unsigned char buffer[4096];
    long current_pos;
    long end_pos;

    if (ctx == NULL || ctx->tell == NULL || ctx->seek == NULL)
        return 0;

    current_pos = ctx->tell(ctx);
    if (current_pos < 0)
        return 0;

    while (ctx->getBuf(ctx, buffer, sizeof(buffer)) > 0)
        ;
    end_pos = ctx->tell(ctx);
    (void)ctx->seek(ctx, (int)current_pos);

    if (end_pos < 0)
        return 0;
    if (end_pos > INT_MAX)
        return INT_MAX;

    return (int)end_pos;
}

/*  tiffWriter
 *  ----------
 *  Write the gd image as a tiff file (called by gdImageTiffCtx)
 *  Parameters are:
 *  image:    gd image structure;
 *  out:      the stream where to write
 *  bitDepth: depth in bits of each pixel
 */
static void tiffWriter(gdImagePtr image, gdIOCtx *out, int bitDepth)
{
    int x, y;
    int i;
    int r, g, b, a;
    TIFF *tiff;
    int width, height;
    int color;
    char *scan;
    int samplesPerPixel = 3;
    int bitsPerSample;
    int transparentColorR = -1;
    int transparentColorG = -1;
    int transparentColorB = -1;
    uint16_t extraSamples[1];
    uint16_t *colorMapRed = NULL;
    uint16_t *colorMapGreen = NULL;
    uint16_t *colorMapBlue = NULL;
    size_t colorMapSize;

    tiff_handle *th;

    th = new_tiff_handle(out, 0);
    if (!th) {
        return;
    }
    extraSamples[0] = EXTRASAMPLE_ASSOCALPHA;

    /* read in the width/height of gd image */
    width = gdImageSX(image);
    height = gdImageSY(image);

    /* reset clip region to whole image */
    gdImageSetClip(image, 0, 0, width, height);

    /* handle old-style single-colour mapping to 100% transparency */
    if (image->transparent != -1) {
        /* set our 100% transparent colour value */
        transparentColorR = gdImageRed(image, image->transparent);
        transparentColorG = gdImageGreen(image, image->transparent);
        transparentColorB = gdImageBlue(image, image->transparent);
    }

    /* Open tiff file writing routines, but use special read/write/seek
     * functions so that tiff lib writes correct bits of tiff content to
     * correct areas of file opened and modifieable by the gdIOCtx functions
     */
    tiff = TIFFClientOpen("", "w", th, tiff_readproc, tiff_writeproc, tiff_seekproc, tiff_closeproc,
                          tiff_sizeproc, tiff_mapproc, tiff_unmapproc);

    TIFFSetField(tiff, TIFFTAG_IMAGEWIDTH, width);
    TIFFSetField(tiff, TIFFTAG_IMAGELENGTH, height);
    TIFFSetField(tiff, TIFFTAG_COMPRESSION, COMPRESSION_ADOBE_DEFLATE);
    TIFFSetField(tiff, TIFFTAG_PLANARCONFIG, PLANARCONFIG_CONTIG);
    TIFFSetField(tiff, TIFFTAG_PHOTOMETRIC,
                 (bitDepth == 24) ? PHOTOMETRIC_RGB : PHOTOMETRIC_PALETTE);

    bitsPerSample = (bitDepth == 24 || bitDepth == 8) ? 8 : 1;
    TIFFSetField(tiff, TIFFTAG_BITSPERSAMPLE, bitsPerSample);

    TIFFSetField(tiff, TIFFTAG_XRESOLUTION, (float)image->res_x);
    TIFFSetField(tiff, TIFFTAG_YRESOLUTION, (float)image->res_y);

    /* build the color map for 8 bit images */
    if (bitDepth != 24) {
        if (overflow2(1 << bitsPerSample, sizeof(uint16_t))) {
            gdFree(th);
            return;
        }
        colorMapSize = (size_t)(1 << bitsPerSample) * sizeof(uint16_t);
        colorMapRed = (uint16_t *)gdMalloc(colorMapSize);
        if (!colorMapRed) {
            gdFree(th);
            return;
        }
        colorMapGreen = (uint16_t *)gdMalloc(colorMapSize);
        if (!colorMapGreen) {
            gdFree(colorMapRed);
            gdFree(th);
            return;
        }
        colorMapBlue = (uint16_t *)gdMalloc(colorMapSize);
        if (!colorMapBlue) {
            gdFree(colorMapRed);
            gdFree(colorMapGreen);
            gdFree(th);
            return;
        }

        for (i = 0; i < image->colorsTotal; i++) {
            colorMapRed[i] = gdImageRed(image, i) + (gdImageRed(image, i) * 256);
            colorMapGreen[i] = gdImageGreen(image, i) + (gdImageGreen(image, i) * 256);
            colorMapBlue[i] = gdImageBlue(image, i) + (gdImageBlue(image, i) * 256);
        }

        TIFFSetField(tiff, TIFFTAG_COLORMAP, colorMapRed, colorMapGreen, colorMapBlue);
        samplesPerPixel = 1;
    }

    /* here, we check if the 'save alpha' flag is set on the source gd image */
    if ((bitDepth == 24) && (image->saveAlphaFlag || image->transparent != -1)) {
        /* so, we need to store the alpha values too!
         * Also, tell TIFF what the extra sample means (associated alpha) */
        samplesPerPixel = 4;
        TIFFSetField(tiff, TIFFTAG_SAMPLESPERPIXEL, samplesPerPixel);
        TIFFSetField(tiff, TIFFTAG_EXTRASAMPLES, 1, extraSamples);
    } else {
        TIFFSetField(tiff, TIFFTAG_SAMPLESPERPIXEL, samplesPerPixel);
    }

    TIFFSetField(tiff, TIFFTAG_ROWSPERSTRIP, 1);

    if (overflow2(width, samplesPerPixel)) {
        if (colorMapRed)
            gdFree(colorMapRed);
        if (colorMapGreen)
            gdFree(colorMapGreen);
        if (colorMapBlue)
            gdFree(colorMapBlue);
        gdFree(th);
        return;
    }

    if (!(scan = (char *)gdMalloc(width * samplesPerPixel))) {
        if (colorMapRed)
            gdFree(colorMapRed);
        if (colorMapGreen)
            gdFree(colorMapGreen);
        if (colorMapBlue)
            gdFree(colorMapBlue);
        gdFree(th);
        return;
    }

    /* loop through y-coords, and x-coords */
    for (y = 0; y < height; y++) {
        for (x = 0; x < width; x++) {
            /* generate scan line for writing to tiff */
            color = gdImageGetPixel(image, x, y);

            a = (127 - gdImageAlpha(image, color)) * 2;
            a = (a == 0xfe) ? 0xff : a & 0xff;
            b = gdImageBlue(image, color);
            g = gdImageGreen(image, color);
            r = gdImageRed(image, color);

            /* if this pixel has the same RGB as the transparent colour,
             * then set alpha fully transparent */
            if (transparentColorR == r && transparentColorG == g && transparentColorB == b) {
                a = 0x00;
            }

            if (bitDepth != 24) {
                /* write out 1 or 8 bit value in 1 byte
                 * (currently treats 1bit as 8bit) */
                scan[(x * samplesPerPixel) + 0] = color;
            } else {
                /* write out 24 bit value in 3 (or 4 if transparent) bytes */
                if (image->saveAlphaFlag || image->transparent != -1) {
                    scan[(x * samplesPerPixel) + 3] = a;
                }

                scan[(x * samplesPerPixel) + 2] = b;
                scan[(x * samplesPerPixel) + 1] = g;
                scan[(x * samplesPerPixel) + 0] = r;
            }
        }

        /* Write the scan line to the tiff */
        if (TIFFWriteEncodedStrip(tiff, y, scan, width * samplesPerPixel) == -1) {
            if (colorMapRed)
                gdFree(colorMapRed);
            if (colorMapGreen)
                gdFree(colorMapGreen);
            if (colorMapBlue)
                gdFree(colorMapBlue);
            gdFree(th);
            /* error handler here */
            gd_error("Could not create TIFF\n");
            return;
        }
    }

    /* now cloase and free up resources */
    TIFFClose(tiff);
    gdFree(scan);
    gdFree(th);

    if (bitDepth != 24) {
        gdFree(colorMapRed);
        gdFree(colorMapGreen);
        gdFree(colorMapBlue);
    }
}

BGD_DECLARE(void) gdImageTiffCtx(gdImagePtr image, gdIOCtx *out)
{
    int clipx1P, clipy1P, clipx2P, clipy2P;
    int bitDepth = 24;

    /* First, switch off clipping, or we'll not get all the image! */
    gdImageGetClip(image, &clipx1P, &clipy1P, &clipx2P, &clipy2P);

    /* use the appropriate routine depending on the bit depth of the image */
    if (image->trueColor) {
        bitDepth = 24;
    } else if (image->colorsTotal == 2) {
        bitDepth = 1;
    } else {
        bitDepth = 8;
    }

    tiffWriter(image, out, bitDepth);

    /* reset clipping area to the gd image's original values */
    gdImageSetClip(image, clipx1P, clipy1P, clipx2P, clipy2P);
}

/* Check if we are really in 8bit mode */
static int checkColorMap(int n, uint16_t *r, uint16_t *g, uint16_t *b)
{
    while (n-- > 0)
        if (*r++ >= 256 || *g++ >= 256 || *b++ >= 256)
            return (16);
    return (8);
}

/* Read and convert a TIFF colormap */
static int readTiffColorMap(gdImagePtr im, TIFF *tif, char is_bw, int photometric)
{
    uint16_t *redcmap, *greencmap, *bluecmap;
    uint16_t bps;
    int i;

    if (is_bw) {
        if (photometric == PHOTOMETRIC_MINISWHITE) {
            gdImageColorAllocate(im, 255, 255, 255);
            gdImageColorAllocate(im, 0, 0, 0);
        } else {
            gdImageColorAllocate(im, 0, 0, 0);
            gdImageColorAllocate(im, 255, 255, 255);
        }
    } else {
        uint16_t min_sample_val, max_sample_val;

        if (!TIFFGetField(tif, TIFFTAG_MINSAMPLEVALUE, &min_sample_val)) {
            min_sample_val = 0;
        }
        if (!TIFFGetField(tif, TIFFTAG_MAXSAMPLEVALUE, &max_sample_val)) {
            max_sample_val = 255;
        }

        if (photometric == PHOTOMETRIC_MINISBLACK || photometric == PHOTOMETRIC_MINISWHITE) {
            /* TODO: use TIFFTAG_MINSAMPLEVALUE and TIFFTAG_MAXSAMPLEVALUE */
            /* Gray level palette */
            for (i = min_sample_val; i <= max_sample_val; i++) {
                gdImageColorAllocate(im, i, i, i);
            }
            return GD_SUCCESS;

        } else if (!TIFFGetField(tif, TIFFTAG_COLORMAP, &redcmap, &greencmap, &bluecmap)) {
            gd_error("Cannot read the color map");
            return GD_FAILURE;
        }

        TIFFGetFieldDefaulted(tif, TIFFTAG_BITSPERSAMPLE, &bps);

#define CVT(x) (((x) * 255) / ((1L << 16) - 1))
        if (checkColorMap(1 << bps, redcmap, greencmap, bluecmap) == 16) {
            for (i = (1 << bps) - 1; i > 0; i--) {
                redcmap[i] = CVT(redcmap[i]);
                greencmap[i] = CVT(greencmap[i]);
                bluecmap[i] = CVT(bluecmap[i]);
            }
        }
        for (i = 0; i < 256; i++) {
            gdImageColorAllocate(im, redcmap[i], greencmap[i], bluecmap[i]);
        }
#undef CVT
    }
    return GD_SUCCESS;
}

static void readTiffBw(const unsigned char *src, gdImagePtr im, uint16_t photometric, int startx,
                       int starty, int width, int height, char has_alpha, int extra, int align)
{
    int x = startx, y = starty;

    (void)has_alpha;
    (void)extra;
    (void)align;

    for (y = starty; y < starty + height; y++) {
        for (x = startx; x < startx + width;) {
            register unsigned char curr = *src++;
            register unsigned char mask;

            if (photometric == PHOTOMETRIC_MINISWHITE) {
                curr = ~curr;
            }
            for (mask = 0x80; mask != 0 && x < startx + width; x++, mask >>= 1) {
                gdImageSetPixel(im, x, y, ((curr & mask) != 0) ? 0 : 1);
            }
        }
    }
}

static void readTiff8bit(const unsigned char *src, gdImagePtr im, uint16_t photometric, int startx,
                         int starty, int width, int height, char has_alpha, int extra, int align)
{
    int red, green, blue, alpha;
    int x, y;

    (void)extra;
    (void)align;

    switch (photometric) {
    case PHOTOMETRIC_PALETTE:
        /* Palette has no alpha (see TIFF specs for more details */
        for (y = starty; y < starty + height; y++) {
            for (x = startx; x < startx + width; x++) {
                gdImageSetPixel(im, x, y, *(src++));
            }
        }
        break;

    case PHOTOMETRIC_RGB:
        if (has_alpha) {
            gdImageAlphaBlending(im, 0);
            gdImageSaveAlpha(im, 1);

            for (y = starty; y < starty + height; y++) {
                for (x = startx; x < startx + width; x++) {
                    red = *src++;
                    green = *src++;
                    blue = *src++;
                    alpha = *src++;
                    red = MIN(red, alpha);
                    blue = MIN(blue, alpha);
                    green = MIN(green, alpha);

                    if (alpha) {
                        gdImageSetPixel(im, x, y,
                                        gdTrueColorAlpha(red * 255 / alpha, green * 255 / alpha,
                                                         blue * 255 / alpha,
                                                         gdAlphaMax - (alpha >> 1)));
                    } else {
                        gdImageSetPixel(
                            im, x, y,
                            gdTrueColorAlpha(red, green, blue, gdAlphaMax - (alpha >> 1)));
                    }
                }
            }

        } else {
            for (y = 0; y < height; y++) {
                for (x = 0; x < width; x++) {
                    register unsigned char r = *src++;
                    register unsigned char g = *src++;
                    register unsigned char b = *src++;

                    gdImageSetPixel(im, x, y, gdTrueColor(r, g, b));
                }
            }
        }
        break;

    case PHOTOMETRIC_MINISWHITE:
        if (has_alpha) {
            /* We don't process the extra yet */
        } else {
            for (y = starty; y < starty + height; y++) {
                for (x = startx; x < startx + width; x++) {
                    gdImageSetPixel(im, x, y, ~(*src++));
                }
            }
        }
        break;

    case PHOTOMETRIC_MINISBLACK:
        if (has_alpha) {
            /* We don't process the extra yet */
        } else {
            for (y = starty; y < height; y++) {
                for (x = 0; x < width; x++) {
                    gdImageSetPixel(im, x, y, *src++);
                }
            }
        }
        break;
    }
}

static int createFromTiffTiles(TIFF *tif, gdImagePtr im, uint16_t bps, uint16_t photometric,
                               char has_alpha, char is_bw, int extra)
{
    uint16_t planar;
    int im_width, im_height;
    int tile_width, tile_height;
    int x, y, height, width;
    unsigned char *buffer;
    tmsize_t tile_size;
    int success = GD_SUCCESS;

    if (!TIFFGetField(tif, TIFFTAG_PLANARCONFIG, &planar)) {
        planar = PLANARCONFIG_CONTIG;
    }
    if (TIFFGetField(tif, TIFFTAG_IMAGEWIDTH, &im_width) == 0 ||
        TIFFGetField(tif, TIFFTAG_IMAGELENGTH, &im_height) == 0 ||
        TIFFGetField(tif, TIFFTAG_TILEWIDTH, &tile_width) == 0 ||
        TIFFGetField(tif, TIFFTAG_TILELENGTH, &tile_height) == 0) {
        return FALSE;
    }
    if (tile_width <= 0 || tile_height <= 0) {
        return FALSE;
    }

    tile_size = TIFFTileSize(tif);
    if (tile_size <= 0) {
        return FALSE;
    }
    buffer = (unsigned char *)gdMalloc((size_t)tile_size);
    if (!buffer) {
        return FALSE;
    }

    for (y = 0; y < im_height; y += tile_height) {
        for (x = 0; x < im_width; x += tile_width) {
            if (TIFFReadTile(tif, buffer, x, y, 0, 0) < 0) {
                success = GD_FAILURE;
                goto end;
            }
            width = MIN(im_width - x, tile_width);
            height = MIN(im_height - y, tile_height);
            if (bps == 8) {
                readTiff8bit(buffer, im, photometric, x, y, width, height, has_alpha, extra, 0);
            } else if (is_bw) {
                readTiffBw(buffer, im, photometric, x, y, width, height, has_alpha, extra, 0);
            } else {
                gd_error("TIFF error, unsupported tiled image format in direct "
                         "reader");
                success = GD_FAILURE;
                goto end;
            }
        }
    }
end:
    gdFree(buffer);
    return success;
}

static int createFromTiffLines(TIFF *tif, gdImagePtr im, uint16_t bps, uint16_t photometric,
                               char has_alpha, char is_bw, int extra)
{
    uint16_t planar;
    uint32_t im_height, im_width, y;

    unsigned char *buffer;
    int success = GD_SUCCESS;

    if (!TIFFGetField(tif, TIFFTAG_PLANARCONFIG, &planar)) {
        planar = PLANARCONFIG_CONTIG;
    }

    if (!TIFFGetField(tif, TIFFTAG_IMAGELENGTH, &im_height)) {
        gd_error("Can't fetch TIFF height\n");
        return FALSE;
    }

    if (!TIFFGetField(tif, TIFFTAG_IMAGEWIDTH, &im_width)) {
        gd_error("Can't fetch TIFF width \n");
        return FALSE;
    }

    if (im_width > INT_MAX || overflow2((int)im_width, 4)) {
        return GD_FAILURE;
    }
    buffer = (unsigned char *)gdMalloc((size_t)im_width * 4);
    if (!buffer) {
        return GD_FAILURE;
    }
    if (planar == PLANARCONFIG_CONTIG) {
        switch (bps) {
        case 8:
            for (y = 0; y < im_height; y++) {
                if (TIFFReadScanline(tif, buffer, y, 0) < 0) {
                    gd_error("Error while reading scanline %i", y);
                    success = GD_FAILURE;
                    break;
                }
                /* reading one line at a time */
                readTiff8bit(buffer, im, photometric, 0, y, im_width, 1, has_alpha, extra, 0);
            }
            break;

        default:
            if (is_bw) {
                for (y = 0; y < im_height; y++) {
                    if (TIFFReadScanline(tif, buffer, y, 0) < 0) {
                        gd_error("Error while reading scanline %i", y);
                        success = GD_FAILURE;
                        break;
                    }
                    /* reading one line at a time */
                    readTiffBw(buffer, im, photometric, 0, y, im_width, 1, has_alpha, extra, 0);
                }
            } else {
                gd_error("TIFF error, unsupported scanline image format in "
                         "direct reader");
                success = GD_FAILURE;
            }
            break;
        }
    } else {
        gd_error("TIFF error, unsupported separate planar image in direct reader");
        success = GD_FAILURE;
    }

    gdFree(buffer);
    return success;
}

static int createFromTiffRgba(TIFF *tif, gdImagePtr im)
{
    int a;
    int x, y;
    int alphaBlendingFlag = 0;
    int color;
    int width = im->sx;
    int height = im->sy;
    uint32_t *buffer;
    uint32_t rgba;
    int success;

    buffer = (uint32_t *)gdCalloc(sizeof(uint32_t), width * height);
    if (!buffer) {
        return GD_FAILURE;
    }

    /* switch off colour merging on target gd image just while we write out
     * content - we want to preserve the alpha data until the user chooses
     * what to do with the image */
    alphaBlendingFlag = im->alphaBlendingFlag;
    gdImageAlphaBlending(im, 0);

    success = TIFFReadRGBAImage(tif, width, height, buffer, 1);

    if (success) {
        for (y = 0; y < height; y++) {
            for (x = 0; x < width; x++) {
                /* if it doesn't already exist, allocate a new colour,
                 * else use existing one */
                rgba = buffer[(y * width + x)];
                a = (0xff - TIFFGetA(rgba)) / 2;
                color = gdTrueColorAlpha(TIFFGetR(rgba), TIFFGetG(rgba), TIFFGetB(rgba), a);

                /* set pixel colour to this colour */
                gdImageSetPixel(im, x, height - y - 1, color);
            }
        }
    }

    gdFree(buffer);

    /* now reset colour merge for alpha blending routines */
    gdImageAlphaBlending(im, alphaBlendingFlag);
    return success;
}

static gdImagePtr TiffDecodeCurrentDirectory(TIFF *tif)
{
    uint16_t bps, spp, photometric;
    uint16_t orientation;
    int width, height;
    uint16_t extra, *extra_types;
    uint16_t planar;
    char has_alpha, is_bw, is_gray;
    char force_rgba = FALSE;
    char save_transparent;
    int image_type;
    int ret;
    float res_float;

    gdImagePtr im = NULL;

    if (!TIFFGetField(tif, TIFFTAG_IMAGEWIDTH, &width)) {
        gd_error("TIFF error, Cannot read image width");
        return NULL;
    }

    if (!TIFFGetField(tif, TIFFTAG_IMAGELENGTH, &height)) {
        gd_error("TIFF error, Cannot read image height");
        return NULL;
    }

    if (width <= 0 || height <= 0) {
        gd_error("TIFF error, image dimensions must be greater than 0");
        return NULL;
    }

    if (overflow2(width, height)) {
        gd_error("TIFF error, image dimensions are too large");
        return NULL;
    }

    TIFFGetFieldDefaulted(tif, TIFFTAG_BITSPERSAMPLE, &bps);

    if (bps == 0 || bps > 32) {
        gd_error("TIFF error, invalid bits per sample: %u", (unsigned)bps);
        return NULL;
    }

    TIFFGetFieldDefaulted(tif, TIFFTAG_SAMPLESPERPIXEL, &spp);

    if (spp == 0 || spp > 4) {
        gd_error("TIFF error, invalid samples per pixel: %u", (unsigned)spp);
        return NULL;
    }

    if (!TIFFGetField(tif, TIFFTAG_EXTRASAMPLES, &extra, &extra_types)) {
        extra = 0;
    }

    if (!TIFFGetField(tif, TIFFTAG_PHOTOMETRIC, &photometric)) {
        uint16_t compression;
        if (TIFFGetField(tif, TIFFTAG_COMPRESSION, &compression) &&
            (compression == COMPRESSION_CCITTFAX3 || compression == COMPRESSION_CCITTFAX4 ||
             compression == COMPRESSION_CCITTRLE || compression == COMPRESSION_CCITTRLEW)) {
            gd_error("Could not get photometric. "
                     "Image is CCITT compressed, assuming min-is-white");
            photometric = PHOTOMETRIC_MINISWHITE;
        } else {
            gd_error("Could not get photometric. "
                     "Assuming min-is-black");

            photometric = PHOTOMETRIC_MINISBLACK;
        }
    }
    save_transparent = FALSE;

    /* test if the extrasample represents an associated alpha channel... */
    if (extra > 0 && (extra_types[0] == EXTRASAMPLE_ASSOCALPHA)) {
        has_alpha = TRUE;
        save_transparent = FALSE;
        --extra;
    } else if (extra > 0 && (extra_types[0] == EXTRASAMPLE_UNASSALPHA)) {
        has_alpha = TRUE;
        save_transparent = TRUE;
        --extra;
    } else if (extra > 0 && (extra_types[0] == EXTRASAMPLE_UNSPECIFIED)) {
        /* assuming unassociated alpha if unspecified */
        gd_error("alpha channel type not defined, assuming alpha is not "
                 "premultiplied");
        has_alpha = TRUE;
        save_transparent = TRUE;
        --extra;
    } else {
        has_alpha = FALSE;
    }

    if (photometric == PHOTOMETRIC_RGB && spp > 3 + extra) {
        has_alpha = TRUE;
        extra = spp - 4;
    } else if (photometric != PHOTOMETRIC_RGB && spp > 1 + extra) {
        has_alpha = TRUE;
        extra = spp - 2;
    }

    is_bw = FALSE;
    is_gray = FALSE;

    switch (photometric) {
    case PHOTOMETRIC_MINISBLACK:
    case PHOTOMETRIC_MINISWHITE:
        if (!has_alpha && bps == 1 && spp == 1) {
            image_type = GD_INDEXED;
            is_bw = TRUE;
        } else {
            image_type = GD_GRAY;
        }
        break;

    case PHOTOMETRIC_RGB:
        image_type = GD_RGB;
        break;

    case PHOTOMETRIC_PALETTE:
        image_type = GD_INDEXED;
        break;

    default:
        force_rgba = TRUE;
        break;
    }

    if (!TIFFGetField(tif, TIFFTAG_PLANARCONFIG, &planar)) {
        planar = PLANARCONFIG_CONTIG;
    }

    /* The direct scanline/tile readers only implement contiguous 1-bit BW
     * images. */
    if (!is_bw || bps != 1 || spp != 1 || has_alpha || planar != PLANARCONFIG_CONTIG) {
        force_rgba = TRUE;
    }

    /* Force rgba if image planes are not contiguous or the format is otherwise
     * unsupported. */
    if (force_rgba) {
        image_type = GD_RGB;
    }

    if (!force_rgba &&
        (image_type == GD_PALETTE || image_type == GD_INDEXED || image_type == GD_GRAY)) {
        im = gdImageCreate(width, height);
        if (!im)
            return NULL;
        readTiffColorMap(im, tif, is_bw, photometric);
    } else {
        im = gdImageCreateTrueColor(width, height);
        if (!im)
            return NULL;
    }

#ifdef DEBUG
    printf("force rgba: %i\n", force_rgba);
    printf("has_alpha: %i\n", has_alpha);
    printf("save trans: %i\n", save_transparent);
    printf("is_bw: %i\n", is_bw);
    printf("is_gray: %i\n", is_gray);
    printf("type: %i\n", image_type);
#else
    (void)is_gray;
    (void)save_transparent;
#endif

    if (force_rgba) {
        ret = createFromTiffRgba(tif, im);
    } else if (TIFFIsTiled(tif)) {
        ret = createFromTiffTiles(tif, im, bps, photometric, has_alpha, is_bw, extra);
    } else {
        ret = createFromTiffLines(tif, im, bps, photometric, has_alpha, is_bw, extra);
    }

    if (!ret) {
        gdImageDestroy(im);
        return NULL;
    }

    if (TIFFGetField(tif, TIFFTAG_XRESOLUTION, &res_float)) {
        im->res_x = (unsigned int)res_float; // truncate
    }
    if (TIFFGetField(tif, TIFFTAG_YRESOLUTION, &res_float)) {
        im->res_y = (unsigned int)res_float; // truncate
    }

    if (TIFFGetField(tif, TIFFTAG_ORIENTATION, &orientation)) {
        switch (orientation) {
        case ORIENTATION_TOPLEFT:
        case ORIENTATION_TOPRIGHT:
        case ORIENTATION_BOTRIGHT:
        case ORIENTATION_BOTLEFT:
            break;

        default:
            gd_error("Orientation %d not handled yet!", orientation);
            break;
        }
    }

    return im;
}

static gdImagePtr gdImageCreateFromTiffCtxEx(gdIOCtx *infile, int initial_size)
{
    TIFF *tif;
    tiff_handle *th;
    gdImagePtr im = NULL;

    th = new_tiff_handle(infile, initial_size);
    if (!th) {
        return NULL;
    }

    tif = TIFFClientOpen("", "rb", th, tiff_readproc, tiff_writeproc, tiff_seekproc, tiff_closeproc,
                         tiff_sizeproc, tiff_mapproc, tiff_unmapproc);

    if (!tif) {
        gd_error("Cannot open TIFF image");
        gdFree(th);
        return NULL;
    }

    im = TiffDecodeCurrentDirectory(tif);

    TIFFClose(tif);
    gdFree(th);
    return im;
}

BGD_DECLARE(gdImagePtr) gdImageCreateFromTiffCtx(gdIOCtx *infile)
{
    return gdImageCreateFromTiffCtxEx(infile, tiff_ctx_size(infile));
}

BGD_DECLARE(gdImagePtr) gdImageCreateFromTiff(FILE *inFile)
{
    gdImagePtr im;
    gdIOCtx *in = gdNewFileCtx(inFile);
    int initial_size = tiff_file_size(inFile);

    if (in == NULL)
        return NULL;
    im = gdImageCreateFromTiffCtxEx(in, initial_size);
    in->gd_free(in);
    return im;
}

BGD_DECLARE(gdImagePtr) gdImageCreateFromTiffPtr(int size, void *data)
{
    gdImagePtr im;
    gdIOCtx *in = gdNewDynamicCtxEx(size, data, 0);
    if (in == NULL)
        return NULL;
    im = gdImageCreateFromTiffCtxEx(in, size);
    in->gd_free(in);
    return im;
}

BGD_DECLARE(void) gdImageTiff(gdImagePtr im, FILE *outFile)
{
    gdIOCtx *out = gdNewFileCtx(outFile);
    if (out == NULL)
        return;
    gdImageTiffCtx(im, out); /* what's an fg again? */
    out->gd_free(out);
}

BGD_DECLARE(void *) gdImageTiffPtr(gdImagePtr im, int *size)
{
    void *rv;
    gdIOCtx *out = gdNewDynamicCtx(2048, NULL);
    if (out == NULL)
        return NULL;
    gdImageTiffCtx(im, out); /* what's an fg again? */
    rv = gdDPExtractData(out, size);
    out->gd_free(out);
    return rv;
}

#define GD_TIFF_ALLOC_STEP (4 * 1024)

typedef struct gdTiffReadStruct {
    uint8_t *data;
    size_t size;
    TIFF *tif;
    tiff_handle *th;
    gdIOCtx *memCtx;
    int pageCount;
    int currentPage;
} gdTiffRead;

static uint8_t *TiffReadCtxData(gdIOCtx *infile, size_t *size)
{
    uint8_t *filedata = NULL, *temp, *read;
    ssize_t n;

    *size = 0;
    do {
        temp = gdRealloc(filedata, *size + GD_TIFF_ALLOC_STEP);
        if (temp == NULL) {
            gdFree(filedata);
            gd_error("TIFF decode: realloc failed");
            return NULL;
        }
        filedata = temp;
        read = temp + *size;
        n = gdGetBuf(read, GD_TIFF_ALLOC_STEP, infile);
        if (n > 0 && n != EOF) {
            *size += n;
        }
    } while (n > 0 && n != EOF);

    if (*size == 0) {
        gdFree(filedata);
        return NULL;
    }

    return filedata;
}

static void TiffFillInfo(TIFF *tif, gdTiffInfo *info, int pageCount)
{
    uint16_t bps, spp, photometric, compression;
    float res_float;
    uint16_t resUnit;

    if (info == NULL || tif == NULL) {
        return;
    }
    memset(info, 0, sizeof(*info));
    info->page_count = pageCount;

    TIFFGetFieldDefaulted(tif, TIFFTAG_IMAGEWIDTH, (uint32_t *)&info->width);
    TIFFGetFieldDefaulted(tif, TIFFTAG_IMAGELENGTH, (uint32_t *)&info->height);
    TIFFGetFieldDefaulted(tif, TIFFTAG_BITSPERSAMPLE, &bps);
    TIFFGetFieldDefaulted(tif, TIFFTAG_SAMPLESPERPIXEL, &spp);
    info->bits_per_sample = bps;
    info->samples_per_pixel = spp;

    if (TIFFGetField(tif, TIFFTAG_COMPRESSION, &compression)) {
        info->compression = compression;
    }
    if (TIFFGetField(tif, TIFFTAG_PHOTOMETRIC, &photometric)) {
        info->photometric = photometric;
    }
    if (TIFFGetField(tif, TIFFTAG_XRESOLUTION, &res_float)) {
        info->x_resolution = res_float;
    }
    if (TIFFGetField(tif, TIFFTAG_YRESOLUTION, &res_float)) {
        info->y_resolution = res_float;
    }
    if (TIFFGetField(tif, TIFFTAG_RESOLUTIONUNIT, &resUnit)) {
        info->resolution_unit = resUnit;
    } else {
        info->resolution_unit = GD_TIFF_RESUNIT_INCH;
    }
}

static void TiffFillPageInfo(TIFF *tif, gdTiffPageInfo *info, int pageIndex)
{
    uint16_t bps, spp, photometric, compression, planar;
    uint16_t extra, *extra_types;
    uint16_t resUnit;
    float res_float;

    if (info == NULL || tif == NULL) {
        return;
    }
    memset(info, 0, sizeof(*info));
    info->page_index = pageIndex;

    TIFFGetFieldDefaulted(tif, TIFFTAG_IMAGEWIDTH, (uint32_t *)&info->width);
    TIFFGetFieldDefaulted(tif, TIFFTAG_IMAGELENGTH, (uint32_t *)&info->height);
    TIFFGetFieldDefaulted(tif, TIFFTAG_BITSPERSAMPLE, &bps);
    TIFFGetFieldDefaulted(tif, TIFFTAG_SAMPLESPERPIXEL, &spp);
    info->bits_per_sample = bps;
    info->samples_per_pixel = spp;
    info->is_tiled = TIFFIsTiled(tif);

    if (TIFFGetField(tif, TIFFTAG_COMPRESSION, &compression)) {
        info->compression = compression;
    }
    if (TIFFGetField(tif, TIFFTAG_PHOTOMETRIC, &photometric)) {
        info->photometric = photometric;
    }
    if (TIFFGetField(tif, TIFFTAG_PLANARCONFIG, &planar)) {
        info->planar = planar;
    } else {
        info->planar = GD_TIFF_PLANARCONFIG_CONTIG;
    }

    if (!TIFFGetField(tif, TIFFTAG_EXTRASAMPLES, &extra, &extra_types)) {
        extra = 0;
    }
    info->has_alpha = (extra > 0);

    if (TIFFGetField(tif, TIFFTAG_XRESOLUTION, &res_float)) {
        info->x_resolution = res_float;
    }
    if (TIFFGetField(tif, TIFFTAG_YRESOLUTION, &res_float)) {
        info->y_resolution = res_float;
    }
    if (TIFFGetField(tif, TIFFTAG_RESOLUTIONUNIT, &resUnit)) {
        info->resolution_unit = resUnit;
    } else {
        info->resolution_unit = GD_TIFF_RESUNIT_INCH;
    }
}

static gdTiffReadPtr TiffReadOpenFromData(uint8_t *data, size_t size)
{
    gdTiffReadPtr tiff;
    gdIOCtx *memCtx;
    tiff_handle *th;
    TIFF *tif;

    gd_tiff_metadata_register_fields();

    tiff = (gdTiffReadPtr)gdCalloc(1, sizeof(gdTiffRead));
    if (tiff == NULL) {
        return NULL;
    }

    tiff->data = data;
    tiff->size = size;

    memCtx = gdNewDynamicCtxEx((int)size, data, 0);
    if (memCtx == NULL) {
        gdFree(tiff);
        return NULL;
    }
    tiff->memCtx = memCtx;

    th = new_tiff_handle(memCtx, (int)size);
    if (th == NULL) {
        memCtx->gd_free(memCtx);
        gdFree(tiff);
        return NULL;
    }
    tiff->th = th;

    tif = TIFFClientOpen("", "rb", th, tiff_readproc, tiff_writeproc, tiff_seekproc, tiff_closeproc,
                         tiff_sizeproc, tiff_mapproc, tiff_unmapproc);
    if (tif == NULL) {
        gdFree(th);
        memCtx->gd_free(memCtx);
        gdFree(tiff);
        return NULL;
    }
    tiff->tif = tif;

    tiff->pageCount = (int)TIFFNumberOfDirectories(tif);
    if (tiff->pageCount <= 0) {
        gd_error("TIFF error, invalid page count: %d", tiff->pageCount);
        TIFFClose(tif);
        tiff->tif = NULL;
        gdFree(th);
        tiff->th = NULL;
        memCtx->gd_free(memCtx);
        tiff->memCtx = NULL;
        gdFree(tiff);
        return NULL;
    }
    tiff->currentPage = -1;

    return tiff;
}

static int TiffReadValidateOptions(const gdTiffReadOptions *options)
{
    ARG_NOT_USED(options);
    return 1;
}

BGD_DECLARE(void) gdTiffReadOptionsInit(gdTiffReadOptions *options)
{
    if (options == NULL) {
        return;
    }
    memset(options, 0, sizeof(*options));
}

BGD_DECLARE(gdTiffReadPtr) gdTiffReadOpen(FILE *fd, const gdTiffReadOptions *options)
{
    gdIOCtx *in;
    gdTiffReadPtr tiff;
    uint8_t *data;
    size_t size;

    if (fd == NULL || !TiffReadValidateOptions(options)) {
        return NULL;
    }
    in = gdNewFileCtx(fd);
    if (in == NULL) {
        return NULL;
    }
    data = TiffReadCtxData(in, &size);
    in->gd_free(in);
    if (data == NULL) {
        return NULL;
    }
    tiff = TiffReadOpenFromData(data, size);
    if (tiff == NULL) {
        gdFree(data);
    }
    return tiff;
}

BGD_DECLARE(gdTiffReadPtr) gdTiffReadOpenCtx(gdIOCtxPtr in, const gdTiffReadOptions *options)
{
    uint8_t *data;
    size_t size;
    gdTiffReadPtr tiff;

    if (in == NULL || !TiffReadValidateOptions(options)) {
        return NULL;
    }
    data = TiffReadCtxData(in, &size);
    if (data == NULL) {
        return NULL;
    }
    tiff = TiffReadOpenFromData(data, size);
    if (tiff == NULL) {
        gdFree(data);
    }
    return tiff;
}

BGD_DECLARE(gdTiffReadPtr) gdTiffReadOpenPtr(int size, void *data, const gdTiffReadOptions *options)
{
    uint8_t *buf;
    gdTiffReadPtr tiff;

    if (size <= 0 || data == NULL || !TiffReadValidateOptions(options)) {
        return NULL;
    }
    buf = (uint8_t *)gdMalloc((size_t)size);
    if (buf == NULL) {
        return NULL;
    }
    memcpy(buf, data, (size_t)size);
    tiff = TiffReadOpenFromData(buf, (size_t)size);
    if (tiff == NULL) {
        gdFree(buf);
    }
    return tiff;
}

BGD_DECLARE(void) gdTiffReadClose(gdTiffReadPtr tiff)
{
    if (tiff == NULL) {
        return;
    }
    if (tiff->tif != NULL) {
        TIFFClose(tiff->tif);
    }
    if (tiff->th != NULL) {
        gdFree(tiff->th);
    }
    if (tiff->memCtx != NULL) {
        tiff->memCtx->gd_free(tiff->memCtx);
    }
    if (tiff->data != NULL) {
        gdFree(tiff->data);
    }
    gdFree(tiff);
}

BGD_DECLARE(int) gdTiffReadGetInfo(gdTiffReadPtr tiff, gdTiffInfo *info)
{
    tdir_t savedDir;

    if (tiff == NULL || info == NULL || tiff->tif == NULL) {
        return 0;
    }
    savedDir = TIFFCurrentDirectory(tiff->tif);
    TIFFSetDirectory(tiff->tif, 0);
    TiffFillInfo(tiff->tif, info, tiff->pageCount);
    TIFFSetDirectory(tiff->tif, savedDir);
    return 1;
}

BGD_DECLARE(int) gdTiffReadGetMetadata(gdTiffReadPtr tiff, gdImageMetadata *metadata)
{
    tdir_t savedDir;
    int status;

    if (tiff == NULL || metadata == NULL || tiff->tif == NULL) return 0;
    savedDir = TIFFCurrentDirectory(tiff->tif);
    if (!TIFFSetDirectory(tiff->tif, 0)) return 0;
    status = gd_tiff_metadata_collect(tiff->tif, metadata);
    TIFFSetDirectory(tiff->tif, savedDir);
    return status == GD_META_OK;
}

BGD_DECLARE(int)
gdTiffReadNextImage(gdTiffReadPtr tiff, gdTiffPageInfo *info, gdImagePtr *image)
{
    gdImagePtr decoded;
    int ok;

    if (image != NULL) {
        *image = NULL;
    }
    if (tiff == NULL || tiff->tif == NULL || tiff->pageCount <= 0) {
        return -1;
    }

    if (tiff->currentPage >= tiff->pageCount - 1 && tiff->currentPage >= 0) {
        return 0;
    }

    if (tiff->currentPage < 0) {
        if (!TIFFSetDirectory(tiff->tif, 0)) {
            return 0;
        }
        tiff->currentPage = 0;
    } else {
        if (!TIFFReadDirectory(tiff->tif)) {
            return 0;
        }
        tiff->currentPage++;
    }

    TiffFillPageInfo(tiff->tif, info, tiff->currentPage);

    decoded = TiffDecodeCurrentDirectory(tiff->tif);
    if (decoded == NULL) {
        ok = TIFFSetDirectory(tiff->tif, TIFFCurrentDirectory(tiff->tif));
        (void)ok;
        return -1;
    }

    if (image != NULL) {
        *image = decoded;
    } else {
        gdImageDestroy(decoded);
    }
    return 1;
}

BGD_DECLARE(int) gdTiffIsMultiPage(FILE *fd)
{
    gdIOCtx *in;
    uint8_t *data;
    size_t size;
    gdIOCtx *memCtx;
    tiff_handle *th;
    TIFF *tif;
    tdir_t dirCount;
    int pos;

    if (fd == NULL) {
        return -1;
    }
    in = gdNewFileCtx(fd);
    if (in == NULL) {
        return -1;
    }
    pos = (int)gdTell(in);
    if (pos < 0) {
        in->gd_free(in);
        return -1;
    }
    data = TiffReadCtxData(in, &size);
    if (data == NULL) {
        in->gd_free(in);
        return -1;
    }
    memCtx = gdNewDynamicCtxEx((int)size, data, 0);
    if (memCtx == NULL) {
        gdFree(data);
        in->gd_free(in);
        return -1;
    }
    th = new_tiff_handle(memCtx, (int)size);
    if (th == NULL) {
        memCtx->gd_free(memCtx);
        gdFree(data);
        in->gd_free(in);
        return -1;
    }
    tif = TIFFClientOpen("", "rb", th, tiff_readproc, tiff_writeproc, tiff_seekproc, tiff_closeproc,
                         tiff_sizeproc, tiff_mapproc, tiff_unmapproc);
    if (tif == NULL) {
        gdFree(th);
        memCtx->gd_free(memCtx);
        gdFree(data);
        in->gd_free(in);
        return -1;
    }
    dirCount = TIFFNumberOfDirectories(tif);
    TIFFClose(tif);
    gdFree(th);
    memCtx->gd_free(memCtx);
    gdFree(data);
    if (!gdSeek(in, pos)) {
        in->gd_free(in);
        return -1;
    }
    in->gd_free(in);
    return dirCount > 1 ? 1 : 0;
}

BGD_DECLARE(int) gdTiffIsMultiPageCtx(gdIOCtxPtr in)
{
    uint8_t *data;
    size_t size;
    gdIOCtx *memCtx;
    tiff_handle *th;
    TIFF *tif;
    tdir_t dirCount;
    int pos;

    if (in == NULL || in->tell == NULL || in->seek == NULL) {
        return -1;
    }
    pos = (int)gdTell(in);
    if (pos < 0) {
        return -1;
    }
    data = TiffReadCtxData(in, &size);
    if (data == NULL) {
        return -1;
    }
    memCtx = gdNewDynamicCtxEx((int)size, data, 0);
    if (memCtx == NULL) {
        gdFree(data);
        return -1;
    }
    th = new_tiff_handle(memCtx, (int)size);
    if (th == NULL) {
        memCtx->gd_free(memCtx);
        gdFree(data);
        return -1;
    }
    tif = TIFFClientOpen("", "rb", th, tiff_readproc, tiff_writeproc, tiff_seekproc, tiff_closeproc,
                         tiff_sizeproc, tiff_mapproc, tiff_unmapproc);
    if (tif == NULL) {
        gdFree(th);
        memCtx->gd_free(memCtx);
        gdFree(data);
        return -1;
    }
    dirCount = TIFFNumberOfDirectories(tif);
    TIFFClose(tif);
    gdFree(th);
    memCtx->gd_free(memCtx);
    gdFree(data);
    if (!gdSeek(in, pos)) {
        return -1;
    }
    return dirCount > 1 ? 1 : 0;
}

BGD_DECLARE(int) gdTiffIsMultiPagePtr(int size, void *data)
{
    gdIOCtx *in;
    int result;

    if (size <= 0 || data == NULL) {
        return -1;
    }
    in = gdNewDynamicCtxEx(size, data, 0);
    if (in == NULL) {
        return -1;
    }
    result = gdTiffIsMultiPageCtx(in);
    in->gd_free(in);
    return result;
}

/* ========== TIFF Write API ========== */

struct gdTiffWriteStruct {
    gdIOCtx *out;
    int ownsCtx;
    int memoryWriter;
    TIFF *tif;
    tiff_handle *th;
    gdTiffWriteOptions options;
    int pageCount;
    int finalized;
};

static int TiffWriteValidateOptions(const gdTiffWriteOptions *opts)
{
    if (opts == NULL) {
        gd_error("gd-tiff write: options is NULL");
        return 0;
    }
    switch (opts->colorspace) {
    case GD_TIFF_RGB:
    case GD_TIFF_RGBA:
    case GD_TIFF_GRAY:
    case GD_TIFF_BILEVEL:
        break;
    default:
        gd_error("gd-tiff write: invalid colorspace %d", opts->colorspace);
        return 0;
    }
    switch (opts->bitDepth) {
    case 1:
    case 8:
    case 16:
        break;
    default:
        gd_error("gd-tiff write: invalid bit depth %d", opts->bitDepth);
        return 0;
    }
    if (opts->bitDepth == 1 && opts->colorspace != GD_TIFF_BILEVEL) {
        gd_error("gd-tiff write: 1-bit depth requires GD_TIFF_BILEVEL colorspace");
        return 0;
    }
    if (opts->colorspace == GD_TIFF_BILEVEL && opts->bitDepth != 1) {
        gd_error("gd-tiff write: GD_TIFF_BILEVEL requires 1-bit depth");
        return 0;
    }
    switch (opts->compression) {
    case GD_TIFF_COMPRESSION_NONE:
        break;
    case GD_TIFF_COMPRESSION_LZW:
    case GD_TIFF_COMPRESSION_ADOBE_DEFLATE:
    case GD_TIFF_COMPRESSION_DEFLATE:
    case GD_TIFF_COMPRESSION_PACKBITS:
        if (opts->bitDepth == 1 && opts->colorspace != GD_TIFF_BILEVEL) {
            gd_error("gd-tiff write: LZW/Deflate/PackBits at 1-bit requires "
                     "BILEVEL");
            return 0;
        }
        break;
    case GD_TIFF_COMPRESSION_JPEG:
        if (opts->bitDepth != 8) {
            gd_error("gd-tiff write: JPEG compression requires 8-bit depth");
            return 0;
        }
        break;
    case GD_TIFF_COMPRESSION_CCITT_FAX3:
    case GD_TIFF_COMPRESSION_CCITT_FAX4:
        if (opts->bitDepth != 1) {
            gd_error("gd-tiff write: CCITT compression requires 1-bit depth");
            return 0;
        }
        break;
    default:
        gd_error("gd-tiff write: unsupported compression %d", opts->compression);
        return 0;
    }
    if (opts->colorspace == GD_TIFF_RGB && opts->bitDepth == 1) {
        gd_error("gd-tiff write: RGB at 1-bit is not valid");
        return 0;
    }
    if (opts->colorspace == GD_TIFF_RGBA && opts->bitDepth == 1) {
        gd_error("gd-tiff write: RGBA at 1-bit is not valid");
        return 0;
    }
    return 1;
}

static void TiffWriteSetDefaults(gdTiffWriteOptions *opts)
{
    if (opts->bitDepth == 0)
        opts->bitDepth = 8;
    if (opts->colorspace == 0)
        opts->colorspace = GD_TIFF_RGBA;
    if (opts->compression == 0)
        opts->compression = GD_TIFF_COMPRESSION_ADOBE_DEFLATE;
    if (opts->jpegQuality == 0 && opts->compression == GD_TIFF_COMPRESSION_JPEG)
        opts->jpegQuality = 75;
    if (opts->resolutionUnit == 0)
        opts->resolutionUnit = GD_TIFF_RESUNIT_INCH;
    if (opts->xResolution == 0)
        opts->xResolution = 72.0f;
    if (opts->yResolution == 0)
        opts->yResolution = 72.0f;
    if (opts->alphaType == 0)
        opts->alphaType = GD_TIFF_ALPHA_UNASSOCIATED;
}

BGD_DECLARE(void) gdTiffWriteOptionsInit(gdTiffWriteOptions *options)
{
    if (options == NULL) {
        return;
    }
    memset(options, 0, sizeof(*options));
    TiffWriteSetDefaults(options);
}

static int TiffWriteCopyOptions(gdTiffWriteOptions *dst, const gdTiffWriteOptions *src)
{
    memset(dst, 0, sizeof(*dst));
    if (src != NULL) {
        *dst = *src;
    }
    TiffWriteSetDefaults(dst);
    return 1;
}

static int TiffWriteSamplesPerPixel(const gdTiffWriteOptions *opts)
{
    switch (opts->colorspace) {
    case GD_TIFF_RGBA:
        return 4;
    case GD_TIFF_RGB:
        return 3;
    case GD_TIFF_GRAY:
    case GD_TIFF_BILEVEL:
        return 1;
    default:
        return 3;
    }
}

static int TiffWritePhotometric(const gdTiffWriteOptions *opts)
{
    switch (opts->colorspace) {
    case GD_TIFF_RGB:
    case GD_TIFF_RGBA:
        return GD_TIFF_PHOTOMETRIC_RGB;
    case GD_TIFF_GRAY:
        return opts->minIsWhite ? GD_TIFF_PHOTOMETRIC_MINISWHITE : GD_TIFF_PHOTOMETRIC_MINISBLACK;
    case GD_TIFF_BILEVEL:
        return opts->minIsWhite ? GD_TIFF_PHOTOMETRIC_MINISWHITE : GD_TIFF_PHOTOMETRIC_MINISBLACK;
    default:
        return GD_TIFF_PHOTOMETRIC_RGB;
    }
}

static int TiffWriteBitsPerSample(const gdTiffWriteOptions *opts)
{
    if (opts->colorspace == GD_TIFF_BILEVEL)
        return 1;
    return opts->bitDepth;
}

static void TiffWriteConvertRowRGBA8(gdImagePtr im, int y, uint8_t *buf, int width)
{
    int x;
    for (x = 0; x < width; x++) {
        int c = im->tpixels[y][x];
        int r = gdImageRed(im, c);
        int g = gdImageGreen(im, c);
        int b = gdImageBlue(im, c);
        int a = gdImageAlpha(im, c);
        a = (127 - a) * 2;
        if (a > 255)
            a = 255;
        if (a == 254)
            a = 255;
        buf[x * 4 + 0] = (uint8_t)r;
        buf[x * 4 + 1] = (uint8_t)g;
        buf[x * 4 + 2] = (uint8_t)b;
        buf[x * 4 + 3] = (uint8_t)a;
    }
}

static void TiffWriteConvertRowRGB8(gdImagePtr im, int y, uint8_t *buf, int width)
{
    int x;
    for (x = 0; x < width; x++) {
        int c = im->tpixels[y][x];
        int r = gdImageRed(im, c);
        int g = gdImageGreen(im, c);
        int b = gdImageBlue(im, c);
        int a = gdImageAlpha(im, c);
        a = (127 - a) * 2;
        if (a > 255)
            a = 255;
        if (a == 254)
            a = 255;
        if (a < 255) {
            int af = a + 1;
            r = (r * af + 127) / 255;
            g = (g * af + 127) / 255;
            b = (b * af + 127) / 255;
        }
        buf[x * 3 + 0] = (uint8_t)r;
        buf[x * 3 + 1] = (uint8_t)g;
        buf[x * 3 + 2] = (uint8_t)b;
    }
}

static void TiffWriteConvertRowGray8(gdImagePtr im, int y, uint8_t *buf, int width, int minIsWhite)
{
    int x;
    for (x = 0; x < width; x++) {
        int c = im->tpixels[y][x];
        int r = gdImageRed(im, c);
        int g = gdImageGreen(im, c);
        int b = gdImageBlue(im, c);
        int gray = (int)(0.2126f * r + 0.7152f * g + 0.0722f * b + 0.5f);
        if (gray > 255)
            gray = 255;
        if (minIsWhite)
            gray = 255 - gray;
        buf[x] = (uint8_t)gray;
    }
}

static void TiffWriteConvertRowBilevel(gdImagePtr im, int y, uint8_t *buf, int width,
                                       int minIsWhite)
{
    int x;
    memset(buf, 0, (width + 7) / 8);
    for (x = 0; x < width; x++) {
        int c = im->tpixels[y][x];
        int r = gdImageRed(im, c);
        int g = gdImageGreen(im, c);
        int b = gdImageBlue(im, c);
        int gray = (int)(0.2126f * r + 0.7152f * g + 0.0722f * b + 0.5f);
        int bit;
        if (minIsWhite)
            bit = (gray < 128) ? 1 : 0;
        else
            bit = (gray >= 128) ? 1 : 0;
        if (bit)
            buf[x / 8] |= (uint8_t)(0x80 >> (x & 7));
    }
}

static void TiffWriteConvertRowRGBA16(gdImagePtr im, int y, uint16_t *buf, int width)
{
    int x;
    for (x = 0; x < width; x++) {
        int c = im->tpixels[y][x];
        int r = gdImageRed(im, c);
        int g = gdImageGreen(im, c);
        int b = gdImageBlue(im, c);
        int a = gdImageAlpha(im, c);
        a = (127 - a) * 2;
        if (a > 255)
            a = 255;
        if (a == 254)
            a = 255;
        buf[x * 4 + 0] = (uint16_t)(r * 257);
        buf[x * 4 + 1] = (uint16_t)(g * 257);
        buf[x * 4 + 2] = (uint16_t)(b * 257);
        buf[x * 4 + 3] = (uint16_t)(a * 257);
    }
}

static void TiffWriteConvertRowRGB16(gdImagePtr im, int y, uint16_t *buf, int width)
{
    int x;
    for (x = 0; x < width; x++) {
        int c = im->tpixels[y][x];
        int r = gdImageRed(im, c);
        int g = gdImageGreen(im, c);
        int b = gdImageBlue(im, c);
        int a = gdImageAlpha(im, c);
        a = (127 - a) * 2;
        if (a > 255)
            a = 255;
        if (a == 254)
            a = 255;
        if (a < 255) {
            int af = a + 1;
            r = (r * af + 127) / 255;
            g = (g * af + 127) / 255;
            b = (b * af + 127) / 255;
        }
        buf[x * 3 + 0] = (uint16_t)(r * 257);
        buf[x * 3 + 1] = (uint16_t)(g * 257);
        buf[x * 3 + 2] = (uint16_t)(b * 257);
    }
}

static void TiffWriteConvertRowGray16(gdImagePtr im, int y, uint16_t *buf, int width,
                                      int minIsWhite)
{
    int x;
    for (x = 0; x < width; x++) {
        int c = im->tpixels[y][x];
        int r = gdImageRed(im, c);
        int g = gdImageGreen(im, c);
        int b = gdImageBlue(im, c);
        int gray = (int)(0.2126f * r + 0.7152f * g + 0.0722f * b + 0.5f);
        if (gray > 255)
            gray = 255;
        if (minIsWhite)
            gray = 255 - gray;
        buf[x] = (uint16_t)(gray * 257);
    }
}

static int TiffWriteWritePage(gdTiffWritePtr write, gdImagePtr im)
{
    TIFF *tif = write->tif;
    gdTiffWriteOptions *opts = &write->options;
    int width, height;
    int nsamples, bps, photometric;
    int y;
    uint8_t *scanbuf = NULL;

    width = gdImageSX(im);
    height = gdImageSY(im);
    nsamples = TiffWriteSamplesPerPixel(opts);
    bps = TiffWriteBitsPerSample(opts);
    photometric = TiffWritePhotometric(opts);

    TIFFSetField(tif, TIFFTAG_IMAGEWIDTH, width);
    TIFFSetField(tif, TIFFTAG_IMAGELENGTH, height);
    TIFFSetField(tif, TIFFTAG_BITSPERSAMPLE, bps);
    TIFFSetField(tif, TIFFTAG_SAMPLESPERPIXEL, nsamples);
    TIFFSetField(tif, TIFFTAG_PHOTOMETRIC, photometric);
    TIFFSetField(tif, TIFFTAG_COMPRESSION, opts->compression);
    TIFFSetField(tif, TIFFTAG_PLANARCONFIG, GD_TIFF_PLANARCONFIG_CONTIG);
    TIFFSetField(tif, TIFFTAG_ROWSPERSTRIP, TIFFDefaultStripSize(tif, 0));

    if (opts->colorspace == GD_TIFF_RGBA) {
        uint16_t extra = (opts->alphaType == GD_TIFF_ALPHA_ASSOCIATED) ? EXTRASAMPLE_ASSOCALPHA
                                                                       : EXTRASAMPLE_UNASSALPHA;
        TIFFSetField(tif, TIFFTAG_EXTRASAMPLES, 1, &extra);
    }

    if (opts->compression == GD_TIFF_COMPRESSION_LZW || opts->compression == GD_TIFF_COMPRESSION_ADOBE_DEFLATE ||
        opts->compression == GD_TIFF_COMPRESSION_DEFLATE) {
        TIFFSetField(tif, TIFFTAG_PREDICTOR, PREDICTOR_HORIZONTAL);
    }

    if (opts->compression == GD_TIFF_COMPRESSION_JPEG && opts->jpegQuality > 0) {
        TIFFSetField(tif, TIFFTAG_JPEGQUALITY, opts->jpegQuality);
    }

    if (opts->colorspace == GD_TIFF_BILEVEL && (opts->compression == GD_TIFF_COMPRESSION_CCITT_FAX3 ||
                                                opts->compression == GD_TIFF_COMPRESSION_CCITT_FAX4)) {
        uint32_t g3opts = 0;
        if (opts->compression == GD_TIFF_COMPRESSION_CCITT_FAX3) {
            TIFFSetField(tif, TIFFTAG_GROUP3OPTIONS, g3opts);
        }
        if (opts->compression == GD_TIFF_COMPRESSION_CCITT_FAX4) {
            TIFFSetField(tif, TIFFTAG_GROUP4OPTIONS, 0);
        }
        TIFFSetField(tif, TIFFTAG_FILLORDER, FILLORDER_MSB2LSB);
    }

    {
        uint16_t resUnit;
        switch (opts->resolutionUnit) {
        case GD_TIFF_RESUNIT_NONE:
            resUnit = RESUNIT_NONE;
            break;
        case GD_TIFF_RESUNIT_INCH:
            resUnit = RESUNIT_INCH;
            break;
        case GD_TIFF_RESUNIT_CENTIMETER:
            resUnit = RESUNIT_CENTIMETER;
            break;
        default:
            resUnit = RESUNIT_INCH;
            break;
        }
        TIFFSetField(tif, TIFFTAG_RESOLUTIONUNIT, resUnit);
        TIFFSetField(tif, TIFFTAG_XRESOLUTION, opts->xResolution);
        TIFFSetField(tif, TIFFTAG_YRESOLUTION, opts->yResolution);
    }

    if (gd_tiff_metadata_apply(tif, opts->metadata) != GD_META_OK) {
        gd_error("gd-tiff write: unable to apply metadata");
        return 0;
    }

    if (opts->colorspace == GD_TIFF_BILEVEL) {
        size_t scanline_size = (size_t)((width + 7) / 8);
        if (opts->compression == GD_TIFF_COMPRESSION_CCITT_FAX3 ||
            opts->compression == GD_TIFF_COMPRESSION_CCITT_FAX4) {
            scanline_size = (size_t)TIFFScanlineSize(tif);
            if (scanline_size == 0)
                scanline_size = (size_t)((width + 7) / 8);
        }
        scanbuf = (uint8_t *)gdMalloc(scanline_size);
        if (scanbuf == NULL) {
            gd_error("gd-tiff write: could not allocate scanline buffer");
            return 0;
        }
        for (y = 0; y < height; y++) {
            TiffWriteConvertRowBilevel(im, y, scanbuf, width, opts->minIsWhite);
            if (TIFFWriteScanline(tif, scanbuf, y, 0) < 0) {
                gdFree(scanbuf);
                gd_error("gd-tiff write: could not write scanline");
                return 0;
            }
        }
    } else if (opts->bitDepth == 16) {
        size_t scanline_size = (size_t)TIFFScanlineSize(tif);
        scanbuf = (uint8_t *)gdMalloc(scanline_size);
        if (scanbuf == NULL) {
            gd_error("gd-tiff write: could not allocate scanline buffer");
            return 0;
        }
        for (y = 0; y < height; y++) {
            uint16_t *buf16 = (uint16_t *)scanbuf;
            switch (opts->colorspace) {
            case GD_TIFF_RGBA:
                TiffWriteConvertRowRGBA16(im, y, buf16, width);
                break;
            case GD_TIFF_RGB:
                TiffWriteConvertRowRGB16(im, y, buf16, width);
                break;
            case GD_TIFF_GRAY:
                TiffWriteConvertRowGray16(im, y, buf16, width, opts->minIsWhite);
                break;
            default:
                break;
            }
            if (TIFFWriteScanline(tif, scanbuf, y, 0) < 0) {
                gdFree(scanbuf);
                gd_error("gd-tiff write: could not write scanline");
                return 0;
            }
        }
    } else {
        size_t scanline_size = (size_t)TIFFScanlineSize(tif);
        scanbuf = (uint8_t *)gdMalloc(scanline_size);
        if (scanbuf == NULL) {
            gd_error("gd-tiff write: could not allocate scanline buffer");
            return 0;
        }
        for (y = 0; y < height; y++) {
            switch (opts->colorspace) {
            case GD_TIFF_RGBA:
                TiffWriteConvertRowRGBA8(im, y, scanbuf, width);
                break;
            case GD_TIFF_RGB:
                TiffWriteConvertRowRGB8(im, y, scanbuf, width);
                break;
            case GD_TIFF_GRAY:
                TiffWriteConvertRowGray8(im, y, scanbuf, width, opts->minIsWhite);
                break;
            default:
                break;
            }
            if (TIFFWriteScanline(tif, scanbuf, y, 0) < 0) {
                gdFree(scanbuf);
                gd_error("gd-tiff write: could not write scanline");
                return 0;
            }
        }
    }

    gdFree(scanbuf);

    if (!TIFFWriteDirectory(tif)) {
        gd_error("gd-tiff write: could not write directory for page %d", write->pageCount);
        return 0;
    }

    write->pageCount++;
    return 1;
}

static void TiffWriteFree(gdTiffWritePtr write)
{
    if (write == NULL)
        return;
    if (write->tif) {
        TIFFClose(write->tif);
        write->tif = NULL;
    }
    if (write->th) {
        gdFree(write->th);
        write->th = NULL;
    }
    if (write->ownsCtx && write->out) {
        write->out->gd_free(write->out);
        write->out = NULL;
    }
    gdFree(write);
}

BGD_DECLARE(gdTiffWritePtr)
gdTiffWriteOpen(FILE *outFile, const gdTiffWriteOptions *options)
{
    gdIOCtx *out;
    gdTiffWritePtr write;

    if (outFile == NULL)
        return NULL;
    out = gdNewFileCtx(outFile);
    if (out == NULL)
        return NULL;
    write = gdTiffWriteOpenCtx(out, options);
    if (write == NULL) {
        out->gd_free(out);
        return NULL;
    }
    write->ownsCtx = 1;
    return write;
}

BGD_DECLARE(gdTiffWritePtr)
gdTiffWriteOpenCtx(gdIOCtxPtr out, const gdTiffWriteOptions *options)
{
    gdTiffWritePtr write;
    tiff_handle *th;
    TIFF *tif;

    gd_tiff_metadata_register_fields();

    if (out == NULL)
        return NULL;

    write = (gdTiffWritePtr)gdCalloc(1, sizeof(struct gdTiffWriteStruct));
    if (write == NULL)
        return NULL;

    write->out = out;
    write->ownsCtx = 0;

    if (!TiffWriteCopyOptions(&write->options, options)) {
        gdFree(write);
        return NULL;
    }

    if (!TiffWriteValidateOptions(&write->options)) {
        gdFree(write);
        return NULL;
    }

    th = new_tiff_handle(out, 0);
    if (th == NULL) {
        gdFree(write);
        return NULL;
    }
    write->th = th;

    tif = TIFFClientOpen("", "w", th, tiff_readproc, tiff_writeproc, tiff_seekproc, tiff_closeproc,
                         tiff_sizeproc, tiff_mapproc, tiff_unmapproc);
    if (tif == NULL) {
        gdFree(th);
        gdFree(write);
        gd_error("gd-tiff write: could not open TIFF for writing");
        return NULL;
    }
    write->tif = tif;

    return write;
}

BGD_DECLARE(gdTiffWritePtr)
gdTiffWriteOpenPtr(const gdTiffWriteOptions *options)
{
    gdIOCtx *out;
    gdTiffWritePtr write;

    out = gdNewDynamicCtx(2048, NULL);
    if (out == NULL)
        return NULL;
    write = gdTiffWriteOpenCtx(out, options);
    if (write == NULL) {
        out->gd_free(out);
        return NULL;
    }
    write->ownsCtx = 1;
    write->memoryWriter = 1;
    return write;
}

BGD_DECLARE(int) gdTiffWriteAddImage(gdTiffWritePtr write, gdImagePtr image)
{
    if (write == NULL || image == NULL || write->finalized)
        return 0;
    if (!image->trueColor) {
        gd_error("gd-tiff write: only truecolor images are supported by the "
                 "new write API");
        return 0;
    }
    return TiffWriteWritePage(write, image);
}

BGD_DECLARE(void) gdTiffWriteClose(gdTiffWritePtr write)
{
    if (write == NULL)
        return;
    write->finalized = 1;
    TiffWriteFree(write);
}

BGD_DECLARE(void *) gdTiffWritePtrFinish(gdTiffWritePtr write, int *size)
{
    void *rv = NULL;

    if (size != NULL)
        *size = 0;
    if (write == NULL || !write->memoryWriter) {
        TiffWriteFree(write);
        return NULL;
    }
    write->finalized = 1;
    if (write->tif != NULL) {
        TIFFClose(write->tif);
        write->tif = NULL;
    }
    if (write->out != NULL) {
        rv = gdDPExtractData(write->out, size);
    }
    TiffWriteFree(write);
    return rv;
}

#else

static void _noTiffError(void) { gd_error("TIFF image support has been disabled\n"); }

BGD_DECLARE(void) gdImageTiffCtx(gdImagePtr image, gdIOCtx *out)
{
    ARG_NOT_USED(image);
    ARG_NOT_USED(out);
    _noTiffError();
}

BGD_DECLARE(gdImagePtr) gdImageCreateFromTiffCtx(gdIOCtx *infile)
{
    ARG_NOT_USED(infile);
    _noTiffError();
    return NULL;
}

BGD_DECLARE(gdImagePtr) gdImageCreateFromTiff(FILE *inFile)
{
    ARG_NOT_USED(inFile);
    _noTiffError();
    return NULL;
}

BGD_DECLARE(gdImagePtr) gdImageCreateFromTiffPtr(int size, void *data)
{
    ARG_NOT_USED(size);
    ARG_NOT_USED(data);
    _noTiffError();
    return NULL;
}

BGD_DECLARE(void) gdImageTiff(gdImagePtr im, FILE *outFile)
{
    ARG_NOT_USED(im);
    ARG_NOT_USED(outFile);
    _noTiffError();
}

BGD_DECLARE(void *) gdImageTiffPtr(gdImagePtr im, int *size)
{
    ARG_NOT_USED(im);
    ARG_NOT_USED(size);
    _noTiffError();
    return NULL;
}

BGD_DECLARE(void) gdTiffReadOptionsInit(gdTiffReadOptions *options)
{
    if (options == NULL) {
        return;
    }
    memset(options, 0, sizeof(*options));
}

BGD_DECLARE(gdTiffReadPtr) gdTiffReadOpen(FILE *fd, const gdTiffReadOptions *options)
{
    ARG_NOT_USED(fd);
    ARG_NOT_USED(options);
    _noTiffError();
    return NULL;
}

BGD_DECLARE(gdTiffReadPtr) gdTiffReadOpenCtx(gdIOCtxPtr in, const gdTiffReadOptions *options)
{
    ARG_NOT_USED(in);
    ARG_NOT_USED(options);
    _noTiffError();
    return NULL;
}

BGD_DECLARE(gdTiffReadPtr) gdTiffReadOpenPtr(int size, void *data, const gdTiffReadOptions *options)
{
    ARG_NOT_USED(size);
    ARG_NOT_USED(data);
    ARG_NOT_USED(options);
    _noTiffError();
    return NULL;
}

BGD_DECLARE(void) gdTiffReadClose(gdTiffReadPtr tiff)
{
    ARG_NOT_USED(tiff);
    _noTiffError();
}

BGD_DECLARE(int) gdTiffReadGetInfo(gdTiffReadPtr tiff, gdTiffInfo *info)
{
    ARG_NOT_USED(tiff);
    ARG_NOT_USED(info);
    _noTiffError();
    return 0;
}

BGD_DECLARE(int)
gdTiffReadNextImage(gdTiffReadPtr tiff, gdTiffPageInfo *info, gdImagePtr *image)
{
    ARG_NOT_USED(tiff);
    ARG_NOT_USED(info);
    ARG_NOT_USED(image);
    _noTiffError();
    return -1;
}

BGD_DECLARE(int) gdTiffIsMultiPage(FILE *fd)
{
    ARG_NOT_USED(fd);
    _noTiffError();
    return -1;
}

BGD_DECLARE(int) gdTiffIsMultiPageCtx(gdIOCtxPtr in)
{
    ARG_NOT_USED(in);
    _noTiffError();
    return -1;
}

BGD_DECLARE(int) gdTiffIsMultiPagePtr(int size, void *data)
{
    ARG_NOT_USED(size);
    ARG_NOT_USED(data);
    _noTiffError();
    return -1;
}

BGD_DECLARE(void) gdTiffWriteOptionsInit(gdTiffWriteOptions *options)
{
    if (options == NULL) {
        return;
    }
    memset(options, 0, sizeof(*options));
    options->bitDepth = 8;
    options->colorspace = GD_TIFF_RGBA;
    options->compression = GD_TIFF_COMPRESSION_ADOBE_DEFLATE;
    options->resolutionUnit = GD_TIFF_RESUNIT_INCH;
    options->xResolution = 72.0f;
    options->yResolution = 72.0f;
    options->alphaType = GD_TIFF_ALPHA_UNASSOCIATED;
}

BGD_DECLARE(gdTiffWritePtr)
gdTiffWriteOpen(FILE *outFile, const gdTiffWriteOptions *options)
{
    ARG_NOT_USED(outFile);
    ARG_NOT_USED(options);
    _noTiffError();
    return NULL;
}

BGD_DECLARE(gdTiffWritePtr)
gdTiffWriteOpenCtx(gdIOCtxPtr out, const gdTiffWriteOptions *options)
{
    ARG_NOT_USED(out);
    ARG_NOT_USED(options);
    _noTiffError();
    return NULL;
}

BGD_DECLARE(gdTiffWritePtr)
gdTiffWriteOpenPtr(const gdTiffWriteOptions *options)
{
    ARG_NOT_USED(options);
    _noTiffError();
    return NULL;
}

BGD_DECLARE(int) gdTiffWriteAddImage(gdTiffWritePtr write, gdImagePtr image)
{
    ARG_NOT_USED(write);
    ARG_NOT_USED(image);
    _noTiffError();
    return 0;
}

BGD_DECLARE(void) gdTiffWriteClose(gdTiffWritePtr write)
{
    ARG_NOT_USED(write);
    _noTiffError();
}

BGD_DECLARE(void *) gdTiffWritePtrFinish(gdTiffWritePtr write, int *size)
{
    ARG_NOT_USED(write);
    ARG_NOT_USED(size);
    _noTiffError();
    return NULL;
}

#endif
