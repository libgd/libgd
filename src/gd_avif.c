/**
 * File: AVIF IO
 *
 * Read and write AVIF images using libavif
 * (https://github.com/AOMediaCodec/libavif) . Currently, the only ICC profile
 * we support is sRGB. Since that's what web browsers use, it's sufficient for
 * now.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <limits.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "gd.h"
#include "gd_avif_metadata.h"
#include "gd_errors.h"
#include "gd_intern.h"
#include "gdhelpers.h"

#ifdef HAVE_LIBAVIF
#include <avif/avif.h>

/*
        Define defaults for encoding images:
                CHROMA_SUBSAMPLING_DEFAULT: 4:2:0 is commonly used for Chroma
   subsampling. CHROMA_SUBAMPLING_HIGH_QUALITY: Use 4:4:4, or no subsampling,
   when a sufficient high quality is requested.
                SUBAMPLING_HIGH_QUALITY_THRESHOLD: At or above this value, use
   CHROMA_SUBAMPLING_HIGH_QUALITY QUANTIZER_DEFAULT: We need more testing to
   really know what quantizer settings are optimal, but teams at Google have
   been using maximum=30 as a starting point. QUALITY_DEFAULT: following gd
   conventions, -1 indicates the default. SPEED_DEFAULT: AVIF_SPEED_DEFAULT is
   simply the default encoding speed of the AV1 codec. This could be as slow as
   0. So we use 6, which is currently considered to be a fine default.

*/

#define CHROMA_SUBSAMPLING_DEFAULT AVIF_PIXEL_FORMAT_YUV420
#define CHROMA_SUBAMPLING_HIGH_QUALITY AVIF_PIXEL_FORMAT_YUV444
#define HIGH_QUALITY_SUBSAMPLING_THRESHOLD 90
#define QUANTIZER_DEFAULT 30
#define QUALITY_DEFAULT -1
#define SPEED_DEFAULT 6

// This initial size for the gdIOCtx is standard among GD image conversion
// functions.
#define NEW_DYNAMIC_CTX_SIZE 2048

// Our quality param ranges from 0 to 100.
// To calculate quality, we convert from AVIF's quantizer scale, which runs from
// 63 to 0.
#define MAX_QUALITY 100

// These constants are for computing the number of tiles and threads to use
// during encoding. Maximum threads are from
// libavif/contrib/gkd-pixbuf/loader.c.
#define MIN_TILE_AREA (512 * 512)
#define MAX_TILES 8
#define MAX_THREADS 64

/*** Macros ***/

/*
        From gd_png.c:
                convert the 7-bit alpha channel to an 8-bit alpha channel.
                We do a little bit-flipping magic, repeating the MSB
                as the LSB, to ensure that 0 maps to 0 and
                127 maps to 255. We also have to invert to match
                PNG's convention in which 255 is opaque.
*/
#define alpha7BitTo8Bit(alpha7Bit)                                                                 \
    (alpha7Bit == 127 ? 0 : 255 - ((alpha7Bit << 1) + (alpha7Bit >> 6)))

#define alpha8BitTo7Bit(alpha8Bit) (gdAlphaMax - (alpha8Bit >> 1))

/*** Helper functions ***/

/* Convert the quality param we expose to the quantity params used by libavif.
         The *Quantizer* params values can range from 0 to 63, with 0 = highest
   quality and 63 = worst. We make the scale 0-100, and we reverse this, so that
   0 = worst quality and 100 = highest.

         Values below 0 are set to 0, and values below MAX_QUALITY are set to
   MAX_QUALITY.
*/
static int quality2Quantizer(int quality)
{
    int clampedQuality = CLAMP(quality, 0, MAX_QUALITY);

    float scaleFactor = (float)AVIF_QUANTIZER_WORST_QUALITY / (float)MAX_QUALITY;

    return round(scaleFactor * (MAX_QUALITY - clampedQuality));
}

