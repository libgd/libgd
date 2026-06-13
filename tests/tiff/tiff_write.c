#include "gd.h"
#include "gdtest.h"

#include "tiffio.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static gdImagePtr create_truecolor(int w, int h)
{
	gdImagePtr im;

	im = gdImageCreateTrueColor(w, h);
	if (im == NULL) {
		return NULL;
	}
	gdImageAlphaBlending(im, 0);
	gdImageSaveAlpha(im, 1);
	return im;
}

static gdImagePtr create_page(int w, int h, int r, int g, int b)
{
	gdImagePtr im;
	int color;

	im = create_truecolor(w, h);
	if (im == NULL) {
		return NULL;
	}
	color = gdTrueColorAlpha(r, g, b, gdAlphaOpaque);
	gdImageFilledRectangle(im, 0, 0, w - 1, h - 1, color);
	gdImageSetPixel(im, w - 1, h - 1, gdTrueColorAlpha(255 - r, 255 - g, 255 - b, gdAlphaOpaque));
	return im;
}

static gdImagePtr create_pattern_image(int w, int h)
{
	gdImagePtr im;
	int x, y;

	im = create_truecolor(w, h);
	if (im == NULL) {
		return NULL;
	}
	for (y = 0; y < h; y++) {
		for (x = 0; x < w; x++) {
			gdImageSetPixel(im, x, y, gdTrueColorAlpha((x * 37 + y * 19) & 255,
			                                           (x * 11 + y * 47) & 255,
			                                           (x * 67 + y * 5) & 255,
			                                           gdAlphaOpaque));
		}
	}
	return im;
}

static void *write_image_ptr(gdImagePtr im, const gdTiffWriteOptions *opts, int *size)
{
	gdTiffWritePtr writer;
	void *data;

	*size = 0;
	writer = gdTiffWriteOpenPtr(opts);
	gdTestAssert(writer != NULL);
	if (writer == NULL) {
		return NULL;
	}
	gdTestAssert(gdTiffWriteAddImage(writer, im) == 1);
	data = gdTiffWritePtrFinish(writer, size);
	gdTestAssert(data != NULL);
	gdTestAssert(*size > 0);
	return data;
}

static TIFF *open_tiff_from_data(void *data, int size, char **path)
{
	FILE *fp;
	TIFF *tif;

	*path = gdTestTempFile("tiff_write_XXXXXX");
	gdTestAssert(*path != NULL);
	if (*path == NULL) {
		return NULL;
	}
	fp = fopen(*path, "wb");
	gdTestAssert(fp != NULL);
	if (fp == NULL) {
		return NULL;
	}
	gdTestAssertMsg(fwrite(data, 1, (size_t)size, fp) == (size_t)size, "could not write temporary TIFF");
	fclose(fp);

	tif = TIFFOpen(*path, "r");
	gdTestAssert(tif != NULL);
	return tif;
}

static void close_temp_tiff(TIFF *tif, char *path)
{
	if (tif != NULL) {
		TIFFClose(tif);
	}
	if (path != NULL) {
		remove(path);
		free(path);
	}
}

static int close_enough(int a, int b, int tolerance)
{
	int d = a - b;

	if (d < 0) {
		d = -d;
	}
	return d <= tolerance;
}

static void assert_pixel_rgb(gdImagePtr im, int x, int y, int r, int g, int b)
{
	int c = gdImageGetPixel(im, x, y);

	gdTestAssertMsg(gdImageRed(im, c) == r && gdImageGreen(im, c) == g && gdImageBlue(im, c) == b,
	                "pixel (%d,%d) is %d,%d,%d, expected %d,%d,%d",
	                x, y, gdImageRed(im, c), gdImageGreen(im, c), gdImageBlue(im, c), r, g, b);
}

