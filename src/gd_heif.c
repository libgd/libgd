/**
 * File: HEIF IO
 *
 * Read and write HEIF images.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif /* HAVE_CONFIG_H */

#include "gd.h"
#include "gd_errors.h"
#include "gdhelpers.h"
#include "gd_heif_metadata.h"
#include <limits.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef HAVE_LIBHEIF
#include <libheif/heif.h>

#define GD_HEIF_ALLOC_STEP (4 * 1024)
#define GD_HEIF_HEADER 12

typedef enum gd_heif_brand {
    GD_HEIF_BRAND_AVIF = 1,
    GD_HEIF_BRAND_MIF1 = 2,
    GD_HEIF_BRAND_HEIC = 4,
    GD_HEIF_BRAND_HEIX = 8,
} gd_heif_brand;

static gdImagePtr _gdImageCreateFromHeifCtx(gdIOCtx *infile, gd_heif_brand expected_brand,
                                            const gdHeifReadOptions *options);

BGD_DECLARE(void) gdHeifReadOptionsInit(gdHeifReadOptions *options)
{
    if (options == NULL)
        return;
    memset(options, 0, sizeof(*options));
    options->ignore_transformations = GD_TRUE;
}

BGD_DECLARE(void) gdHeifWriteOptionsInit(gdHeifWriteOptions *options)
{
    if (options == NULL)
        return;
    memset(options, 0, sizeof(*options));
    options->quality = -1;
    options->lossless = GD_FALSE;
    options->codec = GD_HEIF_CODEC_HEVC;
    options->chroma = GD_HEIF_CHROMA_444;
}

BGD_DECLARE(gdImagePtr) gdImageCreateFromHeif(FILE *inFile)
{
    gdImagePtr im;
    gdIOCtx *in = gdNewFileCtx(inFile);

    if (!in)
        return NULL;
    im = gdImageCreateFromHeifCtx(in);
    in->gd_free(in);

    return im;
}

BGD_DECLARE(gdImagePtr) gdImageCreateFromHeifPtr(int size, void *data)
{
    return gdImageCreateFromHeifPtrWithOptions(size, data, NULL);
}

BGD_DECLARE(gdImagePtr)
gdImageCreateFromHeifPtrWithOptions(int size, void *data, const gdHeifReadOptions *options)
{
    gdImagePtr im;
    gdIOCtx *in = gdNewDynamicCtxEx(size, data, 0);

    if (!in)
        return NULL;
    im = _gdImageCreateFromHeifCtx(in, GD_HEIF_BRAND_AVIF | GD_HEIF_BRAND_MIF1 |
                                           GD_HEIF_BRAND_HEIC | GD_HEIF_BRAND_HEIX, options);
    in->gd_free(in);

    return im;
}

static int _gdHeifCheckBrand(unsigned char *magic, gd_heif_brand expected_brand)
{
    if (memcmp(magic + 4, "ftyp", 4) != 0)
        return GD_FALSE;
    if (memcmp(magic + 8, "avif", 4) == 0 && expected_brand & GD_HEIF_BRAND_AVIF)
        return GD_TRUE;
    if (memcmp(magic + 8, "heic", 4) == 0 && expected_brand & GD_HEIF_BRAND_HEIC)
        return GD_TRUE;
    if (memcmp(magic + 8, "heix", 4) == 0 && expected_brand & GD_HEIF_BRAND_HEIX)
        return GD_TRUE;
    if (memcmp(magic + 8, "mif1", 4) == 0 && expected_brand & GD_HEIF_BRAND_MIF1)
        return GD_TRUE;

    return GD_FALSE;
}