BGD_DECLARE(void) gdAvifWriteOptionsInit(gdAvifWriteOptions *options)
{
    if (options == NULL)
        return;
    memset(options, 0, sizeof(*options));
    options->quality = QUALITY_DEFAULT;
    options->speed = SPEED_DEFAULT;
    options->lossless = GD_FALSE;
    options->chroma_subsampling = GD_AVIF_CHROMA_SUBSAMPLING_AUTO;
}

/*
         As of February 2021, this algorithm reflects the latest research on how
   many tiles and threads to include for a given image size. This is subject to
   change as research continues.

         Returns false if there was an error, true if all was well.
 */
static avifBool setEncoderTilesAndThreads(avifEncoder *encoder, avifRGBImage *rgb)
{
    int imageArea, tiles, tilesLog2, encoderTiles;

    // _gdImageAvifCtx(), the calling function, checks this operation for
    // overflow
    imageArea = rgb->width * rgb->height;

    tiles = (int)ceil((double)imageArea / MIN_TILE_AREA);
    tiles = MIN(tiles, MAX_TILES);
    tiles = MIN(tiles, MAX_THREADS);

    // The number of tiles in any dimension will always be a power of 2. We can
    // only specify log(2)tiles.

    tilesLog2 = floor(log2(tiles));

    // If the image's width is greater than the height, use more tile columns
    // than tile rows to make the tile size close to a square.

    if (rgb->width >= rgb->height) {
        encoder->tileRowsLog2 = tilesLog2 / 2;
        encoder->tileColsLog2 = tilesLog2 - encoder->tileRowsLog2;
    } else {
        encoder->tileColsLog2 = tilesLog2 / 2;
        encoder->tileRowsLog2 = tilesLog2 - encoder->tileColsLog2;
    }

    // It's good to have one thread per tile.
    encoderTiles = (1 << encoder->tileRowsLog2) * (1 << encoder->tileColsLog2);
    encoder->maxThreads = encoderTiles;

    return AVIF_TRUE;
}

/*
         We can handle AVIF images whose color profile is sRGB, or whose color
   profile isn't set.
*/
static avifBool isAvifSrgbImage(avifImage *avifIm)
{
    return (avifIm->colorPrimaries == AVIF_COLOR_PRIMARIES_BT709 ||
            avifIm->colorPrimaries == AVIF_COLOR_PRIMARIES_UNSPECIFIED) &&
           (avifIm->transferCharacteristics == AVIF_TRANSFER_CHARACTERISTICS_SRGB ||
            avifIm->transferCharacteristics == AVIF_TRANSFER_CHARACTERISTICS_UNSPECIFIED);
}

/*
         Check the result from an Avif function to see if it's an error.
         If so, decode the error and output it, and return true.
         Otherwise, return false.
*/
static avifBool isAvifError(avifResult result, const char *msg)
{
    if (result != AVIF_RESULT_OK) {
        gd_error("avif error - %s: %s\n", msg, avifResultToString(result));
        return AVIF_TRUE;
    }

    return AVIF_FALSE;
}

typedef struct avifIOCtxReader {
    avifIO io; // this must be the first member for easy casting to avifIO*
    avifROData rodata;
} avifIOCtxReader;