static void assert_roundtrip_close(gdImagePtr src, void *data, int size, int tolerance)
{
	gdImagePtr dst;
	int x, y;

	dst = gdImageCreateFromTiffPtr(size, data);
	gdTestAssert(dst != NULL);
	if (dst == NULL) {
		return;
	}
	gdTestAssertMsg(gdImageSX(dst) == gdImageSX(src) && gdImageSY(dst) == gdImageSY(src),
	                "roundtrip dimensions changed");
	for (y = 0; y < gdImageSY(src) && y < gdImageSY(dst); y++) {
		for (x = 0; x < gdImageSX(src) && x < gdImageSX(dst); x++) {
			int s = gdImageGetPixel(src, x, y);
			int d = gdImageGetPixel(dst, x, y);
			if (!gdTestAssertMsg(close_enough(gdImageRed(src, s), gdImageRed(dst, d), tolerance) &&
			                     close_enough(gdImageGreen(src, s), gdImageGreen(dst, d), tolerance) &&
			                     close_enough(gdImageBlue(src, s), gdImageBlue(dst, d), tolerance),
			                     "roundtrip pixel (%d,%d) is %d,%d,%d, expected near %d,%d,%d",
			                     x, y, gdImageRed(dst, d), gdImageGreen(dst, d), gdImageBlue(dst, d),
			                     gdImageRed(src, s), gdImageGreen(src, s), gdImageBlue(src, s))) {
				gdImageDestroy(dst);
				return;
			}
		}
	}
	gdImageDestroy(dst);
}

static void test_multipage_rgb_ptr(void)
{
	gdTiffWriteOptions opts = {0};
	gdTiffWritePtr writer;
	gdTiffReadPtr reader;
	gdTiffInfo info;
	gdTiffPageInfo page;
	gdImagePtr page0, page1, im;
	void *data;
	int size = 0;

	opts.colorspace = GD_TIFF_RGB;
	opts.compression = COMPRESSION_NONE;
	opts.xResolution = 123.0f;
	opts.yResolution = 234.0f;

	page0 = create_page(7, 5, 255, 0, 0);
	page1 = create_page(3, 4, 0, 128, 255);
	gdTestAssert(page0 != NULL);
	gdTestAssert(page1 != NULL);
	if (page0 == NULL || page1 == NULL) {
		goto cleanup_images;
	}

	writer = gdTiffWriteOpenPtr(&opts);
	gdTestAssert(writer != NULL);
	if (writer == NULL) {
		goto cleanup_images;
	}
	gdTestAssert(gdTiffWriteAddImage(writer, page0) == 1);
	gdTestAssert(gdTiffWriteAddImage(writer, page1) == 1);

	data = gdTiffWritePtrFinish(writer, &size);
	gdTestAssertMsg(data != NULL, "gdTiffWritePtrFinish returned NULL");
	gdTestAssertMsg(size > 0, "gdTiffWritePtrFinish returned empty data");
	if (data == NULL) {
		goto cleanup_images;
	}

	gdTestAssert(gdTiffIsMultiPagePtr(size, data) == 1);

	reader = gdTiffReadOpenPtr(size, data);
	gdTestAssert(reader != NULL);
	if (reader == NULL) {
		gdFree(data);
		goto cleanup_images;
	}

	gdTestAssert(gdTiffReadGetInfo(reader, &info) == 1);
	gdTestAssertMsg(info.pageCount == 2, "expected 2 pages, got %d", info.pageCount);
	gdTestAssertMsg(info.width == 7 && info.height == 5, "unexpected first page dimensions");
	gdTestAssertMsg(info.xResolution == 123.0f && info.yResolution == 234.0f,
	                "unexpected first page resolution %.2f x %.2f", info.xResolution, info.yResolution);

	gdTestAssert(gdTiffReadNextImage(reader, &page, &im) == 1);
	gdTestAssertMsg(page.pageIndex == 0 && page.width == 7 && page.height == 5,
	                "unexpected page 0 info");
	gdTestAssertMsg(page.bitsPerSample == 8 && page.samplesPerPixel == 3,
	                "unexpected page 0 sample layout");
	gdTestAssertMsg(page.compression == COMPRESSION_NONE && page.photometric == PHOTOMETRIC_RGB,
	                "unexpected page 0 tags");
	if (im != NULL) {
		assert_pixel_rgb(im, 0, 0, 255, 0, 0);
		assert_pixel_rgb(im, 6, 4, 0, 255, 255);
	}

	gdTestAssert(gdTiffReadNextImage(reader, &page, &im) == 1);
	gdTestAssertMsg(page.pageIndex == 1 && page.width == 3 && page.height == 4,
	                "unexpected page 1 info");
	if (im != NULL) {
		assert_pixel_rgb(im, 0, 0, 0, 128, 255);
		assert_pixel_rgb(im, 2, 3, 255, 127, 0);
	}
	gdTestAssert(gdTiffReadNextImage(reader, &page, &im) == 0);

	gdTiffReadClose(reader);
	gdFree(data);

cleanup_images:
	if (page0 != NULL) gdImageDestroy(page0);
	if (page1 != NULL) gdImageDestroy(page1);
}

