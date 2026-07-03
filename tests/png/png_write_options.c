#include "gd.h"
#include "gdtest.h"

#include <string.h>

static gdImagePtr create_image(void) {
	gdImagePtr im = gdImageCreateTrueColor(16, 16);
	int x, y;
	if (im == NULL)
		return NULL;
	for (y = 0; y < 16; y++) {
		for (x = 0; x < 16; x++) {
			gdImageSetPixel(im, x, y,
							gdTrueColorAlpha(x * 16, y * 16, (x + y) * 8,
											 (x + y) % 128));
		}
	}
	gdImageSaveAlpha(im, 1);
	return im;
}

static int check_ptr(gdImagePtr im, const gdPngWriteOptions *options) {
	gdImagePtr decoded;
	void *data;
	int size = 0;
	data = gdImagePngPtrWithOptions(im, &size, options);
	if (data == NULL || size <= 0)
		return 0;
	decoded = gdImageCreateFromPngPtr(size, data);
	gdFree(data);
	if (decoded == NULL)
		return 0;
	gdImageDestroy(decoded);
	return 1;
}

int main(void) {
	static const unsigned int filters[] = {
		GD_PNG_FILTER_NONE, GD_PNG_FILTER_SUB, GD_PNG_FILTER_UP,
		GD_PNG_FILTER_AVERAGE, GD_PNG_FILTER_PAETH, GD_PNG_FILTER_ALL};
	gdPngWriteOptions options;
	gdImagePtr im;
	void *legacy;
	void *advanced;
	gdIOCtx *ctx;
	FILE *file;
	int legacy_size = 0;
	int advanced_size = 0;
	int i;

	im = create_image();
	gdTestAssert(im != NULL);

	gdPngWriteOptionsInit(&options);
	gdTestAssert(options.struct_size == sizeof(options));
	gdTestAssert(options.compression_level == -1);
	gdTestAssert(options.filters == GD_PNG_FILTER_AUTO);
	gdTestAssert(options.compression_strategy ==
				 GD_PNG_COMPRESSION_STRATEGY_DEFAULT);

	legacy = gdImagePngPtr(im, &legacy_size);
	advanced = gdImagePngPtrWithOptions(im, &advanced_size, &options);
	gdTestAssert(legacy != NULL && advanced != NULL);
	gdTestAssert(legacy_size == advanced_size);
	gdTestAssert(memcmp(legacy, advanced, legacy_size) == 0);
	gdFree(legacy);
	gdFree(advanced);

	for (i = 0; i < (int)(sizeof(filters) / sizeof(filters[0])); i++) {
		gdPngWriteOptionsInit(&options);
		options.filters = filters[i];
		gdTestAssert(check_ptr(im, &options));
	}

	for (i = GD_PNG_COMPRESSION_STRATEGY_DEFAULT;
		 i <= GD_PNG_COMPRESSION_STRATEGY_FIXED; i++) {
		gdPngWriteOptionsInit(&options);
		options.compression_strategy = i;
		gdTestAssert(check_ptr(im, &options));
	}

	ctx = gdNewDynamicCtx(2048, NULL);
	gdTestAssert(ctx != NULL);
	gdTestAssert(gdImagePngCtxWithOptions(im, ctx, NULL) == 0);
	ctx->gd_free(ctx);

	file = tmpfile();
	gdTestAssert(file != NULL);
	gdTestAssert(gdImagePngWithOptions(im, file, NULL) == 0);
	fclose(file);

	gdPngWriteOptionsInit(&options);
	options.filters = GD_PNG_FILTER_ALL | (1U << 31);
	advanced_size = 123;
	gdTestAssert(gdImagePngPtrWithOptions(im, &advanced_size, &options) == NULL);
	gdTestAssert(advanced_size == 0);

	gdPngWriteOptionsInit(&options);
	options.compression_level = 10;
	gdTestAssert(!check_ptr(im, &options));

	gdImageDestroy(im);
	return gdNumFailures();
}