/*
        <readfromCtx> implements the avifIOReadFunc interface by calling the
   relevant functions in the gdIOCtx. Our logic is inspired by
   avifIOMemoryReaderRead() and avifIOFileReaderRead(). We don't know whether
   we're reading from a file or from memory. We don't have to know, since we
   rely on the helper functions in the gdIOCtx. We assume we've stashed the
   gdIOCtx in io->data, as we do in createAvifIOFromCtx().

        We ignore readFlags, just as the avifIO*ReaderRead() functions do.

        If there's a problem, this returns an avifResult error.
        If things go well, return AVIF_RESULT_OK.
        Of course these AVIF codes shouldn't be returned by any top-level GD
   function.
*/
static avifResult readFromCtx(avifIO *io, uint32_t readFlags, uint64_t offset, size_t size,
                              avifROData *out)
{
    gdIOCtx *ctx = (gdIOCtx *)io->data;
    avifIOCtxReader *reader = (avifIOCtxReader *)io;

    // readFlags is unsupported
    if (readFlags != 0) {
        return AVIF_RESULT_IO_ERROR;
    }

    // TODO: if we set sizeHint, this will be more efficient.

    if (offset > INT_MAX || size > INT_MAX)
        return AVIF_RESULT_IO_ERROR;

    // Try to seek offset bytes forward. If we pass the end of the buffer, throw
    // an error.
    if (!ctx->seek(ctx, (int)offset))
        return AVIF_RESULT_IO_ERROR;

    if (size > reader->rodata.size) {
        void *tmp = gdRealloc((void *)reader->rodata.data, size);
        if (tmp == NULL) {
            gd_error("avif error - couldn't allocate memory");
            return AVIF_RESULT_UNKNOWN_ERROR;
        }
        reader->rodata.data = tmp;
        reader->rodata.size = size;
    }
    if (!reader->rodata.data) {
        gd_error("avif error - couldn't allocate memory");
        return AVIF_RESULT_UNKNOWN_ERROR;
    }

    // Read the number of bytes requested.
    // If getBuf() returns a negative value, that means there was an error.
    int charsRead = ctx->getBuf(ctx, (void *)reader->rodata.data, (int)size);
    if (charsRead < 0) {
        return AVIF_RESULT_IO_ERROR;
    }

    out->data = reader->rodata.data;
    out->size = charsRead;
    return AVIF_RESULT_OK;
}

// avif.h says this is optional, but it seemed easy to implement.
static void destroyAvifIO(struct avifIO *io)
{
    avifIOCtxReader *reader = (avifIOCtxReader *)io;
    if (reader->rodata.data != NULL) {
        gdFree((void *)reader->rodata.data);
    }
    gdFree(reader);
}

/* Set up an avifIO object.
         The functions in the gdIOCtx struct may point either to a file or a memory
   buffer. To us, that's immaterial. Our task is simply to assign avifIO
   functions to the proper functions from gdIOCtx. The destroy function needs to
   destroy the avifIO object and anything else it uses.

         Returns NULL if memory for the object can't be allocated.
*/

// TODO: can we get sizeHint somehow?
static avifIO *createAvifIOFromCtx(gdIOCtx *ctx)
{
    struct avifIOCtxReader *reader;

    reader = gdMalloc(sizeof(*reader));
    if (reader == NULL)
        return NULL;

    // TODO: setting persistent=FALSE is safe, but it's less efficient. Is it
    // necessary?
    reader->io.persistent = AVIF_FALSE;
    reader->io.read = readFromCtx;
    reader->io.write = NULL; // this function is currently unused; see avif.h
    reader->io.destroy = destroyAvifIO;
    reader->io.sizeHint = 0; // sadly, we don't get this information from the gdIOCtx.
    reader->io.data = ctx;
    reader->rodata.data = NULL;
    reader->rodata.size = 0;

    return (avifIO *)reader;
}

BGD_DECLARE(gdImagePtr) gdImageCreateFromAvif(FILE *infile)
{
    gdImagePtr im;
    gdIOCtx *ctx = gdNewFileCtx(infile);

    if (!ctx)
        return NULL;

    im = gdImageCreateFromAvifCtx(ctx);
    ctx->gd_free(ctx);

    return im;
}

BGD_DECLARE(gdImagePtr) gdImageCreateFromAvifPtr(int size, void *data)
{
    gdImagePtr im;
    gdIOCtx *ctx = gdNewDynamicCtxEx(size, data, 0);

    if (!ctx)
        return 0;

    im = gdImageCreateFromAvifCtx(ctx);
    ctx->gd_free(ctx);

    return im;
}