static void test_options_and_formats(void)
{
	gdTiffWriteOptions opts = {0};
	gdTiffWritePtr writer;
	gdTiffReadPtr reader;
	gdTiffPageInfo page;
	gdImagePtr src, im;
	void *data;
	int size = 0;

	src = create_page(9, 3, 30, 200, 90);
	gdTestAssert(src != NULL);
	if (src == NULL) {
		return;
	}

	opts.colorspace = GD_TIFF_GRAY;
	opts.bitDepth = 16;
	opts.compression = COMPRESSION_ADOBE_DEFLATE;
	opts.resolutionUnit = GD_TIFF_RESUNIT_CENTIMETER;
	opts.xResolution = 40.0f;
	opts.yResolution = 50.0f;

	writer = gdTiffWriteOpenPtr(&opts);
	gdTestAssert(writer != NULL);
	if (writer != NULL) {
		gdTestAssert(gdTiffWriteAddImage(writer, src) == 1);
		data = gdTiffWritePtrFinish(writer, &size);
		gdTestAssert(data != NULL);
		if (data != NULL) {
			reader = gdTiffReadOpenPtr(size, data);
			gdTestAssert(reader != NULL);
			if (reader != NULL) {
				gdTestAssert(gdTiffReadNextImage(reader, &page, &im) == 1);
				gdTestAssertMsg(page.bitsPerSample == 16 && page.samplesPerPixel == 1,
				                "unexpected 16-bit gray layout");
				gdTestAssertMsg(page.photometric == PHOTOMETRIC_MINISBLACK,
				                "unexpected gray photometric");
				gdTestAssertMsg(page.resolutionUnit == RESUNIT_CENTIMETER,
				                "unexpected resolution unit %d", page.resolutionUnit);
				gdTiffReadClose(reader);
			}
			gdFree(data);
		}
	}

	memset(&opts, 0, sizeof(opts));
	opts.colorspace = GD_TIFF_BILEVEL;
	opts.bitDepth = 1;
	opts.compression = COMPRESSION_CCITTFAX4;
	opts.minIsWhite = 1;

	writer = gdTiffWriteOpenPtr(&opts);
	gdTestAssert(writer != NULL);
	if (writer != NULL) {
		gdTestAssert(gdTiffWriteAddImage(writer, src) == 1);
		data = gdTiffWritePtrFinish(writer, &size);
		gdTestAssert(data != NULL);
		if (data != NULL) {
			reader = gdTiffReadOpenPtr(size, data);
			gdTestAssert(reader != NULL);
			if (reader != NULL) {
				gdTestAssert(gdTiffReadNextImage(reader, &page, &im) == 1);
				gdTestAssertMsg(page.bitsPerSample == 1 && page.samplesPerPixel == 1,
				                "unexpected bilevel layout");
				gdTestAssertMsg(page.compression == COMPRESSION_CCITTFAX4,
				                "unexpected bilevel compression");
				gdTestAssertMsg(page.photometric == PHOTOMETRIC_MINISWHITE,
				                "unexpected bilevel photometric");
				gdTiffReadClose(reader);
			}
			gdFree(data);
		}
	}

	memset(&opts, 0, sizeof(opts));
	opts.colorspace = GD_TIFF_RGB;
	opts.bitDepth = 16;
	opts.compression = COMPRESSION_JPEG;
	gdSetErrorMethod(gdSilence);
	writer = gdTiffWriteOpenPtr(&opts);
	gdClearErrorMethod();
	gdTestAssertMsg(writer == NULL, "JPEG compression should reject 16-bit output");
	if (writer != NULL) {
		gdTiffWriteClose(writer);
	}

	gdImageDestroy(src);
}