static gdImagePtr _gdImageCreateFromHeifCtx(gdIOCtx *infile, gd_heif_brand expected_brand,
                                            const gdHeifReadOptions *options)
{
    struct heif_context *heif_ctx;
    struct heif_decoding_options *heif_dec_opts;
    struct heif_image_handle *heif_imhandle;
    struct heif_image *heif_im;
    struct heif_error err;
    int width, height;
    uint8_t *filedata = NULL;
    uint8_t *rgba = NULL;
    unsigned char *read, *temp, magic[GD_HEIF_HEADER];
    int magic_len;
    size_t size = 0, n = GD_HEIF_ALLOC_STEP;
    gdImagePtr im;
    int x, y;
    uint8_t *p, *row_start;
    int stride;
    gdHeifReadOptions default_options;

    if (options == NULL) {
        gdHeifReadOptionsInit(&default_options);
        options = &default_options;
    }

    magic_len = gdGetBuf(magic, GD_HEIF_HEADER, infile);
    if (magic_len != GD_HEIF_HEADER || !_gdHeifCheckBrand(magic, expected_brand)) {
        gd_error("gd-heif incorrect type of file\n");
        return NULL;
    }
    gdSeek(infile, 0);

    while (n == GD_HEIF_ALLOC_STEP) {
        temp = gdRealloc(filedata, size + GD_HEIF_ALLOC_STEP);
        if (temp) {
            filedata = temp;
            read = temp + size;
        } else {
            gdFree(filedata);
            gd_error("gd-heif decode realloc failed\n");
            return NULL;
        }

        n = gdGetBuf(read, GD_HEIF_ALLOC_STEP, infile);
        if (n > 0) {
            size += n;
        }
    }

    heif_ctx = heif_context_alloc();
    if (heif_ctx == NULL) {
        gd_error("gd-heif could not allocate context\n");
        gdFree(filedata);
        return NULL;
    }
    err = heif_context_read_from_memory_without_copy(heif_ctx, filedata, size, NULL);
    if (err.code != heif_error_Ok) {
        gd_error("gd-heif context creation failed\n");
        gdFree(filedata);
        heif_context_free(heif_ctx);
        return NULL;
    }

    heif_imhandle = NULL;
    err = heif_context_get_primary_image_handle(heif_ctx, &heif_imhandle);
    if (err.code != heif_error_Ok) {
        gd_error("gd-heif cannot retrieve handle\n");
        gdFree(filedata);
        heif_context_free(heif_ctx);
        return NULL;
    }

    heif_im = NULL;
    heif_dec_opts = heif_decoding_options_alloc();
    if (heif_dec_opts == NULL) {
        gd_error("gd-heif could not allocate decode options\n");
        gdFree(filedata);
        heif_image_handle_release(heif_imhandle);
        heif_context_free(heif_ctx);
        return NULL;
    }

    heif_dec_opts->convert_hdr_to_8bit = GD_TRUE;
    heif_dec_opts->ignore_transformations = options->ignore_transformations;
    err = heif_decode_image(heif_imhandle, &heif_im, heif_colorspace_RGB,
                            heif_chroma_interleaved_RGBA, heif_dec_opts);
    heif_decoding_options_free(heif_dec_opts);
    if (err.code != heif_error_Ok) {
        gd_error("gd-heif decoding failed\n");
        gdFree(filedata);
        heif_image_handle_release(heif_imhandle);
        heif_context_free(heif_ctx);
        return NULL;
    }

    width = heif_image_get_width(heif_im, heif_channel_interleaved);
    height = heif_image_get_height(heif_im, heif_channel_interleaved);

    im = gdImageCreateTrueColor(width, height);
    if (!im) {
        gdFree(filedata);
        heif_image_release(heif_im);
        heif_image_handle_release(heif_imhandle);
        heif_context_free(heif_ctx);
        return NULL;
    }
    rgba = (uint8_t *)heif_image_get_plane_readonly(heif_im, heif_channel_interleaved, &stride);
    if (!rgba) {
        gd_error("gd-heif cannot get image plane\n");
        gdFree(filedata);
        heif_image_release(heif_im);
        heif_image_handle_release(heif_imhandle);
        heif_context_free(heif_ctx);
        gdImageDestroy(im);
        return NULL;
    }
    row_start = rgba;
    for (y = 0, p = rgba; y < height; y++) {
        p = row_start;
        for (x = 0; x < width; x++) {
            uint8_t r = *(p++);
            uint8_t g = *(p++);
            uint8_t b = *(p++);
            uint8_t a = gdAlphaMax - (*(p++) >> 1);
            im->tpixels[y][x] = gdTrueColorAlpha(r, g, b, a);
        }
        row_start += stride;
    }
    gdFree(filedata);
    heif_image_release(heif_im);
    heif_image_handle_release(heif_imhandle);
    heif_context_free(heif_ctx);

    return im;
}

BGD_DECLARE(gdImagePtr) gdImageCreateFromHeifCtx(gdIOCtx *infile)
{
    return _gdImageCreateFromHeifCtx(infile, GD_HEIF_BRAND_AVIF | GD_HEIF_BRAND_MIF1 |
                                                 GD_HEIF_BRAND_HEIC | GD_HEIF_BRAND_HEIX,
                                                 NULL);
}