BGD_DECLARE(gdImagePtr) gdImageCreateFromAvifCtx(gdIOCtx *ctx)
{
    uint32_t x, y;
    gdImage *im = NULL;
    avifResult result;
    avifIO *io;
    avifDecoder *decoder;
    avifRGBImage rgb;

    // this lets us know that memory hasn't been allocated yet for the pixels
    rgb.pixels = NULL;

    decoder = avifDecoderCreate();
    if (decoder == NULL) {
        gd_error("avif error - Could not create decoder");
        return NULL;
    }

    // Check if libavif version is >= 0.9.1.
    // If so, allow the PixelInformationProperty ('pixi') to be missing in AV1
    // image items. libheif v1.11.0 or older does not add the 'pixi' item
    // property to AV1 image items. (This issue has been corrected in libheif
    // v1.12.0.)

#if AVIF_VERSION >= 90100
    decoder->strictFlags &= ~AVIF_STRICT_PIXI_REQUIRED;
#endif

    io = createAvifIOFromCtx(ctx);
    if (!io) {
        gd_error("avif error - Could not allocate memory");
        goto cleanup;
    }

    avifDecoderSetIO(decoder, io);

    result = avifDecoderParse(decoder);
    if (isAvifError(result, "Could not parse image"))
        goto cleanup;

    // Note again that, for an image sequence, we read only the first image,
    // ignoring the rest.
    result = avifDecoderNextImage(decoder);
    if (isAvifError(result, "Could not decode image"))
        goto cleanup;

    if (!isAvifSrgbImage(decoder->image))
        gd_error_ex(LOG_NOTICE, "Image's color profile is not sRGB");

    // Set up the avifRGBImage, and convert it from YUV to an 8-bit RGB image.
    // (While AVIF image pixel depth can be 8, 10, or 12 bits, GD truecolor
    // images are 8-bit.)
    avifRGBImageSetDefaults(&rgb, decoder->image);
    rgb.depth = 8;
#if AVIF_VERSION >= 1000000
    result = avifRGBImageAllocatePixels(&rgb);
    if (isAvifError(result, "Allocating RGB pixels failed"))
        goto cleanup;
#else
    avifRGBImageAllocatePixels(&rgb);
#endif

    result = avifImageYUVToRGB(decoder->image, &rgb);
    if (isAvifError(result, "Conversion from YUV to RGB failed"))
        goto cleanup;

    im = gdImageCreateTrueColor(decoder->image->width, decoder->image->height);
    if (!im) {
        gd_error("avif error - Could not create GD truecolor image");
        goto cleanup;
    }

    im->saveAlphaFlag = 1;

    // Read the pixels from the AVIF image and copy them into the GD image.

    uint8_t *p = rgb.pixels;

    for (y = 0; y < decoder->image->height; y++) {
        for (x = 0; x < decoder->image->width; x++) {
            uint8_t r = *(p++);
            uint8_t g = *(p++);
            uint8_t b = *(p++);
            uint8_t a = alpha8BitTo7Bit(*(p++));
            im->tpixels[y][x] = gdTrueColorAlpha(r, g, b, a);
        }
    }

cleanup:
    // if io has been allocated, this frees it
    avifDecoderDestroy(decoder);

    if (rgb.pixels)
        avifRGBImageFreePixels(&rgb);

    return im;
}

/*** Encoding functions ***/

