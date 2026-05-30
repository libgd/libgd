#include <string.h>
#include <stdlib.h>

#include "gd.h"
#include "gdtest.h"

typedef struct {
	const unsigned char *data;
	int size;
	int pos;
} SourceBuffer;

typedef struct {
	unsigned char *data;
	int size;
} SinkBuffer;

static int source_read(void *context, char *buffer, int len)
{
	SourceBuffer *src = (SourceBuffer *)context;
	int remaining = src->size - src->pos;
	int n = remaining < len ? remaining : len;

	if (n <= 0) {
		return 0;
	}
	memcpy(buffer, src->data + src->pos, n);
	src->pos += n;
	return n;
}

static int sink_write(void *context, const char *buffer, int len)
{
	SinkBuffer *sink = (SinkBuffer *)context;
	unsigned char *data = (unsigned char *)realloc(sink->data, sink->size + len);

	if (data == NULL) {
		return -1;
	}
	memcpy(data + sink->size, buffer, len);
	sink->data = data;
	sink->size += len;
	return len;
}

static gdImagePtr create_truecolor_image(void)
{
	gdImagePtr im;
	int x, y;

	im = gdImageCreateTrueColor(16, 16);
	gdTestAssert(im != NULL);
	if (im == NULL) {
		return NULL;
	}

	gdImageAlphaBlending(im, 0);
	gdImageSaveAlpha(im, 1);
	for (y = 0; y < gdImageSY(im); y++) {
		for (x = 0; x < gdImageSX(im); x++) {
			int alpha = (x + y) % (gdAlphaMax + 1);
			gdImageSetPixel(im, x, y, gdTrueColorAlpha(x * 13, y * 11, (x + y) * 7, alpha));
		}
	}
	return im;
}

static gdImagePtr create_palette_expected(void)
{
	gdImagePtr im;
	int colors[4];
	int x, y;

	im = gdImageCreateTrueColor(8, 8);
	gdTestAssert(im != NULL);
	if (im == NULL) {
		return NULL;
	}

	gdImageAlphaBlending(im, 0);
	gdImageSaveAlpha(im, 1);
	colors[0] = gdTrueColorAlpha(255, 0, 0, gdAlphaOpaque);
	colors[1] = gdTrueColorAlpha(0, 255, 0, 32);
	colors[2] = gdTrueColorAlpha(0, 0, 255, 96);
	colors[3] = gdTrueColorAlpha(255, 255, 255, gdAlphaTransparent);

	for (y = 0; y < gdImageSY(im); y++) {
		for (x = 0; x < gdImageSX(im); x++) {
			gdImageSetPixel(im, x, y, colors[(x + y) % 4]);
		}
	}
	return im;
}

static gdImagePtr create_palette_image(void)
{
	gdImagePtr im;
	int colors[4];
	int x, y;

	im = gdImageCreate(8, 8);
	gdTestAssert(im != NULL);
	if (im == NULL) {
		return NULL;
	}

	colors[0] = gdImageColorAllocateAlpha(im, 255, 0, 0, gdAlphaOpaque);
	colors[1] = gdImageColorAllocateAlpha(im, 0, 255, 0, 32);
	colors[2] = gdImageColorAllocateAlpha(im, 0, 0, 255, 96);
	colors[3] = gdImageColorAllocateAlpha(im, 255, 255, 255, gdAlphaTransparent);

	for (y = 0; y < gdImageSY(im); y++) {
		for (x = 0; x < gdImageSX(im); x++) {
			gdImageSetPixel(im, x, y, colors[(x + y) % 4]);
		}
	}
	return im;
}