static struct heif_error _gdImageWriteHeif(struct heif_context *heif_ctx, const void *data,
                                           size_t size, void *userdata)
{
    ARG_NOT_USED(heif_ctx);
    gdIOCtx *outfile;
    struct heif_error err;
    int bytes_written;

    outfile = (gdIOCtx *)userdata;
    if (outfile == NULL || data == NULL || size > INT_MAX) {
        err.code = heif_error_Encoding_error;
        err.subcode = heif_suberror_Cannot_write_output_data;
        err.message = "gd-heif write callback received invalid arguments";
        return err;
    }

    bytes_written = gdPutBuf(data, (int)size, outfile);
    if (bytes_written != (int)size) {
        err.code = heif_error_Encoding_error;
        err.subcode = heif_suberror_Cannot_write_output_data;
        err.message = "gd-heif failed to write output data";
        return err;
    }

    err.code = heif_error_Ok;
    err.subcode = heif_suberror_Unspecified;
    err.message = "";

    return err;
}

/* returns GD_TRUE on success, GD_FALSE on failure */
static int _gdImageHeifCtx(gdImagePtr im, gdIOCtx *outfile, const gdHeifWriteOptions *options)
{
    struct heif_context *heif_ctx;
    struct heif_encoder *heif_enc;
    struct heif_image *heif_im;
    struct heif_image_handle *heif_imhandle = NULL;
    struct heif_writer heif_wr;
    struct heif_error err;
    uint8_t *rgba;
    int x, y;
    uint8_t *p;
    uint8_t *row_start;
    int stride;
    gdHeifWriteOptions default_options;
    int quality;
    gdHeifCodec codec;
    gdHeifChroma chroma;
    int lossless;

    if (options == NULL) {
        gdHeifWriteOptionsInit(&default_options);
        options = &default_options;
    }

    quality = options->quality;
    codec = options->codec;
    chroma = options->chroma;
    lossless = options->lossless || quality == 200;

    if (im == NULL) {
        return GD_FALSE;
    }

    if (codec != GD_HEIF_CODEC_HEVC && codec != GD_HEIF_CODEC_AV1) {
        gd_error("Unsupported format by heif");
        return GD_FALSE;
    }

    if (!gdImageTrueColor(im)) {
        gd_error("Palette image not supported by heif\n");
        return GD_FALSE;
    }

    if (overflow2(gdImageSX(im), 4)) {
        return GD_FALSE;
    }

    if (overflow2(gdImageSX(im) * 4, gdImageSY(im))) {
        return GD_FALSE;
    }

    heif_ctx = heif_context_alloc();
    if (heif_ctx == NULL) {
        gd_error("gd-heif could not allocate context\n");
        return GD_FALSE;
    }
    err = heif_context_get_encoder_for_format(heif_ctx, (enum heif_compression_format)codec,
                                              &heif_enc);
    if (err.code != heif_error_Ok) {
        gd_error("gd-heif encoder acquisition failed (missing codec support?: "
                 "code: %d, subcode: %d, message: %s)\n",
                 err.code, err.subcode, err.message);
        heif_context_free(heif_ctx);
        return GD_FALSE;
    }

    if (lossless) {
        err = heif_encoder_set_lossless(heif_enc, GD_TRUE);
    } else if (quality == -1) {
        err = heif_encoder_set_lossy_quality(heif_enc, 80);
    } else {
        err = heif_encoder_set_lossy_quality(heif_enc, quality);
    }
    if (err.code != heif_error_Ok) {
        gd_error("gd-heif invalid quality number\n");
        heif_encoder_release(heif_enc);
        heif_context_free(heif_ctx);
        return GD_FALSE;
    }

    if (heif_get_version_number_major() >= 1 && heif_get_version_number_minor() >= 9) {
        err = heif_encoder_set_parameter_string(heif_enc, "chroma", chroma);
        if (err.code != heif_error_Ok) {
            gd_error("gd-heif invalid chroma subsampling parameter\n");
            heif_encoder_release(heif_enc);
            heif_context_free(heif_ctx);
            return GD_FALSE;
        }
    }

    err = heif_image_create(gdImageSX(im), gdImageSY(im), heif_colorspace_RGB,
                            heif_chroma_interleaved_RGBA, &heif_im);
    if (err.code != heif_error_Ok) {
        gd_error("gd-heif image creation failed");
        heif_encoder_release(heif_enc);
        heif_context_free(heif_ctx);
        return GD_FALSE;
    }

    err = heif_image_add_plane(heif_im, heif_channel_interleaved, gdImageSX(im), gdImageSY(im), 32);
    if (err.code != heif_error_Ok) {
        gd_error("gd-heif cannot add image plane\n");
        heif_image_release(heif_im);
        heif_encoder_release(heif_enc);
        heif_context_free(heif_ctx);
        return GD_FALSE;
    }

    rgba = (uint8_t *)heif_image_get_plane_readonly(heif_im, heif_channel_interleaved, &stride);
    if (!rgba) {
        gd_error("gd-heif cannot get image plane\n");
        heif_image_release(heif_im);
        heif_encoder_release(heif_enc);
        heif_context_free(heif_ctx);
        return GD_FALSE;
    }
    row_start = rgba;
    for (y = 0; y < gdImageSY(im); y++) {
        p = row_start;
        for (x = 0; x < gdImageSX(im); x++) {
            int c;
            char a;
            c = im->tpixels[y][x];
            a = gdTrueColorGetAlpha(c);
            if (a == 127) {
                a = 0;
            } else {
                a = 255 - ((a << 1) + (a >> 6));
            }
            *(p++) = gdTrueColorGetRed(c);
            *(p++) = gdTrueColorGetGreen(c);
            *(p++) = gdTrueColorGetBlue(c);
            *(p++) = a;
        }
        row_start += stride;
    }
    err = heif_context_encode_image(heif_ctx, heif_im, heif_enc, NULL, &heif_imhandle);
    heif_encoder_release(heif_enc);
    if (err.code != heif_error_Ok || heif_imhandle == NULL) {
        gd_error("gd-heif encoding failed\n");
        heif_image_release(heif_im);
        heif_context_free(heif_ctx);
        return GD_FALSE;
    }
    if (options->metadata != NULL && gdHeifApplyMetadata(heif_ctx, heif_im, heif_imhandle, options->metadata) != GD_META_OK) {
        gd_error("gd-heif metadata application failed\n");
        heif_image_handle_release(heif_imhandle);
        heif_image_release(heif_im);
        heif_context_free(heif_ctx);
        return GD_FALSE;
    }
    heif_wr.write = _gdImageWriteHeif;
    heif_wr.writer_api_version = 1;
    err = heif_context_write(heif_ctx, &heif_wr, (void *)outfile);

    heif_image_handle_release(heif_imhandle);
    heif_image_release(heif_im);
    heif_context_free(heif_ctx);
    if (err.code != heif_error_Ok) {
        gd_error("gd-heif write failed (code: %d, subcode: %d, message: %s)\n", err.code,
                 err.subcode, err.message);
        return GD_FALSE;
    }

    return GD_TRUE;
}