static avifBool _gdImageAvifCtx(gdImagePtr im, gdIOCtx *outfile, const gdAvifWriteOptions *options)
{
    avifResult result;
    avifRGBImage rgb = {0};
    avifRWData avifOutput = AVIF_DATA_EMPTY;
    avifBool failed = AVIF_FALSE;
    avifBool lossless;
    avifEncoder *encoder = NULL;
    avifImage *avifIm = NULL;
    gdAvifWriteOptions default_options;
    int quality, speed, chroma_subsampling, subsampling_quality;

    uint32_t val;
    uint8_t *p;
    uint32_t x, y;

    if (options == NULL) {
        gdAvifWriteOptionsInit(&default_options);
        options = &default_options;
    }

    quality = options->quality;
    speed = options->speed;
    chroma_subsampling = options->chroma_subsampling;
    lossless = options->lossless || quality == 100;
    subsampling_quality = lossless ? 100 : quality;

    if (im == NULL)
        return 1;

    if (!gdImageTrueColor(im)) {
        gd_error("avif doesn't support palette images");
        return 1;
    }

    if (!gdImageSX(im) || !gdImageSY(im)) {
        gd_error("image dimensions must not be zero");
        return 1;
    }

    if (overflow2(gdImageSX(im), gdImageSY(im))) {
        gd_error("image dimensions are too large");
        return 1;
    }

    speed = CLAMP(speed, AVIF_SPEED_SLOWEST, AVIF_SPEED_FASTEST);

    avifPixelFormat subsampling;
    switch (chroma_subsampling) {
    case GD_AVIF_CHROMA_SUBSAMPLING_YUV420:
        subsampling = AVIF_PIXEL_FORMAT_YUV420;
        break;
    case GD_AVIF_CHROMA_SUBSAMPLING_YUV444:
        subsampling = AVIF_PIXEL_FORMAT_YUV444;
        break;
    default:
        subsampling = subsampling_quality >= HIGH_QUALITY_SUBSAMPLING_THRESHOLD
                                      ? CHROMA_SUBAMPLING_HIGH_QUALITY
                                      : CHROMA_SUBSAMPLING_DEFAULT;
        break;
    }


    // Note that MATRIX_COEFFICIENTS_IDENTITY enables lossless conversion from
    // RGB to YUV.

    avifIm = avifImageCreate(gdImageSX(im), gdImageSY(im), 8, subsampling);
    if (avifIm == NULL) {
        gd_error("avif error - Creating image failed\n");
        goto cleanup;
    }

    avifIm->colorPrimaries = AVIF_COLOR_PRIMARIES_BT709;
    avifIm->transferCharacteristics = AVIF_TRANSFER_CHARACTERISTICS_SRGB;
    avifIm->matrixCoefficients =
        lossless ? AVIF_MATRIX_COEFFICIENTS_IDENTITY : AVIF_MATRIX_COEFFICIENTS_BT709;

    if (gdAvifApplyMetadata(avifIm, options->metadata) != GD_META_OK) {
        gd_error("avif error - Applying metadata failed\n");
        goto cleanup;
    }

    avifRGBImageSetDefaults(&rgb, avifIm);
    // this allocates memory, and sets rgb.rowBytes and rgb.pixels.
    result = avifRGBImageAllocatePixels(&rgb);
    if (isAvifError(result, "Allocating RGB pixels failed"))
        goto cleanup;

    // Parse RGB data from the GD image, and copy it into the AVIF RGB image.
    // Convert 7-bit GD alpha channel values to 8-bit AVIF values.

    p = rgb.pixels;
    for (y = 0; y < rgb.height; y++) {
        for (x = 0; x < rgb.width; x++) {
            val = im->tpixels[y][x];

            *(p++) = gdTrueColorGetRed(val);
            *(p++) = gdTrueColorGetGreen(val);
            *(p++) = gdTrueColorGetBlue(val);
            *(p++) = alpha7BitTo8Bit(gdTrueColorGetAlpha(val));
        }
    }

    // Convert the RGB image to YUV.

    result = avifImageRGBToYUV(avifIm, &rgb);
    failed = isAvifError(result, "Could not convert image to YUV");
    if (failed)
        goto cleanup;

    // Encode the image in AVIF format.

    encoder = avifEncoderCreate();
    if (encoder == NULL) {
        gd_error("avif error - Creating encoder failed\n");
        goto cleanup;
    }

    int quantizerQuality =
        lossless ? quality2Quantizer(100)
                 : (quality == QUALITY_DEFAULT ? QUANTIZER_DEFAULT : quality2Quantizer(quality));

    encoder->minQuantizer = quantizerQuality;
    encoder->maxQuantizer = quantizerQuality;
    encoder->minQuantizerAlpha = quantizerQuality;
    encoder->maxQuantizerAlpha = quantizerQuality;
    encoder->speed = speed;

    failed = !setEncoderTilesAndThreads(encoder, &rgb);
    if (failed)
        goto cleanup;

    // TODO: is there a reason to use timeSscales != 1?
    result = avifEncoderAddImage(encoder, avifIm, 1, AVIF_ADD_IMAGE_FLAG_SINGLE);
    failed = isAvifError(result, "Could not encode image");
    if (failed)
        goto cleanup;

    result = avifEncoderFinish(encoder, &avifOutput);
    failed = isAvifError(result, "Could not finish encoding");
    if (failed)
        goto cleanup;

    // Write the AVIF image bytes to the GD ctx.

    gdPutBuf(avifOutput.data, avifOutput.size, outfile);

cleanup:
    if (rgb.pixels)
        avifRGBImageFreePixels(&rgb);

    if (encoder)
        avifEncoderDestroy(encoder);

    if (avifOutput.data)
        avifRWDataFree(&avifOutput);

    if (avifIm)
        avifImageDestroy(avifIm);

    return failed;
}

