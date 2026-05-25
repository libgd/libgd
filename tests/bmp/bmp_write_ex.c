#include "gd.h"
#include "gdtest.h"

#define OFFSET_BF_SIZE 2
#define OFFSET_BF_OFF_BITS 10
#define OFFSET_BI_SIZE 14
#define OFFSET_BI_BIT_COUNT 28
#define OFFSET_BI_COMPRESSION 30
#define OFFSET_BI_SIZE_IMAGE 34

static unsigned int get_field(unsigned char *data, int offset)
{
	return data[offset + 3] << 24 | data[offset + 2] << 16 | data[offset + 1] << 8 | data[offset];
}

static unsigned int get_word(unsigned char *data, int offset)
{
	return data[offset + 1] << 8 | data[offset];
}

static gdImagePtr create_palette_image(void)
{
	gdImagePtr im;
	int black, white, red, green, blue;

	im = gdImageCreate(17, 13);
	if (im == NULL) {
		return NULL;
	}
	black = gdImageColorAllocate(im, 0, 0, 0);
	white = gdImageColorAllocate(im, 255, 255, 255);
	red = gdImageColorAllocate(im, 255, 0, 0);
	green = gdImageColorAllocate(im, 0, 255, 0);
	blue = gdImageColorAllocate(im, 0, 0, 255);
	gdImageFilledRectangle(im, 0, 0, 16, 12, black);
	gdImageLine(im, 0, 0, 16, 12, white);
	gdImageLine(im, 0, 12, 16, 0, red);
	gdImageRectangle(im, 2, 2, 14, 10, green);
	gdImageFilledRectangle(im, 6, 4, 10, 8, blue);
	return im;
}

static gdImagePtr create_mono_image(void)
{
	gdImagePtr im;
	int black, white;

	im = gdImageCreate(17, 13);
	if (im == NULL) {
		return NULL;
	}
	black = gdImageColorAllocate(im, 0, 0, 0);
	white = gdImageColorAllocate(im, 255, 255, 255);
	gdImageFilledRectangle(im, 0, 0, 16, 12, black);
	gdImageLine(im, 0, 0, 16, 12, white);
	return im;
}

static gdImagePtr create_truecolor_image(int alpha)
{
	gdImagePtr im;
	int c;

	im = gdImageCreateTrueColor(17, 13);
	if (im == NULL) {
		return NULL;
	}
	gdImageAlphaBlending(im, 0);
	gdImageSaveAlpha(im, 1);
	gdImageFilledRectangle(im, 0, 0, 16, 12, gdTrueColorAlpha(20, 40, 60, gdAlphaOpaque));
	c = alpha ? gdTrueColorAlpha(255, 0, 0, gdAlphaTransparent / 2) : gdTrueColor(255, 0, 0);
	gdImageFilledRectangle(im, 4, 3, 12, 9, c);
	return im;
}

static void assert_bmp_fields(unsigned char *data, int size, int bpp, int compression, int header_size)
{
	gdTestAssert(data != NULL);
	if (data == NULL) {
		return;
	}
	gdTestAssert(data[0] == 'B' && data[1] == 'M');
	gdTestAssertMsg(get_field(data, OFFSET_BF_SIZE) == (unsigned int) size,
		"bfSize mismatch for %dbpp", bpp);
	gdTestAssertMsg(get_field(data, OFFSET_BI_SIZE) == (unsigned int) header_size,
		"header size mismatch for %dbpp", bpp);
	gdTestAssertMsg(get_word(data, OFFSET_BI_BIT_COUNT) == (unsigned int) bpp,
		"bit count mismatch for %dbpp", bpp);
	gdTestAssertMsg(get_field(data, OFFSET_BI_COMPRESSION) == (unsigned int) compression,
		"compression mismatch for %dbpp", bpp);
	gdTestAssertMsg(get_field(data, OFFSET_BF_OFF_BITS) < (unsigned int) size,
		"pixel offset outside file for %dbpp", bpp);
	gdTestAssertMsg(get_field(data, OFFSET_BI_SIZE_IMAGE) <= (unsigned int) size,
		"image size outside file for %dbpp", bpp);
}

static void assert_roundtrip(gdImagePtr im, int bpp, int compression, int flags, int expected_compression, int header_size)
{
	void *data;
	int size;
	gdImagePtr actual;

	data = gdImageBmpPtrEx(im, &size, bpp, compression, flags);
	assert_bmp_fields((unsigned char *) data, size, bpp ? bpp : get_word((unsigned char *) data, OFFSET_BI_BIT_COUNT),
		expected_compression, header_size);
	actual = gdImageCreateFromBmpPtr(size, data);
	gdTestAssertMsg(actual != NULL, "failed to read generated BMP");
	if (actual != NULL) {
		gdImageDestroy(actual);
	}
	gdFree(data);
}

int main()
{
	gdImagePtr mono, pal, tc, tca;
	void *data;
	int size;

	mono = create_mono_image();
	pal = create_palette_image();
	tc = create_truecolor_image(0);
	tca = create_truecolor_image(1);
	gdTestAssert(mono != NULL);
	gdTestAssert(pal != NULL);
	gdTestAssert(tc != NULL);
	gdTestAssert(tca != NULL);

	assert_roundtrip(mono, 1, GD_BMP_COMPRESS_NONE, GD_BMP_FLAG_NONE, 0, 40);
	assert_roundtrip(pal, 4, GD_BMP_COMPRESS_NONE, GD_BMP_FLAG_NONE, 0, 40);
	assert_roundtrip(pal, 4, GD_BMP_COMPRESS_RLE4, GD_BMP_FLAG_NONE, 2, 40);
	assert_roundtrip(pal, 8, GD_BMP_COMPRESS_NONE, GD_BMP_FLAG_NONE, 0, 40);
	assert_roundtrip(pal, 8, GD_BMP_COMPRESS_RLE8, GD_BMP_FLAG_NONE, 1, 40);
	assert_roundtrip(tc, 16, GD_BMP_COMPRESS_NONE, GD_BMP_FLAG_NONE, 3, 40);
	assert_roundtrip(tc, 24, GD_BMP_COMPRESS_NONE, GD_BMP_FLAG_NONE, 0, 40);
	assert_roundtrip(tca, 32, GD_BMP_COMPRESS_NONE, GD_BMP_FLAG_NONE, 3, 108);

	data = gdImageBmpPtrEx(tc, &size, 8, GD_BMP_COMPRESS_NONE, GD_BMP_FLAG_NONE);
	gdTestAssertMsg(data == NULL, "truecolor to indexed without quantize flag should fail");

	data = gdImageBmpPtrEx(tc, &size, 8, GD_BMP_COMPRESS_NONE, GD_BMP_FLAG_QUANTIZE);
	assert_bmp_fields((unsigned char *) data, size, 8, 0, 40);
	gdTestAssertMsg(tc->trueColor, "quantized write mutated source image");
	gdFree(data);

	data = gdImageBmpPtrEx(tc, &size, 0, GD_BMP_COMPRESS_NONE, GD_BMP_FLAG_NONE);
	assert_bmp_fields((unsigned char *) data, size, 24, 0, 40);
	gdFree(data);

	data = gdImageBmpPtrEx(tca, &size, 0, GD_BMP_COMPRESS_NONE, GD_BMP_FLAG_NONE);
	assert_bmp_fields((unsigned char *) data, size, 32, 3, 108);
	gdFree(data);

	gdImageDestroy(mono);
	gdImageDestroy(pal);
	gdImageDestroy(tc);
	gdImageDestroy(tca);
	return gdNumFailures();
}