BGD_DECLARE(void)
gdImageHeifCtx(gdImagePtr im, gdIOCtx *outfile, int quality, gdHeifCodec codec, gdHeifChroma chroma)
{
    gdHeifWriteOptions options;

    gdHeifWriteOptionsInit(&options);
    options.quality = quality;
    options.lossless = quality == 200;
    options.codec = codec;
    options.chroma = chroma;
    _gdImageHeifCtx(im, outfile, &options);
}

BGD_DECLARE(int)
gdImageHeifCtxWithOptions(gdImagePtr im, gdIOCtx *outfile, const gdHeifWriteOptions *options)
{
    if (im == NULL || outfile == NULL) {
        return GD_FALSE;
    }
    return _gdImageHeifCtx(im, outfile, options) ? 0 : 1;
}

BGD_DECLARE(int)
gdImageHeifWithOptions(gdImagePtr im, FILE *outFile, const gdHeifWriteOptions *options)
{
    gdIOCtx *out;
    int status;

    if (im == NULL || outFile == NULL) {
        return 1;
    }
    out = gdNewFileCtx(outFile);
    if (out == NULL) {
        return 1;
    }
    status = gdImageHeifCtxWithOptions(im, out, options);
    out->gd_free(out);
    return status;
}

BGD_DECLARE(void)
gdImageHeifEx(gdImagePtr im, FILE *outFile, int quality, gdHeifCodec codec, gdHeifChroma chroma)
{
    gdIOCtx *out = gdNewFileCtx(outFile);
    gdHeifWriteOptions options;

    if (out == NULL) {
        return;
    }
    gdHeifWriteOptionsInit(&options);
    options.quality = quality;
    options.lossless = quality == 200;
    options.codec = codec;
    options.chroma = chroma;
    _gdImageHeifCtx(im, out, &options);
    out->gd_free(out);
}

BGD_DECLARE(void) gdImageHeif(gdImagePtr im, FILE *outFile)
{
    gdIOCtx *out = gdNewFileCtx(outFile);
    gdHeifWriteOptions options;

    if (out == NULL) {
        return;
    }
    gdHeifWriteOptionsInit(&options);
    _gdImageHeifCtx(im, out, &options);
    out->gd_free(out);
}