BGD_DECLARE(void)
gdImageAvifEx(gdImagePtr im, FILE *outFile, int quality, int speed)
{
    gdIOCtx *out = gdNewFileCtx(outFile);
    gdAvifWriteOptions options;

    if (out == NULL)
        return;

    gdAvifWriteOptionsInit(&options);
    options.quality = quality;
    options.speed = speed;
    _gdImageAvifCtx(im, out, &options);
    out->gd_free(out);
}

BGD_DECLARE(void) gdImageAvif(gdImagePtr im, FILE *outFile)
{
    gdImageAvifEx(im, outFile, QUALITY_DEFAULT, SPEED_DEFAULT);
}

BGD_DECLARE(void *)
gdImageAvifPtrEx(gdImagePtr im, int *size, int quality, int speed)
{
    gdAvifWriteOptions options;

    gdAvifWriteOptionsInit(&options);
    options.quality = quality;
    options.speed = speed;
    return gdImageAvifPtrWithOptions(im, size, &options);
}

BGD_DECLARE(void *)
gdImageAvifPtrWithOptions(gdImagePtr im, int *size, const gdAvifWriteOptions *options)
{
    void *rv;
    gdIOCtx *out = gdNewDynamicCtx(NEW_DYNAMIC_CTX_SIZE, NULL);

    if (out == NULL) {
        return NULL;
    }

    if (_gdImageAvifCtx(im, out, options))
        rv = NULL;
    else
        rv = gdDPExtractData(out, size);

    out->gd_free(out);
    return rv;
}

BGD_DECLARE(int)
gdImageAvifCtxWithOptions(gdImagePtr im, gdIOCtx *outfile, const gdAvifWriteOptions *options)
{
    if (im == NULL || outfile == NULL) {
        return 1;
    }
    return _gdImageAvifCtx(im, outfile, options) ? 1 : 0;
}

BGD_DECLARE(int)
gdImageAvifWithOptions(gdImagePtr im, FILE *outFile, const gdAvifWriteOptions *options)
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
    status = gdImageAvifCtxWithOptions(im, out, options);
    out->gd_free(out);
    return status;
}

BGD_DECLARE(void *) gdImageAvifPtr(gdImagePtr im, int *size)
{
    return gdImageAvifPtrEx(im, size, QUALITY_DEFAULT, AVIF_SPEED_DEFAULT);
}