static void test_exact_uncompressed_samples(void)
{
	gdTiffWriteOptions opts = {0};
	gdImagePtr src;
	void *data;
	int size = 0;
	TIFF *tif;
	char *path = NULL;
	uint8_t rgba[12];
	uint16_t rgb16[9];
	uint16_t gray16[3];
	uint8_t bilevel[1];
	uint16_t bps, spp, photometric, extra, *extra_types;

	src = create_truecolor(3, 1);
	gdTestAssert(src != NULL);
	if (src == NULL) {
		return;
	}
	gdImageSetPixel(src, 0, 0, gdTrueColorAlpha(10, 20, 30, gdAlphaOpaque));
	gdImageSetPixel(src, 1, 0, gdTrueColorAlpha(40, 50, 60, 64));
	gdImageSetPixel(src, 2, 0, gdTrueColorAlpha(70, 80, 90, gdAlphaTransparent));

	opts.colorspace = GD_TIFF_RGBA;
	opts.compression = COMPRESSION_NONE;
	data = write_image_ptr(src, &opts, &size);
	if (data != NULL) {
		tif = open_tiff_from_data(data, size, &path);
		if (tif != NULL) {
			gdTestAssert(TIFFGetField(tif, TIFFTAG_BITSPERSAMPLE, &bps) == 1);
			gdTestAssert(TIFFGetField(tif, TIFFTAG_SAMPLESPERPIXEL, &spp) == 1);
			gdTestAssert(TIFFGetField(tif, TIFFTAG_EXTRASAMPLES, &extra, &extra_types) == 1);
			gdTestAssertMsg(bps == 8 && spp == 4 && extra == 1 &&
			                extra_types[0] == EXTRASAMPLE_UNASSALPHA,
			                "unexpected RGBA sample tags");
			gdTestAssert(TIFFReadScanline(tif, rgba, 0, 0) == 1);
			gdTestAssertMsg(rgba[0] == 10 && rgba[1] == 20 && rgba[2] == 30 && rgba[3] == 255,
			                "unexpected opaque RGBA sample");
			gdTestAssertMsg(rgba[4] == 40 && rgba[5] == 50 && rgba[6] == 60 && rgba[7] == 126,
			                "unexpected half-alpha RGBA sample");
			gdTestAssertMsg(rgba[8] == 70 && rgba[9] == 80 && rgba[10] == 90 && rgba[11] == 0,
			                "unexpected transparent RGBA sample");
		}
		close_temp_tiff(tif, path);
		gdFree(data);
	}

	memset(&opts, 0, sizeof(opts));
	opts.colorspace = GD_TIFF_RGBA;
	opts.compression = COMPRESSION_NONE;
	opts.alphaType = GD_TIFF_ALPHA_ASSOCIATED;
	data = write_image_ptr(src, &opts, &size);
	if (data != NULL) {
		tif = open_tiff_from_data(data, size, &path);
		if (tif != NULL) {
			gdTestAssert(TIFFGetField(tif, TIFFTAG_EXTRASAMPLES, &extra, &extra_types) == 1);
			gdTestAssertMsg(extra == 1 && extra_types[0] == EXTRASAMPLE_ASSOCALPHA,
			                "expected associated alpha extrasample");
		}
		close_temp_tiff(tif, path);
		gdFree(data);
	}
	gdImageDestroy(src);

	src = create_truecolor(3, 1);
	gdTestAssert(src != NULL);
	if (src == NULL) {
		return;
	}
	gdImageSetPixel(src, 0, 0, gdTrueColorAlpha(1, 2, 3, gdAlphaOpaque));
	gdImageSetPixel(src, 1, 0, gdTrueColorAlpha(128, 129, 130, gdAlphaOpaque));
	gdImageSetPixel(src, 2, 0, gdTrueColorAlpha(255, 254, 253, gdAlphaOpaque));

	memset(&opts, 0, sizeof(opts));
	opts.colorspace = GD_TIFF_RGB;
	opts.bitDepth = 16;
	opts.compression = COMPRESSION_NONE;
	data = write_image_ptr(src, &opts, &size);
	if (data != NULL) {
		tif = open_tiff_from_data(data, size, &path);
		if (tif != NULL) {
			gdTestAssert(TIFFReadScanline(tif, rgb16, 0, 0) == 1);
			gdTestAssertMsg(rgb16[0] == 257 && rgb16[1] == 514 && rgb16[2] == 771,
			                "unexpected first RGB16 sample");
			gdTestAssertMsg(rgb16[3] == 32896 && rgb16[4] == 33153 && rgb16[5] == 33410,
			                "unexpected middle RGB16 sample");
			gdTestAssertMsg(rgb16[6] == 65535 && rgb16[7] == 65278 && rgb16[8] == 65021,
			                "unexpected last RGB16 sample");
		}
		close_temp_tiff(tif, path);
		gdFree(data);
	}
	gdImageDestroy(src);

	src = create_truecolor(3, 1);
	gdTestAssert(src != NULL);
	if (src == NULL) {
		return;
	}
	gdImageSetPixel(src, 0, 0, gdTrueColorAlpha(0, 0, 0, gdAlphaOpaque));
	gdImageSetPixel(src, 1, 0, gdTrueColorAlpha(128, 128, 128, gdAlphaOpaque));
	gdImageSetPixel(src, 2, 0, gdTrueColorAlpha(255, 255, 255, gdAlphaOpaque));

	memset(&opts, 0, sizeof(opts));
	opts.colorspace = GD_TIFF_GRAY;
	opts.bitDepth = 16;
	opts.compression = COMPRESSION_NONE;
	data = write_image_ptr(src, &opts, &size);
	if (data != NULL) {
		tif = open_tiff_from_data(data, size, &path);
		if (tif != NULL) {
			gdTestAssert(TIFFReadScanline(tif, gray16, 0, 0) == 1);
			gdTestAssertMsg(gray16[0] == 0 && gray16[1] == 32896 && gray16[2] == 65535,
			                "unexpected gray16 samples");
		}
		close_temp_tiff(tif, path);
		gdFree(data);
	}

	memset(&opts, 0, sizeof(opts));
	opts.colorspace = GD_TIFF_GRAY;
	opts.compression = COMPRESSION_NONE;
	opts.minIsWhite = 1;
	data = write_image_ptr(src, &opts, &size);
	if (data != NULL) {
		tif = open_tiff_from_data(data, size, &path);
		if (tif != NULL) {
			uint8_t gray8[3];
			gdTestAssert(TIFFReadScanline(tif, gray8, 0, 0) == 1);
			gdTestAssertMsg(gray8[0] == 255 && gray8[1] == 127 && gray8[2] == 0,
			                "unexpected min-is-white gray8 samples");
		}
		close_temp_tiff(tif, path);
		gdFree(data);
	}
	gdImageDestroy(src);

	src = create_truecolor(8, 1);
	gdTestAssert(src != NULL);
	if (src == NULL) {
		return;
	}
	gdImageSetPixel(src, 0, 0, gdTrueColorAlpha(0, 0, 0, gdAlphaOpaque));
	gdImageSetPixel(src, 1, 0, gdTrueColorAlpha(255, 255, 255, gdAlphaOpaque));
	gdImageSetPixel(src, 2, 0, gdTrueColorAlpha(0, 0, 0, gdAlphaOpaque));
	gdImageSetPixel(src, 3, 0, gdTrueColorAlpha(255, 255, 255, gdAlphaOpaque));
	gdImageSetPixel(src, 4, 0, gdTrueColorAlpha(255, 255, 255, gdAlphaOpaque));
	gdImageSetPixel(src, 5, 0, gdTrueColorAlpha(0, 0, 0, gdAlphaOpaque));
	gdImageSetPixel(src, 6, 0, gdTrueColorAlpha(255, 255, 255, gdAlphaOpaque));
	gdImageSetPixel(src, 7, 0, gdTrueColorAlpha(0, 0, 0, gdAlphaOpaque));

	memset(&opts, 0, sizeof(opts));
	opts.colorspace = GD_TIFF_BILEVEL;
	opts.bitDepth = 1;
	opts.compression = COMPRESSION_NONE;
	data = write_image_ptr(src, &opts, &size);
	if (data != NULL) {
		tif = open_tiff_from_data(data, size, &path);
		if (tif != NULL) {
			gdTestAssert(TIFFGetField(tif, TIFFTAG_BITSPERSAMPLE, &bps) == 1);
			gdTestAssert(TIFFGetField(tif, TIFFTAG_PHOTOMETRIC, &photometric) == 1);
			gdTestAssertMsg(bps == 1 && photometric == PHOTOMETRIC_MINISBLACK,
			                "unexpected bilevel min-is-black tags");
			gdTestAssert(TIFFReadScanline(tif, bilevel, 0, 0) == 1);
			gdTestAssertMsg(bilevel[0] == 0x5a, "unexpected bilevel min-is-black byte 0x%02x", bilevel[0]);
		}
		close_temp_tiff(tif, path);
		gdFree(data);
	}

	memset(&opts, 0, sizeof(opts));
	opts.colorspace = GD_TIFF_BILEVEL;
	opts.bitDepth = 1;
	opts.compression = COMPRESSION_NONE;
	opts.minIsWhite = 1;
	data = write_image_ptr(src, &opts, &size);
	if (data != NULL) {
		tif = open_tiff_from_data(data, size, &path);
		if (tif != NULL) {
			gdTestAssert(TIFFGetField(tif, TIFFTAG_PHOTOMETRIC, &photometric) == 1);
			gdTestAssertMsg(photometric == PHOTOMETRIC_MINISWHITE,
			                "unexpected bilevel min-is-white photometric");
			gdTestAssert(TIFFReadScanline(tif, bilevel, 0, 0) == 1);
			gdTestAssertMsg(bilevel[0] == 0xa5, "unexpected bilevel min-is-white byte 0x%02x", bilevel[0]);
		}
		close_temp_tiff(tif, path);
		gdFree(data);
	}
	gdImageDestroy(src);
}