int main()
{
	gdImagePtr src = NULL, dst = NULL, palette = NULL, palette_expected = NULL;
	void *data = NULL;
	int size = 0;
	char *filename = NULL;
	SourceBuffer source;
	SinkBuffer sink = {0, 0};
	gdSource gd_source;
	gdSink gd_sink;

	src = create_truecolor_image();
	if (src == NULL) {
		goto cleanup;
	}

	data = gdImageQoiPtr(src, &size);
	gdTestAssertMsg(data != NULL, "gdImageQoiPtr returned NULL\n");
	gdTestAssertMsg(size > 0, "gdImageQoiPtr returned a non-positive size\n");
	if (data == NULL) {
		goto cleanup;
	}

	dst = gdImageCreateFromQoiPtr(size, data);
	gdTestAssertMsg(dst != NULL, "gdImageCreateFromQoiPtr returned NULL\n");
	if (dst != NULL) {
		gdAssertImageEquals(src, dst);
		gdImageDestroy(dst);
		dst = NULL;
	}

	gdFree(data);
	data = gdImageQoiPtrEx(src, &size, GD_QOI_LINEAR);
	gdTestAssertMsg(data != NULL, "gdImageQoiPtrEx returned NULL\n");
	if (data != NULL) {
		dst = gdImageCreateFromQoiPtrWithMetadata(size, data, NULL);
		gdTestAssertMsg(dst != NULL, "gdImageCreateFromQoiPtrWithMetadata returned NULL\n");
		if (dst != NULL) {
			gdAssertImageEquals(src, dst);
			gdImageDestroy(dst);
			dst = NULL;
		}
		gdImageMetadataInjectQoi(&data, &size, NULL);
		gdFree(data);
		data = NULL;
	}

	filename = gdTestTempFile("qoi-im2im.qoi");
	gdTestAssert(filename != NULL);
	if (filename != NULL) {
		gdTestAssert(gdSupportsFileType(filename, 0));
		gdTestAssert(gdSupportsFileType(filename, 1));
		gdTestAssert(gdImageFile(src, filename));
		dst = gdImageCreateFromFile(filename);
		gdTestAssertMsg(dst != NULL, "gdImageCreateFromFile returned NULL\n");
		if (dst != NULL) {
			gdAssertImageEquals(src, dst);
			gdImageDestroy(dst);
			dst = NULL;
		}
	}

	data = gdImageQoiPtrWithMetadata(src, &size, NULL);
	source.data = (const unsigned char *)data;
	source.size = size;
	source.pos = 0;
	gd_source.source = source_read;
	gd_source.context = &source;
	dst = gdImageCreateFromQoiSource(&gd_source);
	gdTestAssertMsg(dst != NULL, "gdImageCreateFromQoiSource returned NULL\n");
	if (dst != NULL) {
		gdAssertImageEquals(src, dst);
		gdImageDestroy(dst);
		dst = NULL;
	}

	gd_sink.sink = sink_write;
	gd_sink.context = &sink;
	gdImageQoiToSink(src, &gd_sink);
	gdTestAssertMsg(sink.data != NULL && sink.size > 0, "gdImageQoiToSink wrote no data\n");
	if (sink.data != NULL) {
		dst = gdImageCreateFromQoiPtr(sink.size, sink.data);
		gdTestAssertMsg(dst != NULL, "sink data could not be decoded\n");
		if (dst != NULL) {
			gdAssertImageEquals(src, dst);
			gdImageDestroy(dst);
			dst = NULL;
		}
	}

	palette = create_palette_image();
	palette_expected = create_palette_expected();
	if (palette != NULL && palette_expected != NULL) {
		gdFree(data);
		data = gdImageQoiPtr(palette, &size);
		gdTestAssertMsg(data != NULL, "palette gdImageQoiPtr returned NULL\n");
		if (data != NULL) {
			dst = gdImageCreateFromQoiPtr(size, data);
			gdTestAssertMsg(dst != NULL, "palette QOI decode returned NULL\n");
			if (dst != NULL) {
				gdAssertImageEquals(palette_expected, dst);
			}
		}
	}

cleanup:
	if (src != NULL) gdImageDestroy(src);
	if (dst != NULL) gdImageDestroy(dst);
	if (palette != NULL) gdImageDestroy(palette);
	if (palette_expected != NULL) gdImageDestroy(palette_expected);
	if (data != NULL) gdFree(data);
	if (filename != NULL) gdFree(filename);
	if (sink.data != NULL) free(sink.data);

	return gdNumFailures();
}