BGD_DECLARE(void)
gdImageAvifCtx(gdImagePtr im, gdIOCtx *outfile, int quality, int speed)
{
    gdAvifWriteOptions options;

    gdAvifWriteOptionsInit(&options);
    options.quality = quality;
    options.speed = speed;
    _gdImageAvifCtx(im, outfile, &options);
}

#else /* !HAVE_LIBAVIF */

static void *_noAvifError(void)
{
    gd_error("AVIF image support has been disabled\n");
    return NULL;
}

BGD_DECLARE(void) gdAvifWriteOptionsInit(gdAvifWriteOptions *options)
{
    if (options == NULL)
        return;
    memset(options, 0, sizeof(*options));
    options->quality = -1;
    options->speed = 6;
    options->lossless = GD_FALSE;
    options->chroma_subsampling = GD_AVIF_CHROMA_SUBSAMPLING_AUTO;
}

BGD_DECLARE(gdImagePtr) gdImageCreateFromAvif(FILE *ctx)
{
    ARG_NOT_USED(ctx);
    return _noAvifError();
}

BGD_DECLARE(gdImagePtr) gdImageCreateFromAvifPtr(int size, void *data)
{
    ARG_NOT_USED(size);
    ARG_NOT_USED(data);
    return _noAvifError();
}

BGD_DECLARE(gdImagePtr) gdImageCreateFromAvifCtx(gdIOCtx *ctx)
{
    ARG_NOT_USED(ctx);
    return _noAvifError();
}

BGD_DECLARE(void)
gdImageAvifCtx(gdImagePtr im, gdIOCtx *outfile, int quality, int speed)
{
    ARG_NOT_USED(im);
    ARG_NOT_USED(outfile);
    ARG_NOT_USED(quality);
    ARG_NOT_USED(speed);
    _noAvifError();
}

BGD_DECLARE(void)
gdImageAvifEx(gdImagePtr im, FILE *outfile, int quality, int speed)
{
    ARG_NOT_USED(im);
    ARG_NOT_USED(outfile);
    ARG_NOT_USED(quality);
    ARG_NOT_USED(speed);
    _noAvifError();
}

BGD_DECLARE(void) gdImageAvif(gdImagePtr im, FILE *outfile)
{
    ARG_NOT_USED(im);
    ARG_NOT_USED(outfile);
    _noAvifError();
}

BGD_DECLARE(void *) gdImageAvifPtr(gdImagePtr im, int *size)
{
    ARG_NOT_USED(im);
    ARG_NOT_USED(size);

    return _noAvifError();
}

BGD_DECLARE(void *)
gdImageAvifPtrEx(gdImagePtr im, int *size, int quality, int speed)
{
    ARG_NOT_USED(im);
    ARG_NOT_USED(size);
    ARG_NOT_USED(quality);
    ARG_NOT_USED(speed);
    return _noAvifError();
}

BGD_DECLARE(void *)
gdImageAvifPtrWithOptions(gdImagePtr im, int *size, const gdAvifWriteOptions *options)
{
    ARG_NOT_USED(im);
    ARG_NOT_USED(size);
    ARG_NOT_USED(options);
    return _noAvifError();
}

BGD_DECLARE(int)
gdImageAvifCtxWithOptions(gdImagePtr im, gdIOCtx *outfile, const gdAvifWriteOptions *options)
{
    ARG_NOT_USED(im);
    ARG_NOT_USED(outfile);
    ARG_NOT_USED(options);
    _noAvifError();
    return 1;
}

BGD_DECLARE(int)
gdImageAvifWithOptions(gdImagePtr im, FILE *outfile, const gdAvifWriteOptions *options)
{
    ARG_NOT_USED(im);
    ARG_NOT_USED(outfile);
    ARG_NOT_USED(options);
    _noAvifError();
    return 1;
}

#endif /* HAVE_LIBAVIF */
