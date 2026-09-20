/**
 * File: JPEG XL IO
 *
 * Read and write JPEG XL images.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif /* HAVE_CONFIG_H */

#include "gd.h"
#include "gd_errors.h"
#include "gd_intern.h"
#include "gdhelpers.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef HAVE_LIBJXL
#include <jxl/cms.h>
#include <jxl/color_encoding.h>
#include <jxl/decode.h>
#include <jxl/encode.h>

#define GD_JXL_ALLOC_STEP (4 * 1024)

/* ---- Internal helpers ---- */

/* Slurp gdIOCtx into dynamic buffer (pattern from gd_webp.c) */
static uint8_t *JxlReadCtxData(gdIOCtx *infile, size_t *size)
{
    uint8_t *filedata = NULL, *temp, *read;
    ssize_t n;

    *size = 0;
    do {
        temp = gdRealloc(filedata, *size + GD_JXL_ALLOC_STEP);
        if (temp == NULL) {
            gdFree(filedata);
            gd_error("JXL decode: realloc failed");
            return NULL;
        }
        filedata = temp;
        read = temp + *size;
        n = gdGetBuf(read, GD_JXL_ALLOC_STEP, infile);
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

/* JXL alpha (0-255) -> gd alpha (0-127, 0=opaque) */
static int JxlAlphaJxlToGd(uint8_t jxl_alpha)
{
    if (jxl_alpha == 0) {
        return gdAlphaTransparent;
    }
    return gdAlphaMax - (jxl_alpha >> 1);
}

/* gd alpha (0-127) -> JXL alpha (0-255) */
static uint8_t JxlAlphaGdToJxl(int gd_alpha)
{
    if (gd_alpha == gdAlphaTransparent) {
        return 0;
    }
    return (uint8_t)((gdAlphaMax - gd_alpha) << 1);
}

static int JxlImageHasAlpha(gdImagePtr im)
{
    int x, y;

    if (im == NULL) {
        return 0;
    }

    for (y = 0; y < gdImageSY(im); y++) {
        for (x = 0; x < gdImageSX(im); x++) {
            if (gdTrueColorGetAlpha(im->tpixels[y][x]) != gdAlphaOpaque) {
                return 1;
            }
        }
    }

    return 0;
}

static int JxlDurationToMs(uint32_t duration, const JxlAnimationHeader *animation)
{
    if (animation == NULL || animation->tps_numerator == 0) {
        return 0;
    }

    return (int)((uint64_t)duration * 1000 * animation->tps_denominator / animation->tps_numerator);
}

/* Build gdImagePtr from RGBA u8 buffer */
static gdImagePtr JxlImageFromRGBA(const uint8_t *rgba, int width, int height, int has_alpha)
{
    gdImagePtr im;
    const uint8_t *p;
    int x, y;

    if (rgba == NULL || width <= 0 || height <= 0) {
        return NULL;
    }
    im = gdImageCreateTrueColor(width, height);
    if (im == NULL) {
        return NULL;
    }
    gdImageAlphaBlending(im, 0);
    gdImageSaveAlpha(im, has_alpha);
    for (y = 0, p = rgba; y < height; y++) {
        for (x = 0; x < width; x++) {
            uint8_t r = *(p++);
            uint8_t g = *(p++);
            uint8_t b = *(p++);
            uint8_t a = *(p++);
            im->tpixels[y][x] = gdTrueColorAlpha(r, g, b, JxlAlphaJxlToGd(a));
        }
    }
    return im;
}

/* Extract RGBA u8 buffer from gdImagePtr */
static int JxlImageToRGBA(gdImagePtr im, uint8_t **rgba, int *has_alpha)
{
    uint8_t *p;
    int x, y;
    int w, h;

    *rgba = NULL;
    *has_alpha = 0;

    if (im == NULL) {
        return 0;
    }

    /* Convert palette to truecolor if needed */
    if (!gdImageTrueColor(im)) {
        if (!gdImagePaletteToTrueColor(im)) {
            return 0;
        }
    }

    w = gdImageSX(im);
    h = gdImageSY(im);

    /* Check for alpha presence */
    if (gdImageGetTransparent(im) != -1 || JxlImageHasAlpha(im)) {
        *has_alpha = 1;
    }

    /* Overflow checks */
    if (overflow2(w, 4)) {
        return 0;
    }
    if (overflow2(w * 4, h)) {
        return 0;
    }

    *rgba = (uint8_t *)gdMalloc((size_t)w * 4 * (size_t)h);
    if (*rgba == NULL) {
        return 0;
    }

    p = *rgba;
    for (y = 0; y < h; y++) {
        for (x = 0; x < w; x++) {
            int c = im->tpixels[y][x];
            *(p++) = gdTrueColorGetRed(c);
            *(p++) = gdTrueColorGetGreen(c);
            *(p++) = gdTrueColorGetBlue(c);
            *(p++) = JxlAlphaGdToJxl(gdTrueColorGetAlpha(c));
        }
    }
    return 1;
}

static uint8_t *JxlRGBFromRGBA(const uint8_t *rgba, int width, int height)
{
    uint8_t *rgb, *dst;
    const uint8_t *src;
    int x, y;

    if (overflow2(width, 3) || overflow2(width * 3, height)) {
        return NULL;
    }

    rgb = (uint8_t *)gdMalloc((size_t)width * 3 * (size_t)height);
    if (rgb == NULL) {
        return NULL;
    }

    src = rgba;
    dst = rgb;
    for (y = 0; y < height; y++) {
        for (x = 0; x < width; x++) {
            *(dst++) = *(src++);
            *(dst++) = *(src++);
            *(dst++) = *(src++);
            src++;
        }
    }

    return rgb;
}

/* ---- Still-image decode ---- */

BGD_DECLARE(gdImagePtr) gdImageCreateFromJxl(FILE *inFile)
{
    gdImagePtr im;
    gdIOCtx *in = gdNewFileCtx(inFile);
    if (!in) {
        return 0;
    }
    im = gdImageCreateFromJxlCtx(in);
    in->gd_free(in);
    return im;
}

BGD_DECLARE(gdImagePtr) gdImageCreateFromJxlPtr(int size, void *data)
{
    gdImagePtr im;
    gdIOCtx *in = gdNewDynamicCtxEx(size, data, 0);
    if (!in) {
        return 0;
    }
    im = gdImageCreateFromJxlCtx(in);
    in->gd_free(in);
    return im;
}

BGD_DECLARE(gdImagePtr) gdImageCreateFromJxlCtx(gdIOCtxPtr inCtx)
{
    size_t buf_len = 0;
    uint8_t *buf = NULL;
    JxlDecoder *dec = NULL;
    JxlBasicInfo info = {0};
    uint8_t *pixels = NULL;
    gdImagePtr im = NULL;
    int has_alpha = 0;
    int have_basic_info = 0;

    if (inCtx == NULL) {
        return NULL;
    }

    buf = JxlReadCtxData(inCtx, &buf_len);
    if (buf == NULL) {
        return NULL;
    }

    dec = JxlDecoderCreate(NULL);
    if (dec == NULL) {
        gd_error("gdImageCreateFromJxl: JxlDecoderCreate failed");
        gdFree(buf);
        return NULL;
    }

    JxlDecoderSubscribeEvents(dec,
                              JXL_DEC_BASIC_INFO | JXL_DEC_COLOR_ENCODING | JXL_DEC_FULL_IMAGE);

    /* Attach CMS for color space conversion to sRGB */
    JxlDecoderSetCms(dec, *JxlGetDefaultCms());

    /* Set desired output color profile (sRGB) */
    JxlColorEncoding srgb_enc = {0};
    JxlColorEncodingSetToSRGB(&srgb_enc, JXL_FALSE);
    JxlDecoderSetPreferredColorProfile(dec, &srgb_enc);

    JxlDecoderSetInput(dec, buf, buf_len);
    JxlDecoderCloseInput(dec);

    for (;;) {
        JxlDecoderStatus status = JxlDecoderProcessInput(dec);

        if (status == JXL_DEC_BASIC_INFO) {
            status = JxlDecoderGetBasicInfo(dec, &info);
            if (status != JXL_DEC_SUCCESS) {
                gd_error("gdImageCreateFromJxl: failed to get basic info");
                goto decode_fail;
            }
            have_basic_info = 1;
            has_alpha = (info.alpha_bits > 0);
        } else if (status == JXL_DEC_NEED_IMAGE_OUT_BUFFER) {
            JxlPixelFormat fmt = {.num_channels = 4, /* RGBA */
                                  .data_type = JXL_TYPE_UINT8,
                                  .endianness = JXL_NATIVE_ENDIAN,
                                  .align = 0};
            size_t pixels_size;

            if (!have_basic_info) {
                gd_error("gdImageCreateFromJxl: missing basic info");
                goto decode_fail;
            }
            if (overflow2((int)info.xsize, (int)info.ysize) ||
                overflow2((int)info.xsize * (int)info.ysize, 4)) {
                gd_error("gdImageCreateFromJxl: image dimensions overflow");
                goto decode_fail;
            }

            pixels_size = (size_t)info.xsize * (size_t)info.ysize * 4;
            pixels = (uint8_t *)gdMalloc(pixels_size);
            if (pixels == NULL) {
                gd_error("gdImageCreateFromJxl: pixel buffer allocation failed");
                goto decode_fail;
            }

            if (JxlDecoderSetImageOutBuffer(dec, &fmt, pixels, pixels_size) != JXL_DEC_SUCCESS) {
                gd_error("gdImageCreateFromJxl: failed to set output buffer");
                goto decode_fail;
            }
        } else if (status == JXL_DEC_FULL_IMAGE) {
            if (!have_basic_info || pixels == NULL) {
                gd_error("gdImageCreateFromJxl: incomplete decoded image");
                goto decode_fail;
            }
            break; /* pixels buffer is populated */
        } else if (status == JXL_DEC_SUCCESS || status == JXL_DEC_ERROR) {
            gd_error("gdImageCreateFromJxl: decoder error");
            goto decode_fail;
        }
    }

    /* Build gdImagePtr from RGBA buffer */
    im = JxlImageFromRGBA(pixels, (int)info.xsize, (int)info.ysize, has_alpha);
    if (!im) {
        gd_error("gdImageCreateFromJxl: JxlImageFromRGBA failed");
        goto decode_fail;
    }

    /* Success path - pixels consumed by im */
    gdFree(pixels);
    pixels = NULL;
    gdFree(buf);
    buf = NULL;
    JxlDecoderDestroy(dec);
    return im;

decode_fail:
    if (pixels)
        gdFree(pixels);
    if (buf)
        gdFree(buf);
    if (dec)
        JxlDecoderDestroy(dec);
    return NULL;
}

/* ---- Still-image encode ---- */

static int JxlEncoderAddMetadata(JxlEncoder *enc, const gdImageMetadata *metadata)
{
    size_t i;
    if (metadata == NULL) {
        return 1;
    }

    for (i = 0; i < gdImageMetadataGetProfileCount(metadata); i++) {
        const char *key;
        const unsigned char *data;
        size_t size;
        JxlBoxType type;
        uint8_t *exif = NULL;
        const uint8_t *contents;
        size_t contents_size;

        if (gdImageMetadataGetProfileAt(metadata, i, &key, &data, &size) != GD_META_OK) {
            continue;
        }
        if (strcmp(key, "exif") == 0) {
            if (gdMetadataGetExifTiff(data, size, &data, &size) != GD_META_OK ||
                size > SIZE_MAX - 4) {
                return 0;
            }
            exif = gdMalloc(size + 4);
            if (exif == NULL) {
                return 0;
            }
            exif[0] = 0;
            exif[1] = 0;
            exif[2] = 0;
            exif[3] = 0;
            memcpy(exif + 4, data, size);
            memcpy(type, "Exif", 4);
            contents = exif;
            contents_size = size + 4;
        } else if (strcmp(key, "xmp") == 0) {
            memcpy(type, "xml ", 4);
            contents = data;
            contents_size = size;
        } else {
            continue;
        }

        if (JxlEncoderAddBox(enc, type, contents, contents_size, JXL_FALSE) != JXL_ENC_SUCCESS) {
            gdFree(exif);
            return 0;
        }
        gdFree(exif);
    }

    return 1;
}

static int _gdImageJxlCtxWithOptions(gdImagePtr im, gdIOCtx *outfile,
                                     const gdJxlWriteOptions *options)
{
    JxlEncoder *enc = NULL;
    JxlEncoderFrameSettings *frame_opts = NULL;
    uint8_t *pixels = NULL;
    uint8_t *frame_pixels = NULL;
    int has_alpha = 0;
    uint8_t outbuf[65536];
    int w, h;
    int ret = 1;
    gdJxlWriteOptions defaults;

    gdJxlWriteOptionsInit(&defaults);
    if (options == NULL) {
        options = &defaults;
    }

    if (im == NULL || outfile == NULL || gdImageSX(im) <= 0 || gdImageSY(im) <= 0 ||
            options->distance < 0.0f || options->distance > 25.0f ||
            options->effort < 1 || options->effort > 9) {
        return 1;
    }

    w = gdImageSX(im);
    h = gdImageSY(im);

    /* Extract RGBA pixels */
    if (!JxlImageToRGBA(im, &pixels, &has_alpha)) {
        gd_error("gdImageJxl: pixel extraction failed");
        return 1;
    }

    enc = JxlEncoderCreate(NULL);
    if (enc == NULL) {
        gd_error("gdImageJxl: JxlEncoderCreate failed");
        gdFree(pixels);
        return 1;
    }

    if (options->metadata != NULL) {
        if (JxlEncoderUseContainer(enc, JXL_TRUE) != JXL_ENC_SUCCESS ||
                JxlEncoderUseBoxes(enc) != JXL_ENC_SUCCESS) {
            gd_error("gdImageJxl: failed to enable container metadata");
            goto encode_fail;
        }
    }

    /* Set basic info */
    JxlBasicInfo info;
    JxlEncoderInitBasicInfo(&info);
    info.xsize = w;
    info.ysize = h;
    info.bits_per_sample = 8;
    info.exponent_bits_per_sample = 0;
    info.num_color_channels = 3;
    info.num_extra_channels = has_alpha ? 1 : 0;
    info.alpha_bits = has_alpha ? 8 : 0;
    info.alpha_exponent_bits = 0;
    info.alpha_premultiplied = JXL_FALSE;
    info.uses_original_profile = options->lossless ? JXL_TRUE : JXL_FALSE;

    if (JxlEncoderSetBasicInfo(enc, &info) != JXL_ENC_SUCCESS) {
        gd_error("gdImageJxl: JxlEncoderSetBasicInfo failed");
        goto encode_fail;
    }

    /* Set color encoding (sRGB) */
    JxlColorEncoding srgb_enc;
    JxlColorEncodingSetToSRGB(&srgb_enc, JXL_FALSE);
    if (JxlEncoderSetColorEncoding(enc, &srgb_enc) != JXL_ENC_SUCCESS) {
        gd_error("gdImageJxl: JxlEncoderSetColorEncoding failed");
        goto encode_fail;
    }

    /* Configure frame options */
    frame_opts = JxlEncoderFrameSettingsCreate(enc, NULL);
    if (frame_opts == NULL) {
        gd_error("gdImageJxl: JxlEncoderFrameSettingsCreate failed");
        goto encode_fail;
    }

    if (options->lossless) {
        if (JxlEncoderSetFrameLossless(frame_opts, JXL_TRUE) != JXL_ENC_SUCCESS) {
            gd_error("gdImageJxl: JxlEncoderSetFrameLossless failed");
            goto encode_fail;
        }
    } else {
        if (JxlEncoderSetFrameDistance(frame_opts, options->distance) != JXL_ENC_SUCCESS) {
            gd_error("gdImageJxl: JxlEncoderSetFrameDistance failed");
            goto encode_fail;
        }
    }

    if (JxlEncoderFrameSettingsSetOption(frame_opts, JXL_ENC_FRAME_SETTING_EFFORT, options->effort) !=
        JXL_ENC_SUCCESS) {
        gd_error("gdImageJxl: JxlEncoderFrameSettingsSetOption effort failed");
        goto encode_fail;
    }

    if (options->metadata != NULL && !JxlEncoderAddMetadata(enc, options->metadata)) {
        gd_error("gdImageJxl: failed to add metadata");
        goto encode_fail;
    }

    /* Add image frame */
    size_t frame_pixels_size = (size_t)w * (size_t)h * 4;
    JxlPixelFormat fmt = {.num_channels = has_alpha ? 4u : 3u,
                          .data_type = JXL_TYPE_UINT8,
                          .endianness = JXL_NATIVE_ENDIAN,
                          .align = 0};

    frame_pixels = pixels;
    if (!has_alpha) {
        frame_pixels = JxlRGBFromRGBA(pixels, w, h);
        if (frame_pixels == NULL) {
            gd_error("gdImageJxl: RGB buffer allocation failed");
            goto encode_fail;
        }
        frame_pixels_size = (size_t)w * (size_t)h * 3;
    }

    if (JxlEncoderAddImageFrame(frame_opts, &fmt, frame_pixels, frame_pixels_size) !=
        JXL_ENC_SUCCESS) {
        gd_error("gdImageJxl: JxlEncoderAddImageFrame failed");
        goto encode_fail;
    }

    JxlEncoderCloseInput(enc);
    for (;;) {
        uint8_t *next_out = outbuf;
        size_t avail = sizeof(outbuf);
        JxlEncoderStatus st = JxlEncoderProcessOutput(enc, &next_out, &avail);

        size_t written = sizeof(outbuf) - avail;
        if (written > 0) {
            if (gdPutBuf(outbuf, (int)written, outfile) != (int)written) {
                gd_error("gdImageJxl: write error");
                goto encode_fail;
            }
        }

        if (st == JXL_ENC_SUCCESS) {
            ret = 0;
            break;
        }
        if (st != JXL_ENC_NEED_MORE_OUTPUT) {
            gd_error("gdImageJxl: encoder error");
            goto encode_fail;
        }
    }

encode_fail:
    // frame_opts is owned by enc, so no separate destroy needed
    if (enc)
        JxlEncoderDestroy(enc);
    if (frame_pixels != NULL && frame_pixels != pixels)
        gdFree(frame_pixels);
    if (pixels)
        gdFree(pixels);
    return ret;
}

BGD_DECLARE(void) gdImageJxl(gdImagePtr im, FILE *outFile)
{
    gdIOCtx *out = gdNewFileCtx(outFile);
    if (out == NULL) {
        return;
    }
    _gdImageJxlCtxWithOptions(im, out, NULL);
    out->gd_free(out);
}

BGD_DECLARE(void)
gdImageJxlEx(gdImagePtr im, FILE *outFile, int lossless, float distance, int effort)
{
    gdIOCtx *out = gdNewFileCtx(outFile);
    if (out == NULL) {
        return;
    }
    gdJxlWriteOptions options;
    gdJxlWriteOptionsInit(&options);
    options.lossless = lossless;
    options.distance = distance;
    options.effort = effort;
    _gdImageJxlCtxWithOptions(im, out, &options);
    out->gd_free(out);
}

BGD_DECLARE(void *) gdImageJxlPtr(gdImagePtr im, int *size)
{
    void *rv;
    gdIOCtx *out = gdNewDynamicCtx(2048, NULL);
    if (out == NULL) {
        return NULL;
    }
    if (_gdImageJxlCtxWithOptions(im, out, NULL)) {
        rv = NULL;
    } else {
        rv = gdDPExtractData(out, size);
    }
    out->gd_free(out);
    return rv;
}

BGD_DECLARE(void *)
gdImageJxlPtrEx(gdImagePtr im, int *size, int lossless, float distance, int effort)
{
    void *rv;
    gdIOCtx *out = gdNewDynamicCtx(2048, NULL);
    if (out == NULL) {
        return NULL;
    }
    gdJxlWriteOptions options;
    gdJxlWriteOptionsInit(&options);
    options.lossless = lossless;
    options.distance = distance;
    options.effort = effort;
    if (_gdImageJxlCtxWithOptions(im, out, &options)) {
        rv = NULL;
    } else {
        rv = gdDPExtractData(out, size);
    }
    out->gd_free(out);
    return rv;
}

BGD_DECLARE(void) gdImageJxlCtx(gdImagePtr im, gdIOCtxPtr outfile)
{
    _gdImageJxlCtxWithOptions(im, outfile, NULL);
}

BGD_DECLARE(void)
gdImageJxlCtxEx(gdImagePtr im, gdIOCtxPtr outfile, int lossless, float distance, int effort)
{
    gdJxlWriteOptions options;
    gdJxlWriteOptionsInit(&options);
    options.lossless = lossless;
    options.distance = distance;
    options.effort = effort;
    _gdImageJxlCtxWithOptions(im, outfile, &options);
}

BGD_DECLARE(int)
gdImageJxlWithOptions(gdImagePtr im, FILE *outFile, const gdJxlWriteOptions *options)
{
    gdIOCtx *out;
    int result;

    if (outFile == NULL || (out = gdNewFileCtx(outFile)) == NULL) {
        return 0;
    }
    result = gdImageJxlCtxWithOptions(im, out, options);
    out->gd_free(out);
    return result;
}

BGD_DECLARE(int)
gdImageJxlCtxWithOptions(gdImagePtr im, gdIOCtxPtr outfile, const gdJxlWriteOptions *options)
{
    return _gdImageJxlCtxWithOptions(im, outfile, options) == 0;
}

BGD_DECLARE(void *)
gdImageJxlPtrWithOptions(gdImagePtr im, int *size, const gdJxlWriteOptions *options)
{
    void *result;
    gdIOCtx *out = gdNewDynamicCtx(2048, NULL);

    if (size != NULL) {
        *size = 0;
    }
    if (out == NULL) {
        return NULL;
    }
    if (_gdImageJxlCtxWithOptions(im, out, options) != 0) {
        result = NULL;
    } else {
        result = gdDPExtractData(out, size);
    }
    out->gd_free(out);
    return result;
}

/* ---- Multi-image structures ---- */

typedef struct gdJxlWrite {
    JxlEncoder *enc;
    JxlEncoderFrameSettings *frame_opts;
    gdIOCtxPtr ctx;
    gdJxlAnimWriteOptions options;
    uint32_t width;
    uint32_t height;
    int has_alpha;
    int ownsCtx;
    int memoryWriter;
    int finalized;
    int timestamp;
    int frameCount;
    uint8_t **framePixels;
    int framePixelsCount;
    int framePixelsCapacity;
} gdJxlWrite;

typedef struct gdJxlRead {
    JxlDecoder *dec;
    uint8_t *buf;
    size_t buf_len;
    JxlBasicInfo info;
    int coalesced;
    int done;
    int last_frame_seen;
} gdJxlRead;

BGD_DECLARE(void) gdJxlReadOptionsInit(gdJxlReadOptions *options)
{
    if (options == NULL) {
        return;
    }
    memset(options, 0, sizeof(*options));
    options->coalesced = 1;
}

BGD_DECLARE(void) gdJxlWriteOptionsInit(gdJxlWriteOptions *options)
{
    if (options == NULL) {
        return;
    }
    memset(options, 0, sizeof(*options));
    options->distance = 1.0f;
    options->effort = 7;
}

BGD_DECLARE(void) gdJxlAnimWriteOptionsInit(gdJxlAnimWriteOptions *options)
{
    if (options == NULL) {
        return;
    }
    memset(options, 0, sizeof(*options));
    options->distance = 1.0f;
    options->effort = 7;
}

/* ---- Animation write ---- */

static int JxlWriteDrainEncoder(gdJxlWrite *writer)
{
    uint8_t outbuf[65536];
    for (;;) {
        uint8_t *next_out = outbuf;
        size_t avail = sizeof(outbuf);
        JxlEncoderStatus st = JxlEncoderProcessOutput(writer->enc, &next_out, &avail);

        size_t written = sizeof(outbuf) - avail;
        if (written > 0) {
            if (gdPutBuf(outbuf, (int)written, writer->ctx) != (int)written) {
                gd_error("gd-jxl write: write error");
                return 0;
            }
        }

        if (st == JXL_ENC_SUCCESS) {
            return 1;
        }
        if (st != JXL_ENC_NEED_MORE_OUTPUT) {
            gd_error("gd-jxl write: encoder error");
            return 0;
        }
    }
}

static int JxlWriteNormalizeOptions(const gdJxlAnimWriteOptions *options, gdJxlAnimWriteOptions *normalized)
{
    gdJxlAnimWriteOptions defaults;

    gdJxlAnimWriteOptionsInit(&defaults);
    *normalized = defaults;
    if (options == NULL) {
        return 1;
    }
    *normalized = *options;
    if (normalized->effort == 0) {
        normalized->effort = defaults.effort;
    }
    if (normalized->distance == 0.0f && !normalized->lossless) {
        normalized->distance = defaults.distance;
    }
    return 1;
}

static int JxlWriteEnsureEncoder(gdJxlWrite *writer, gdImagePtr image)
{
    JxlBasicInfo info;
    int width, height;
    const gdJxlAnimWriteOptions *options;

    if (writer == NULL || image == NULL || writer->ctx == NULL) {
        return 0;
    }
    if (writer->enc != NULL) {
        return 1;
    }

    options = &writer->options;
    width = options->canvas_width > 0 ? options->canvas_width : gdImageSX(image);
    height = options->canvas_height > 0 ? options->canvas_height : gdImageSY(image);
    if (width <= 0 || height <= 0) {
        gd_error("gd-jxl write: invalid canvas size");
        return 0;
    }

    writer->enc = JxlEncoderCreate(NULL);
    if (writer->enc == NULL) {
        gd_error("gd-jxl write: JxlEncoderCreate failed");
        return 0;
    }

    writer->width = width;
    writer->height = height;

    JxlEncoderInitBasicInfo(&info);
    info.xsize = width;
    info.ysize = height;
    info.bits_per_sample = 8;
    info.exponent_bits_per_sample = 0;
    info.num_color_channels = 3;
    info.num_extra_channels = 1;
    info.alpha_bits = 8;
    info.alpha_exponent_bits = 0;
    info.alpha_premultiplied = JXL_FALSE;
    info.uses_original_profile = options->lossless ? JXL_TRUE : JXL_FALSE;
    info.have_animation = JXL_TRUE;
    info.animation.tps_numerator = 1000; /* ms == ticks */
    info.animation.tps_denominator = 1;
    info.animation.num_loops = (uint32_t)options->loop_count;

    if (JxlEncoderSetBasicInfo(writer->enc, &info) != JXL_ENC_SUCCESS) {
        gd_error("gd-jxl write: JxlEncoderSetBasicInfo failed");
        goto fail;
    }

    JxlColorEncoding srgb_enc;
    JxlColorEncodingSetToSRGB(&srgb_enc, JXL_FALSE);
    if (JxlEncoderSetColorEncoding(writer->enc, &srgb_enc) != JXL_ENC_SUCCESS) {
        gd_error("gd-jxl write: JxlEncoderSetColorEncoding failed");
        goto fail;
    }

    writer->frame_opts = JxlEncoderFrameSettingsCreate(writer->enc, NULL);
    if (writer->frame_opts == NULL) {
        gd_error("gd-jxl write: JxlEncoderFrameSettingsCreate failed");
        goto fail;
    }

    if (options->lossless) {
        if (JxlEncoderSetFrameLossless(writer->frame_opts, JXL_TRUE) != JXL_ENC_SUCCESS) {
            gd_error("gd-jxl write: JxlEncoderSetFrameLossless failed");
            goto fail;
        }
    } else {
        if (JxlEncoderSetFrameDistance(writer->frame_opts, options->distance) != JXL_ENC_SUCCESS) {
            gd_error("gd-jxl write: JxlEncoderSetFrameDistance failed");
            goto fail;
        }
    }

    if (JxlEncoderFrameSettingsSetOption(writer->frame_opts, JXL_ENC_FRAME_SETTING_EFFORT,
                                         options->effort) != JXL_ENC_SUCCESS) {
        gd_error("gd-jxl write: JxlEncoderFrameSettingsSetOption effort failed");
        goto fail;
    }

    return 1;

fail:
    if (writer->enc) {
        JxlEncoderDestroy(writer->enc);
    }
    writer->enc = NULL;
    writer->frame_opts = NULL;
    return 0;
}

static int JxlWriteEnsureFramePixelCapacity(gdJxlWrite *writer)
{
    uint8_t **framePixels;
    int capacity;

    if (writer->framePixelsCount < writer->framePixelsCapacity) {
        return 1;
    }

    capacity = writer->framePixelsCapacity == 0 ? 4 : writer->framePixelsCapacity * 2;
    framePixels = (uint8_t **)gdRealloc(writer->framePixels, (size_t)capacity * sizeof(uint8_t *));
    if (framePixels == NULL) {
        return 0;
    }
    writer->framePixels = framePixels;
    writer->framePixelsCapacity = capacity;
    return 1;
}

BGD_DECLARE(gdJxlWritePtr) gdJxlWriteOpen(FILE *outFile, const gdJxlAnimWriteOptions *options)
{
    gdIOCtx *out;
    gdJxlWritePtr writer;

    if (outFile == NULL) {
        return NULL;
    }
    out = gdNewFileCtx(outFile);
    if (out == NULL) {
        return NULL;
    }
    writer = gdJxlWriteOpenCtx(out, options);
    if (writer == NULL) {
        out->gd_free(out);
        return NULL;
    }
    writer->ownsCtx = 1;
    return writer;
}

BGD_DECLARE(gdJxlWritePtr) gdJxlWriteOpenCtx(gdIOCtxPtr outCtx, const gdJxlAnimWriteOptions *options)
{
    gdJxlAnimWriteOptions normalized;
    gdJxlWritePtr writer;

    if (outCtx == NULL || !JxlWriteNormalizeOptions(options, &normalized)) {
        return NULL;
    }
    writer = (gdJxlWritePtr)gdCalloc(1, sizeof(struct gdJxlWrite));
    if (writer == NULL) {
        return NULL;
    }
    writer->ctx = outCtx;
    writer->options = normalized;
    return writer;
}

BGD_DECLARE(gdJxlWritePtr) gdJxlWriteOpenPtr(const gdJxlAnimWriteOptions *options)
{
    gdIOCtx *out;
    gdJxlWritePtr writer;

    out = gdNewDynamicCtx(2048, NULL);
    if (out == NULL) {
        return NULL;
    }
    writer = gdJxlWriteOpenCtx(out, options);
    if (writer == NULL) {
        out->gd_free(out);
        return NULL;
    }
    writer->ownsCtx = 1;
    writer->memoryWriter = 1;
    return writer;
}

BGD_DECLARE(int) gdJxlWriteAddImage(gdJxlWritePtr writer, gdImagePtr image, int delay_ms)
{
    JxlFrameHeader fhdr;
    JxlPixelFormat fmt;
    uint8_t *pixels = NULL;
    int has_alpha = 0;
    int w, h;

    if (writer == NULL || image == NULL || delay_ms < 0 || writer->finalized) {
        return 0;
    }

    if (!gdImageTrueColor(image)) {
        gd_error("Palette image not supported by JXL animation");
        return 0;
    }

    if (!JxlWriteEnsureEncoder(writer, image)) {
        return 0;
    }

    w = gdImageSX(image);
    h = gdImageSY(image);

    if (w != (int)writer->width || h != (int)writer->height) {
        gd_error("gd-jxl write: frame size must match canvas size");
        return 0;
    }

    if (!JxlImageToRGBA(image, &pixels, &has_alpha)) {
        gd_error("gd-jxl write: pixel extraction failed");
        return 0;
    }

    if (has_alpha && writer->has_alpha == 0) {
        writer->has_alpha = 1;
    }

    if (!JxlWriteEnsureFramePixelCapacity(writer)) {
        gd_error("gd-jxl write: frame buffer ownership failed");
        gdFree(pixels);
        return 0;
    }

    JxlEncoderInitFrameHeader(&fhdr);
    fhdr.duration = (uint32_t)delay_ms; /* ticks == ms */

    if (JxlEncoderSetFrameHeader(writer->frame_opts, &fhdr) != JXL_ENC_SUCCESS) {
        gd_error("gd-jxl write: JxlEncoderSetFrameHeader failed");
        gdFree(pixels);
        return 0;
    }

    fmt.num_channels = 4;
    fmt.data_type = JXL_TYPE_UINT8;
    fmt.endianness = JXL_NATIVE_ENDIAN;
    fmt.align = 0;

    if (JxlEncoderAddImageFrame(writer->frame_opts, &fmt, pixels, (size_t)w * (size_t)h * 4) !=
        JXL_ENC_SUCCESS) {
        gd_error("gd-jxl write: JxlEncoderAddImageFrame failed");
        gdFree(pixels);
        return 0;
    }

    writer->framePixels[writer->framePixelsCount++] = pixels;

    writer->timestamp += delay_ms;
    writer->frameCount++;
    return 1;
}

static int JxlWriteFinish(gdJxlWritePtr writer)
{
    if (writer == NULL || writer->finalized || writer->enc == NULL || writer->frameCount == 0) {
        return 0;
    }

    JxlEncoderCloseInput(writer->enc);
    if (!JxlWriteDrainEncoder(writer)) {
        return 0;
    }
    writer->finalized = 1;
    return 1;
}

static void JxlWriteFree(gdJxlWritePtr writer)
{
    int i;

    if (writer == NULL) {
        return;
    }
    if (writer->enc) {
        JxlEncoderDestroy(writer->enc);
    }
    for (i = 0; i < writer->framePixelsCount; i++) {
        gdFree(writer->framePixels[i]);
    }
    gdFree(writer->framePixels);
    if (writer->ownsCtx && writer->ctx) {
        writer->ctx->gd_free(writer->ctx);
    }
    gdFree(writer);
}

BGD_DECLARE(void) gdJxlWriteClose(gdJxlWritePtr writer)
{
    if (writer == NULL) {
        return;
    }
    if (!writer->finalized) {
        (void)JxlWriteFinish(writer);
    }
    JxlWriteFree(writer);
}

BGD_DECLARE(void *) gdJxlWritePtrFinish(gdJxlWritePtr writer, int *size)
{
    void *rv = NULL;

    if (size != NULL) {
        *size = 0;
    }
    if (writer == NULL || !writer->memoryWriter) {
        if (writer != NULL) {
            gdJxlWriteClose(writer);
        }
        return NULL;
    }

    if (!writer->finalized && !JxlWriteFinish(writer)) {
        goto finish_cleanup;
    }

    rv = gdDPExtractData(writer->ctx, size);

finish_cleanup:
    JxlWriteFree(writer);
    return rv;
}

/* ---- Animation read (coalesced & raw) ---- */

static gdJxlReadPtr JxlReadOpenCtx(gdIOCtxPtr inCtx, int coalesced)
{
    size_t buf_len = 0;
    uint8_t *buf = NULL;
    JxlDecoder *dec = NULL;
    gdJxlReadPtr reader = NULL;

    if (inCtx == NULL) {
        return NULL;
    }

    buf = JxlReadCtxData(inCtx, &buf_len);
    if (buf == NULL) {
        return NULL;
    }

    dec = JxlDecoderCreate(NULL);
    if (dec == NULL) {
        gd_error("gd-jxl read: JxlDecoderCreate failed");
        gdFree(buf);
        return NULL;
    }

    reader = (gdJxlReadPtr)gdCalloc(1, sizeof(struct gdJxlRead));
    if (reader == NULL) {
        JxlDecoderDestroy(dec);
        gdFree(buf);
        return NULL;
    }

    if (coalesced) {
        JxlDecoderSubscribeEvents(dec, JXL_DEC_BASIC_INFO | JXL_DEC_COLOR_ENCODING | JXL_DEC_FRAME |
                                           JXL_DEC_FULL_IMAGE);
        JxlDecoderSetCoalescing(dec, JXL_TRUE);
    } else {
        JxlDecoderSubscribeEvents(dec, JXL_DEC_BASIC_INFO | JXL_DEC_COLOR_ENCODING | JXL_DEC_FRAME |
                                           JXL_DEC_FULL_IMAGE);
        JxlDecoderSetCoalescing(dec, JXL_FALSE);
    }

    /* Attach CMS for color space conversion to sRGB */
    JxlDecoderSetCms(dec, *JxlGetDefaultCms());

    /* Set desired output color profile (sRGB) */
    JxlColorEncoding srgb_enc;
    JxlColorEncodingSetToSRGB(&srgb_enc, JXL_FALSE);
    JxlDecoderSetPreferredColorProfile(dec, &srgb_enc);

    JxlDecoderSetInput(dec, buf, buf_len);
    JxlDecoderCloseInput(dec);

    /* Process until BASIC_INFO to get dimensions */
    for (;;) {
        JxlDecoderStatus status = JxlDecoderProcessInput(dec);
        if (status == JXL_DEC_BASIC_INFO) {
            if (JxlDecoderGetBasicInfo(dec, &reader->info) != JXL_DEC_SUCCESS) {
                gd_error("gd-jxl read: failed to get basic info");
                JxlDecoderDestroy(dec);
                gdFree(buf);
                gdFree(reader);
                return NULL;
            }
            break;
        }
        if (status == JXL_DEC_ERROR || status == JXL_DEC_SUCCESS) {
            gd_error("gd-jxl read: failed to get basic info");
            JxlDecoderDestroy(dec);
            gdFree(buf);
            gdFree(reader);
            return NULL;
        }
    }

    reader->dec = dec;
    reader->buf = buf;
    reader->buf_len = buf_len;
    reader->coalesced = coalesced;
    reader->done = 0;
    reader->last_frame_seen = 0;

    return reader;
}

static int JxlReadNormalizeOptions(const gdJxlReadOptions *options, gdJxlReadOptions *normalized)
{
    gdJxlReadOptions defaults;

    gdJxlReadOptionsInit(&defaults);
    *normalized = defaults;
    if (options == NULL) {
        return 1;
    }
    *normalized = *options;
    return 1;
}

BGD_DECLARE(gdJxlReadPtr) gdJxlReadOpen(FILE *inFile, const gdJxlReadOptions *options)
{
    gdIOCtx *in;
    gdJxlReadPtr reader;

    if (inFile == NULL) {
        return NULL;
    }
    in = gdNewFileCtx(inFile);
    if (in == NULL) {
        return NULL;
    }
    reader = gdJxlReadOpenCtx(in, options);
    in->gd_free(in);
    return reader;
}

BGD_DECLARE(gdJxlReadPtr) gdJxlReadOpenCtx(gdIOCtxPtr inCtx, const gdJxlReadOptions *options)
{
    gdJxlReadOptions normalized;

    if (!JxlReadNormalizeOptions(options, &normalized)) {
        return NULL;
    }
    return JxlReadOpenCtx(inCtx, normalized.coalesced ? JXL_TRUE : JXL_FALSE);
}

BGD_DECLARE(gdJxlReadPtr)
gdJxlReadOpenPtr(int size, void *data, const gdJxlReadOptions *options)
{
    gdIOCtx *in;
    gdJxlReadPtr reader;

    in = gdNewDynamicCtxEx(size, data, 0);
    if (in == NULL) {
        return NULL;
    }
    reader = gdJxlReadOpenCtx(in, options);
    in->gd_free(in);
    return reader;
}

BGD_DECLARE(int) gdJxlReadGetInfo(gdJxlReadPtr reader, gdJxlInfo *info)
{
    if (reader == NULL || info == NULL) {
        return 0;
    }
    info->width = (int)reader->info.xsize;
    info->height = (int)reader->info.ysize;
    info->animated = reader->info.have_animation == JXL_TRUE;
    info->loop_count = info->animated ? (int)reader->info.animation.num_loops : 0;
    return 1;
}

BGD_DECLARE(int) gdJxlReadGetMetadata(gdJxlReadPtr reader, gdImageMetadata *metadata)
{
    JxlDecoder *dec = NULL;
    int result = GD_META_OK;
    size_t max_profile_size = 0;

    if (reader == NULL || metadata == NULL) {
        return GD_META_ERR_INVALID;
    }
    gdImageMetadataGetLimits(metadata, &max_profile_size, NULL);

    dec = JxlDecoderCreate(NULL);
    if (dec == NULL) {
        return GD_META_ERR_NOMEM;
    }
    if (JxlDecoderSubscribeEvents(dec, JXL_DEC_BOX) != JXL_DEC_SUCCESS ||
            JxlDecoderSetDecompressBoxes(dec, JXL_TRUE) != JXL_DEC_SUCCESS) {
        result = GD_META_ERR_UNSUPPORTED;
        goto metadata_done;
    }
    JxlDecoderSetInput(dec, reader->buf, reader->buf_len);
    JxlDecoderCloseInput(dec);

    for (;;) {
        JxlDecoderStatus status = JxlDecoderProcessInput(dec);

        if (status == JXL_DEC_BOX) {
            JxlBoxType type;
            uint8_t *buffer = NULL;
            size_t capacity = 65536;
            size_t used = 0;
            size_t released;

            if (capacity > max_profile_size) {
                capacity = max_profile_size;
            }
            if (capacity == 0) {
                result = GD_META_ERR_LIMIT;
                goto metadata_done;
            }

            if (JxlDecoderGetBoxType(dec, type, JXL_TRUE) != JXL_DEC_SUCCESS) {
                result = GD_META_ERR_FORMAT;
                goto metadata_done;
            }
            if (memcmp(type, "Exif", 4) != 0 && memcmp(type, "xml ", 4) != 0) {
                continue;
            }

            buffer = gdMalloc(capacity);
            if (buffer == NULL) {
                result = GD_META_ERR_NOMEM;
                goto metadata_done;
            }
            for (;;) {
                if (JxlDecoderSetBoxBuffer(dec, buffer + used, capacity - used) != JXL_DEC_SUCCESS) {
                    result = GD_META_ERR_FORMAT;
                    gdFree(buffer);
                    goto metadata_done;
                }
                status = JxlDecoderProcessInput(dec);
                released = JxlDecoderReleaseBoxBuffer(dec);
                used += capacity - used - released;
                if (status == JXL_DEC_BOX_NEED_MORE_OUTPUT) {
                    uint8_t *grown;
                    if (capacity >= max_profile_size || capacity > SIZE_MAX / 2) {
                        result = GD_META_ERR_LIMIT;
                        gdFree(buffer);
                        goto metadata_done;
                    }
                    capacity *= 2;
                    if (capacity > max_profile_size) {
                        capacity = max_profile_size;
                    }
                    grown = gdRealloc(buffer, capacity);
                    if (grown == NULL) {
                        result = GD_META_ERR_NOMEM;
                        gdFree(buffer);
                        goto metadata_done;
                    }
                    buffer = grown;
                    continue;
                }
                if (status != JXL_DEC_BOX_COMPLETE && status != JXL_DEC_BOX && status != JXL_DEC_SUCCESS) {
                    result = GD_META_ERR_FORMAT;
                    gdFree(buffer);
                    goto metadata_done;
                }
                break;
            }

            if (memcmp(type, "Exif", 4) == 0) {
                size_t offset;
                if (used < 4) {
                    result = GD_META_ERR_FORMAT;
                    gdFree(buffer);
                    goto metadata_done;
                }
                offset = ((size_t)buffer[0] << 24) | ((size_t)buffer[1] << 16) |
                    ((size_t)buffer[2] << 8) | (size_t)buffer[3];
                if (offset > used - 4) {
                    result = GD_META_ERR_FORMAT;
                    gdFree(buffer);
                    goto metadata_done;
                }
                {
                    const unsigned char *tiff;
                    size_t tiff_size;
                    if (gdMetadataGetExifTiff(buffer + 4 + offset, used - 4 - offset,
                                              &tiff, &tiff_size) != GD_META_OK ||
                        gdImageMetadataSetProfile(metadata, "exif", tiff, tiff_size) != GD_META_OK) {
                        result = GD_META_ERR_FORMAT;
                        gdFree(buffer);
                        goto metadata_done;
                    }
                }
            } else if (gdImageMetadataSetProfile(metadata, "xmp", buffer, used) != GD_META_OK) {
                result = GD_META_ERR_FORMAT;
                gdFree(buffer);
                goto metadata_done;
            }
            gdFree(buffer);
            continue;
        }
        if (status == JXL_DEC_SUCCESS) {
            break;
        }
        if (status == JXL_DEC_ERROR) {
            result = GD_META_ERR_FORMAT;
            break;
        }
    }

metadata_done:
    JxlDecoderDestroy(dec);
    return result;
}

/* Helper: build gdImagePtr from current decoder state */
static gdImagePtr JxlReadGetCurrentImage(gdJxlReadPtr reader, int *delay_ms)
{
    JxlDecoder *dec = reader->dec;
    JxlBasicInfo info = reader->info;
    uint8_t *pixels = NULL;
    gdImagePtr im = NULL;
    int have_frame_header = 0;

    if (reader->done) {
        return NULL;
    }

    for (;;) {
        JxlDecoderStatus status = JxlDecoderProcessInput(dec);

        if (status == JXL_DEC_FRAME) {
            JxlFrameHeader fhdr = {0};
            status = JxlDecoderGetFrameHeader(dec, &fhdr);
            if (status != JXL_DEC_SUCCESS) {
                gd_error("gdJxlReadNextFrame: failed to get frame header");
                if (pixels)
                    gdFree(pixels);
                return NULL;
            }
            have_frame_header = 1;
            reader->last_frame_seen = fhdr.is_last == JXL_TRUE;
            if (delay_ms) {
                *delay_ms = JxlDurationToMs(fhdr.duration, &info.animation);
            }
        } else if (status == JXL_DEC_NEED_IMAGE_OUT_BUFFER) {
            JxlPixelFormat fmt = {.num_channels = 4,
                                  .data_type = JXL_TYPE_UINT8,
                                  .endianness = JXL_NATIVE_ENDIAN,
                                  .align = 0};
            size_t pixels_size;
            int w, h;

            if (reader->coalesced) {
                w = (int)info.xsize;
                h = (int)info.ysize;
            } else {
                /* For non-coalesced, we need to query the frame size */
                size_t buf_size;
                if (!have_frame_header) {
                    gd_error("gdJxlReadNextFrame: missing frame header");
                    return NULL;
                }
                if (JxlDecoderImageOutBufferSize(dec, &fmt, &buf_size) != JXL_DEC_SUCCESS) {
                    gd_error("gdJxlReadNextFrame: failed to get buffer size");
                    return NULL;
                }
                w = (int)sqrt(buf_size / 4); /* approximation - we'll get actual size from fmt */
                h = (int)(buf_size / 4 / w);
                /* Actually better: we know the frame size from layer_info but
                 * for simplicity with coalesced=false we allocate based on
                 * the buffer size reported */
            }

            if (overflow2(w, h) || overflow2(w * h, 4)) {
                gd_error("gdJxlReadNextFrame: frame dimensions overflow");
                return NULL;
            }

            pixels_size = (size_t)w * (size_t)h * 4;
            pixels = (uint8_t *)gdMalloc(pixels_size);
            if (pixels == NULL) {
                gd_error("gdJxlReadNextFrame: pixel buffer allocation failed");
                return NULL;
            }

            if (JxlDecoderSetImageOutBuffer(dec, &fmt, pixels, pixels_size) != JXL_DEC_SUCCESS) {
                gd_error("gdJxlReadNextFrame: failed to set output buffer");
                gdFree(pixels);
                return NULL;
            }
        } else if (status == JXL_DEC_FULL_IMAGE) {
            int has_alpha = (info.alpha_bits > 0);
            int w, h;

            if (reader->coalesced) {
                w = (int)info.xsize;
                h = (int)info.ysize;
            } else {
                /* For non-coalesced, we don't know exact dimensions here.
                 * We'll reconstruct from the buffer. This is a simplification.
                 */
                w = (int)info.xsize;
                h = (int)info.ysize;
            }

            im = JxlImageFromRGBA(pixels, w, h, has_alpha);
            if (pixels)
                gdFree(pixels);
            if (reader->last_frame_seen)
                reader->done = 1;
            return im;
        } else if (status == JXL_DEC_SUCCESS) {
            reader->done = 1;
            if (pixels)
                gdFree(pixels);
            return NULL;
        } else if (status == JXL_DEC_ERROR) {
            gd_error("gdJxlReadNextFrame: decoder error");
            if (pixels)
                gdFree(pixels);
            return NULL;
        }
    }
}

BGD_DECLARE(int)
gdJxlReadNextImage(gdJxlReadPtr reader, int *delay_ms, gdImagePtr *image)
{
    if (reader == NULL || reader->coalesced == 0) {
        return -1;
    }
    if (image != NULL) {
        *image = NULL;
    }
    if (reader->done) {
        return 0;
    }
    if (image == NULL) {
        gdImagePtr ignored = JxlReadGetCurrentImage(reader, delay_ms);
        if (ignored == NULL) {
            return 0;
        }
        gdImageDestroy(ignored);
        return 1;
    }
    *image = JxlReadGetCurrentImage(reader, delay_ms);
    return *image != NULL ? 1 : 0;
}

/* For non-coalesced reader, we need to extract frame info */
static void JxlFrameHeaderToInfo(const JxlFrameHeader *fhdr, const JxlBasicInfo *info,
                                 gdJxlFrameInfo *out)
{
    out->delay_ms = JxlDurationToMs(fhdr->duration, &info->animation);
    out->x_offset = (int)fhdr->layer_info.crop_x0;
    out->y_offset = (int)fhdr->layer_info.crop_y0;
    out->width = (int)fhdr->layer_info.xsize;
    out->height = (int)fhdr->layer_info.ysize;
    out->blend_mode = (int)fhdr->layer_info.blend_info.blendmode;
    out->is_last = (int)fhdr->is_last;
}

static gdImagePtr JxlReadGetCurrentRawFrame(gdJxlReadPtr reader, gdJxlFrameInfo *info)
{
    JxlDecoder *dec = reader->dec;
    JxlBasicInfo basic = reader->info;
    JxlFrameHeader fhdr = {0};
    uint8_t *pixels = NULL;
    gdImagePtr im = NULL;
    int has_alpha = 0;
    int have_frame_header = 0;
    int w = 0, h = 0;

    if (reader->done) {
        return NULL;
    }

    for (;;) {
        JxlDecoderStatus status = JxlDecoderProcessInput(dec);

        if (status == JXL_DEC_FRAME) {
            status = JxlDecoderGetFrameHeader(dec, &fhdr);
            if (status != JXL_DEC_SUCCESS) {
                gd_error("gdJxlReadNextFrame: failed to get frame header");
                return NULL;
            }
            have_frame_header = 1;
            reader->last_frame_seen = fhdr.is_last == JXL_TRUE;
            JxlFrameHeaderToInfo(&fhdr, &basic, info);
            has_alpha = (basic.alpha_bits > 0);
        } else if (status == JXL_DEC_NEED_IMAGE_OUT_BUFFER) {
            JxlPixelFormat fmt = {.num_channels = 4,
                                  .data_type = JXL_TYPE_UINT8,
                                  .endianness = JXL_NATIVE_ENDIAN,
                                  .align = 0};
            size_t pixels_size;

            /* Query the exact buffer size for this frame */
            if (!have_frame_header) {
                gd_error("gdJxlReadNextFrame: missing frame header");
                return NULL;
            }
            if (JxlDecoderImageOutBufferSize(dec, &fmt, &pixels_size) != JXL_DEC_SUCCESS) {
                gd_error("gdJxlReadNextFrame: failed to get buffer size");
                return NULL;
            }
            w = (int)fhdr.layer_info.xsize;
            h = (int)fhdr.layer_info.ysize;

            if (overflow2(w, h) || overflow2(w * h, 4)) {
                gd_error("gdJxlReadNextFrame: frame dimensions overflow");
                return NULL;
            }

            /* Verify pixels_size matches expected */
            if (pixels_size != (size_t)w * (size_t)h * 4) {
                gd_error("gdJxlReadNextFrame: unexpected buffer size");
                return NULL;
            }

            pixels = (uint8_t *)gdMalloc(pixels_size);
            if (pixels == NULL) {
                gd_error("gdJxlReadNextFrame: pixel buffer allocation failed");
                return NULL;
            }

            if (JxlDecoderSetImageOutBuffer(dec, &fmt, pixels, pixels_size) != JXL_DEC_SUCCESS) {
                gd_error("gdJxlReadNextFrame: failed to set output buffer");
                gdFree(pixels);
                return NULL;
            }
        } else if (status == JXL_DEC_FULL_IMAGE) {
            if (pixels == NULL) {
                gd_error("gdJxlReadNextFrame: incomplete decoded frame");
                return NULL;
            }
            w = info->width;
            h = info->height;
            im = JxlImageFromRGBA(pixels, w, h, has_alpha);
            if (pixels)
                gdFree(pixels);
            if (reader->last_frame_seen)
                reader->done = 1;
            return im;
        } else if (status == JXL_DEC_SUCCESS) {
            reader->done = 1;
            if (pixels)
                gdFree(pixels);
            return NULL;
        } else if (status == JXL_DEC_ERROR) {
            gd_error("gdJxlReadNextFrame: decoder error");
            if (pixels)
                gdFree(pixels);
            return NULL;
        }
    }
}

BGD_DECLARE(int)
gdJxlReadNextFrame(gdJxlReadPtr reader, gdJxlFrameInfo *info, gdImagePtr *frame)
{
    if (reader == NULL || reader->coalesced != 0 || info == NULL) {
        return -1;
    }
    if (frame != NULL) {
        *frame = NULL;
    }
    if (reader->done) {
        return 0;
    }
    if (frame == NULL) {
        gdImagePtr ignored = JxlReadGetCurrentRawFrame(reader, info);
        if (ignored == NULL) {
            return 0;
        }
        gdImageDestroy(ignored);
        return 1;
    }
    *frame = JxlReadGetCurrentRawFrame(reader, info);
    return *frame != NULL ? 1 : 0;
}

BGD_DECLARE(void) gdJxlReadClose(gdJxlReadPtr reader)
{
    if (reader == NULL) {
        return;
    }
    if (reader->dec)
        JxlDecoderDestroy(reader->dec);
    if (reader->buf)
        gdFree(reader->buf);
    gdFree(reader);
}

#else /* !HAVE_LIBJXL */

static void _noJxlError(void) { gd_error("JXL image support has been disabled\n"); }

BGD_DECLARE(void) gdJxlReadOptionsInit(gdJxlReadOptions *options)
{
    if (options == NULL) {
        return;
    }
    memset(options, 0, sizeof(*options));
    options->coalesced = 1;
}

BGD_DECLARE(void) gdJxlWriteOptionsInit(gdJxlWriteOptions *options)
{
    if (options == NULL) {
        return;
    }
    memset(options, 0, sizeof(*options));
    options->distance = 1.0f;
    options->effort = 7;
}

BGD_DECLARE(void) gdJxlAnimWriteOptionsInit(gdJxlAnimWriteOptions *options)
{
    if (options == NULL) {
        return;
    }
    memset(options, 0, sizeof(*options));
    options->distance = 1.0f;
    options->effort = 7;
}

BGD_DECLARE(gdImagePtr) gdImageCreateFromJxl(FILE *inFile)
{
    ARG_NOT_USED(inFile);
    _noJxlError();
    return NULL;
}

BGD_DECLARE(gdImagePtr) gdImageCreateFromJxlPtr(int size, void *data)
{
    ARG_NOT_USED(size);
    ARG_NOT_USED(data);
    _noJxlError();
    return NULL;
}

BGD_DECLARE(gdImagePtr) gdImageCreateFromJxlCtx(gdIOCtx *infile)
{
    ARG_NOT_USED(infile);
    _noJxlError();
    return NULL;
}

BGD_DECLARE(void) gdImageJxl(gdImagePtr im, FILE *outFile)
{
    ARG_NOT_USED(im);
    ARG_NOT_USED(outFile);
    _noJxlError();
}

BGD_DECLARE(int)
gdImageJxlWithOptions(gdImagePtr im, FILE *outFile, const gdJxlWriteOptions *options)
{
    ARG_NOT_USED(im);
    ARG_NOT_USED(outFile);
    ARG_NOT_USED(options);
    _noJxlError();
    return 0;
}

BGD_DECLARE(int) gdJxlReadGetMetadata(gdJxlReadPtr reader, gdImageMetadata *metadata)
{
    ARG_NOT_USED(reader);
    ARG_NOT_USED(metadata);
    _noJxlError();
    return GD_META_ERR_UNSUPPORTED;
}

BGD_DECLARE(int)
gdImageJxlCtxWithOptions(gdImagePtr im, gdIOCtxPtr outfile, const gdJxlWriteOptions *options)
{
    ARG_NOT_USED(im);
    ARG_NOT_USED(outfile);
    ARG_NOT_USED(options);
    _noJxlError();
    return 0;
}

BGD_DECLARE(void *)
gdImageJxlPtrWithOptions(gdImagePtr im, int *size, const gdJxlWriteOptions *options)
{
    ARG_NOT_USED(im);
    ARG_NOT_USED(size);
    ARG_NOT_USED(options);
    _noJxlError();
    return NULL;
}

BGD_DECLARE(void)
gdImageJxlEx(gdImagePtr im, FILE *outFile, int lossless, float distance, int effort)
{
    ARG_NOT_USED(im);
    ARG_NOT_USED(outFile);
    ARG_NOT_USED(lossless);
    ARG_NOT_USED(distance);
    ARG_NOT_USED(effort);
    _noJxlError();
}

BGD_DECLARE(void *) gdImageJxlPtr(gdImagePtr im, int *size)
{
    ARG_NOT_USED(im);
    ARG_NOT_USED(size);
    _noJxlError();
    return NULL;
}

BGD_DECLARE(void *)
gdImageJxlPtrEx(gdImagePtr im, int *size, int lossless, float distance, int effort)
{
    ARG_NOT_USED(im);
    ARG_NOT_USED(size);
    ARG_NOT_USED(lossless);
    ARG_NOT_USED(distance);
    ARG_NOT_USED(effort);
    _noJxlError();
    return NULL;
}

BGD_DECLARE(void) gdImageJxlCtx(gdImagePtr im, gdIOCtxPtr outfile)
{
    ARG_NOT_USED(im);
    ARG_NOT_USED(outfile);
    _noJxlError();
}

BGD_DECLARE(void)
gdImageJxlCtxEx(gdImagePtr im, gdIOCtxPtr outfile, int lossless, float distance, int effort)
{
    ARG_NOT_USED(im);
    ARG_NOT_USED(outfile);
    ARG_NOT_USED(lossless);
    ARG_NOT_USED(distance);
    ARG_NOT_USED(effort);
    _noJxlError();
}

/* Animation stubs */
BGD_DECLARE(gdJxlReadPtr) gdJxlReadOpen(FILE *inFile, const gdJxlReadOptions *options)
{
    ARG_NOT_USED(inFile);
    ARG_NOT_USED(options);
    _noJxlError();
    return NULL;
}

BGD_DECLARE(gdJxlReadPtr) gdJxlReadOpenCtx(gdIOCtxPtr inCtx, const gdJxlReadOptions *options)
{
    ARG_NOT_USED(inCtx);
    ARG_NOT_USED(options);
    _noJxlError();
    return NULL;
}

BGD_DECLARE(gdJxlReadPtr)
gdJxlReadOpenPtr(int size, void *data, const gdJxlReadOptions *options)
{
    ARG_NOT_USED(size);
    ARG_NOT_USED(data);
    ARG_NOT_USED(options);
    _noJxlError();
    return NULL;
}

BGD_DECLARE(int) gdJxlReadGetInfo(gdJxlReadPtr reader, gdJxlInfo *info)
{
    ARG_NOT_USED(reader);
    ARG_NOT_USED(info);
    _noJxlError();
    return 0;
}

BGD_DECLARE(int)
gdJxlReadNextImage(gdJxlReadPtr reader, int *delay_ms, gdImagePtr *image)
{
    ARG_NOT_USED(reader);
    ARG_NOT_USED(delay_ms);
    if (image != NULL) {
        *image = NULL;
    }
    _noJxlError();
    return -1;
}

BGD_DECLARE(int)
gdJxlReadNextFrame(gdJxlReadPtr reader, gdJxlFrameInfo *info, gdImagePtr *frame)
{
    ARG_NOT_USED(reader);
    ARG_NOT_USED(info);
    if (frame != NULL) {
        *frame = NULL;
    }
    _noJxlError();
    return -1;
}

BGD_DECLARE(void) gdJxlReadClose(gdJxlReadPtr reader)
{
    ARG_NOT_USED(reader);
    _noJxlError();
}

BGD_DECLARE(gdJxlWritePtr) gdJxlWriteOpen(FILE *outFile, const gdJxlAnimWriteOptions *options)
{
    ARG_NOT_USED(outFile);
    ARG_NOT_USED(options);
    _noJxlError();
    return NULL;
}

BGD_DECLARE(gdJxlWritePtr) gdJxlWriteOpenCtx(gdIOCtxPtr outCtx, const gdJxlAnimWriteOptions *options)
{
    ARG_NOT_USED(outCtx);
    ARG_NOT_USED(options);
    _noJxlError();
    return NULL;
}

BGD_DECLARE(gdJxlWritePtr) gdJxlWriteOpenPtr(const gdJxlAnimWriteOptions *options)
{
    ARG_NOT_USED(options);
    _noJxlError();
    return NULL;
}

BGD_DECLARE(int) gdJxlWriteAddImage(gdJxlWritePtr writer, gdImagePtr image, int delay_ms)
{
    ARG_NOT_USED(writer);
    ARG_NOT_USED(image);
    ARG_NOT_USED(delay_ms);
    _noJxlError();
    return 0;
}

BGD_DECLARE(void) gdJxlWriteClose(gdJxlWritePtr writer)
{
    ARG_NOT_USED(writer);
    _noJxlError();
}

BGD_DECLARE(void *) gdJxlWritePtrFinish(gdJxlWritePtr writer, int *size)
{
    ARG_NOT_USED(writer);
    if (size != NULL) {
        *size = 0;
    }
    _noJxlError();
    return NULL;
}

#endif /* HAVE_LIBJXL */