BGD_DECLARE(void *) gdImageHeifPtr(gdImagePtr im, int *size)
{
    gdHeifWriteOptions options;

    gdHeifWriteOptionsInit(&options);
    return gdImageHeifPtrWithOptions(im, size, &options);
}

BGD_DECLARE(void *)
gdImageHeifPtrWithOptions(gdImagePtr im, int *size, const gdHeifWriteOptions *options)
{
    void *rv;
    gdIOCtx *out = gdNewDynamicCtx(2048, NULL);
    if (out == NULL) {
        return NULL;
    }
    if (_gdImageHeifCtx(im, out, options)) {
        rv = gdDPExtractData(out, size);
    } else {
        rv = NULL;
    }
    out->gd_free(out);

    return rv;
}

BGD_DECLARE(void *)
gdImageHeifPtrEx(gdImagePtr im, int *size, int quality, gdHeifCodec codec, gdHeifChroma chroma)
{
    gdHeifWriteOptions options;

    gdHeifWriteOptionsInit(&options);
    options.quality = quality;
    options.lossless = quality == 200;
    options.codec = codec;
    options.chroma = chroma;
    return gdImageHeifPtrWithOptions(im, size, &options);
}

#else /* HAVE_LIBHEIF */

static void _noHeifError(void) { gd_error("HEIF image support has been disabled\n"); }

BGD_DECLARE(void) gdHeifReadOptionsInit(gdHeifReadOptions *options)
{
    if (options == NULL)
        return;
    memset(options, 0, sizeof(*options));
    options->ignore_transformations = GD_TRUE;
}

BGD_DECLARE(void) gdHeifWriteOptionsInit(gdHeifWriteOptions *options)
{
    if (options == NULL)
        return;
    memset(options, 0, sizeof(*options));
    options->quality = -1;
    options->lossless = GD_FALSE;
    options->codec = GD_HEIF_CODEC_HEVC;
    options->chroma = GD_HEIF_CHROMA_444;
}

BGD_DECLARE(gdImagePtr) gdImageCreateFromHeif(FILE *inFile)
{
    _noHeifError();
    return NULL;
}

BGD_DECLARE(gdImagePtr) gdImageCreateFromHeifPtr(int size, void *data)
{
    _noHeifError();
    return NULL;
}

BGD_DECLARE(gdImagePtr)
gdImageCreateFromHeifPtrWithOptions(int size, void *data, const gdHeifReadOptions *options)
{
    _noHeifError();
    return NULL;
}

BGD_DECLARE(gdImagePtr) gdImageCreateFromHeifCtx(gdIOCtx *infile)
{
    _noHeifError();
    return NULL;
}

BGD_DECLARE(void)
gdImageHeifCtx(gdImagePtr im, gdIOCtx *outfile, int quality, gdHeifCodec codec, gdHeifChroma chroma)
{
    _noHeifError();
}

BGD_DECLARE(void)
gdImageHeifEx(gdImagePtr im, FILE *outFile, int quality, gdHeifCodec codec, gdHeifChroma chroma)
{
    _noHeifError();
}

BGD_DECLARE(void) gdImageHeif(gdImagePtr im, FILE *outFile) { _noHeifError(); }

BGD_DECLARE(void *) gdImageHeifPtr(gdImagePtr im, int *size)
{
    _noHeifError();
    return NULL;
}

BGD_DECLARE(void *)
gdImageHeifPtrEx(gdImagePtr im, int *size, int quality, gdHeifCodec codec, gdHeifChroma chroma)
{
    _noHeifError();
    return NULL;
}

BGD_DECLARE(void *)
gdImageHeifPtrWithOptions(gdImagePtr im, int *size, const gdHeifWriteOptions *options)
{
    _noHeifError();
    return NULL;
}

BGD_DECLARE(int)
gdImageHeifCtxWithOptions(gdImagePtr im, gdIOCtx *outfile, const gdHeifWriteOptions *options)
{
    ARG_NOT_USED(im);
    ARG_NOT_USED(outfile);
    ARG_NOT_USED(options);
    _noHeifError();
    return 1;
}

BGD_DECLARE(int)
gdImageHeifWithOptions(gdImagePtr im, FILE *outFile, const gdHeifWriteOptions *options)
{
    ARG_NOT_USED(im);
    ARG_NOT_USED(outFile);
    ARG_NOT_USED(options);
    _noHeifError();
    return 1;
}

#endif /* HAVE_LIBHEIF */