static void test_compression_matrix(void)
{
	static const uint16_t lossless_compressions[] = {
		COMPRESSION_NONE,
		COMPRESSION_LZW,
		COMPRESSION_ADOBE_DEFLATE,
		COMPRESSION_DEFLATE,
		COMPRESSION_PACKBITS
	};
	gdImagePtr src;
	gdTiffWriteOptions opts = {0};
	void *data;
	int size = 0;
	size_t i;

	src = create_pattern_image(8, 8);
	gdTestAssert(src != NULL);
	if (src == NULL) {
		return;
	}

	opts.colorspace = GD_TIFF_RGB;
	for (i = 0; i < sizeof(lossless_compressions) / sizeof(lossless_compressions[0]); i++) {
		gdTiffReadPtr reader;
		gdTiffPageInfo page;
		gdImagePtr im;

		opts.compression = lossless_compressions[i];
		data = write_image_ptr(src, &opts, &size);
		if (data == NULL) {
			continue;
		}
		reader = gdTiffReadOpenPtr(size, data);
		gdTestAssert(reader != NULL);
		if (reader != NULL) {
			gdTestAssert(gdTiffReadNextImage(reader, &page, &im) == 1);
			gdTestAssertMsg(page.compression == lossless_compressions[i],
			                "compression tag %d did not roundtrip, got %d",
			                lossless_compressions[i], page.compression);
			if (im != NULL) {
				CuTestImageResult result = {0, 0};
				gdTestImageDiff(src, im, NULL, &result);
				gdTestAssertMsg(result.pixels_changed == 0,
				                "lossless compression %d changed %u pixels",
				                lossless_compressions[i], result.pixels_changed);
			}
			gdTiffReadClose(reader);
		}
		gdFree(data);
	}

	memset(&opts, 0, sizeof(opts));
	opts.colorspace = GD_TIFF_RGB;
	opts.compression = COMPRESSION_JPEG;
	opts.jpegQuality = 95;
	data = write_image_ptr(src, &opts, &size);
	if (data != NULL) {
		gdTiffReadPtr reader = gdTiffReadOpenPtr(size, data);
		gdTestAssert(reader != NULL);
		if (reader != NULL) {
			gdTiffPageInfo page;
			gdImagePtr im;
			gdTestAssert(gdTiffReadNextImage(reader, &page, &im) == 1);
			gdTestAssertMsg(page.compression == COMPRESSION_JPEG,
			                "unexpected JPEG compression tag %d", page.compression);
			assert_roundtrip_close(src, data, size, 80);
			gdTiffReadClose(reader);
		}
		gdFree(data);
	}

	gdImageDestroy(src);

	src = create_truecolor(8, 2);
	gdTestAssert(src != NULL);
	if (src == NULL) {
		return;
	}
	gdImageFilledRectangle(src, 0, 0, 3, 1, gdTrueColorAlpha(0, 0, 0, gdAlphaOpaque));
	gdImageFilledRectangle(src, 4, 0, 7, 1, gdTrueColorAlpha(255, 255, 255, gdAlphaOpaque));

	memset(&opts, 0, sizeof(opts));
	opts.colorspace = GD_TIFF_BILEVEL;
	opts.bitDepth = 1;
	for (i = 0; i < 2; i++) {
		gdTiffReadPtr reader;
		gdTiffPageInfo page;
		gdImagePtr im;

		opts.compression = (i == 0) ? COMPRESSION_CCITTFAX3 : COMPRESSION_CCITTFAX4;
		data = write_image_ptr(src, &opts, &size);
		if (data == NULL) {
			continue;
		}
		reader = gdTiffReadOpenPtr(size, data);
		gdTestAssert(reader != NULL);
		if (reader != NULL) {
			gdTestAssert(gdTiffReadNextImage(reader, &page, &im) == 1);
			gdTestAssertMsg(page.bitsPerSample == 1 && page.samplesPerPixel == 1,
			                "unexpected CCITT sample layout");
			gdTestAssertMsg(page.compression == opts.compression,
			                "unexpected CCITT compression tag %d", page.compression);
			gdTiffReadClose(reader);
		}
		gdFree(data);
	}
	gdImageDestroy(src);
}

static void test_writer_entry_points_and_validation(void)
{
	gdImagePtr src, palette;
	gdTiffWriteOptions opts = {0};
	gdTiffWritePtr writer;
	gdIOCtxPtr ctx;
	void *data;
	int size = 0;
	char *path;
	FILE *fp;

	src = create_page(4, 4, 12, 34, 56);
	gdTestAssert(src != NULL);
	if (src == NULL) {
		return;
	}
	opts.colorspace = GD_TIFF_RGB;
	opts.compression = COMPRESSION_NONE;

	path = gdTestTempFile("tiff_write_file_api.tif");
	gdTestAssert(path != NULL);
	if (path != NULL) {
		fp = fopen(path, "w+b");
		gdTestAssert(fp != NULL);
		if (fp != NULL) {
			writer = gdTiffWriteOpen(fp, &opts);
			gdTestAssert(writer != NULL);
			if (writer != NULL) {
				gdTestAssert(gdTiffWriteAddImage(writer, src) == 1);
				gdTestAssert(gdTiffWriteAddImage(writer, src) == 1);
				gdTiffWriteClose(writer);
			}
			fclose(fp);
			fp = fopen(path, "rb");
			gdTestAssert(fp != NULL);
			if (fp != NULL) {
				gdTiffReadPtr reader = gdTiffReadOpen(fp);
				gdTestAssert(reader != NULL);
				if (reader != NULL) {
					gdTiffInfo info;
					gdTiffPageInfo page;
					gdImagePtr dst;
					int pages = 0;

					gdTestAssert(gdTiffReadGetInfo(reader, &info) == 1);
					gdTestAssertMsg(info.pageCount == 2, "FILE writer should create 2 pages, got %d", info.pageCount);
					while (gdTiffReadNextImage(reader, &page, &dst) == 1) {
						gdTestAssertMsg(page.pageIndex == pages, "unexpected FILE writer page index");
						if (dst != NULL) {
							assert_pixel_rgb(dst, 0, 0, 12, 34, 56);
						}
						pages++;
					}
					gdTestAssertMsg(pages == 2, "FILE writer decoded %d pages", pages);
					gdTiffReadClose(reader);
				}
				fclose(fp);
			}
		}
		remove(path);
		free(path);
	}

	ctx = gdNewDynamicCtx(2048, NULL);
	gdTestAssert(ctx != NULL);
	if (ctx != NULL) {
		writer = gdTiffWriteOpenCtx(ctx, &opts);
		gdTestAssert(writer != NULL);
		if (writer != NULL) {
			gdTestAssert(gdTiffWriteAddImage(writer, src) == 1);
			gdTiffWriteClose(writer);
			data = gdDPExtractData(ctx, &size);
			gdTestAssert(data != NULL);
			gdTestAssert(size > 0);
			if (data != NULL) {
				gdImagePtr dst = gdImageCreateFromTiffPtr(size, data);
				gdTestAssert(dst != NULL);
				if (dst != NULL) {
					assert_pixel_rgb(dst, 0, 0, 12, 34, 56);
					gdImageDestroy(dst);
				}
				gdFree(data);
			}
		} else {
			ctx->gd_free(ctx);
		}
	}

	palette = gdImageCreate(2, 2);
	gdTestAssert(palette != NULL);
	if (palette != NULL) {
		int color = gdImageColorAllocate(palette, 255, 0, 0);
		gdImageFilledRectangle(palette, 0, 0, 1, 1, color);
		writer = gdTiffWriteOpenPtr(&opts);
		gdTestAssert(writer != NULL);
		if (writer != NULL) {
			gdSetErrorMethod(gdSilence);
			gdTestAssertMsg(gdTiffWriteAddImage(writer, palette) == 0,
			                "palette image should be rejected by new TIFF writer");
			gdClearErrorMethod();
			data = gdTiffWritePtrFinish(writer, &size);
			if (data != NULL) {
				gdFree(data);
			}
		}
		gdImageDestroy(palette);
	}

	gdSetErrorMethod(gdSilence);
	memset(&opts, 0, sizeof(opts));
	opts.colorspace = 999;
	gdTestAssert(gdTiffWriteOpenPtr(&opts) == NULL);
	memset(&opts, 0, sizeof(opts));
	opts.colorspace = GD_TIFF_RGB;
	opts.bitDepth = 12;
	gdTestAssert(gdTiffWriteOpenPtr(&opts) == NULL);
	memset(&opts, 0, sizeof(opts));
	opts.colorspace = GD_TIFF_RGB;
	opts.bitDepth = 1;
	gdTestAssert(gdTiffWriteOpenPtr(&opts) == NULL);
	memset(&opts, 0, sizeof(opts));
	opts.colorspace = GD_TIFF_RGB;
	opts.compression = COMPRESSION_CCITTFAX4;
	gdTestAssert(gdTiffWriteOpenPtr(&opts) == NULL);
	gdClearErrorMethod();

	gdImageDestroy(src);
}

int main(void)
{
	test_multipage_rgb_ptr();
	test_options_and_formats();
	test_exact_uncompressed_samples();
	test_compression_matrix();
	test_writer_entry_points_and_validation();
	return gdNumFailures();
}
