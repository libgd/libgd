#include <gd.h>
#include <tiffio.h>
#include <stdio.h>
#include <stdlib.h>

static void usage(const char *prog)
{
	fprintf(stderr, "Usage: %s output.tif\n", prog);
	fprintf(stderr, "Creates a three-page TIFF using the gdTiffWrite* API.\n");
}

static gdImagePtr create_page(int page, int width, int height)
{
	gdImagePtr im;
	int background;
	int accent;
	int dark;
	int light;

	im = gdImageCreateTrueColor(width, height);
	if (im == NULL) {
		return NULL;
	}

	gdImageAlphaBlending(im, 0);
	gdImageSaveAlpha(im, 1);

	background = gdTrueColor(248 - page * 30, 248 - page * 15, 240);
	accent = gdTrueColor((page == 0) ? 220 : 40,
	                     (page == 1) ? 150 : 70,
	                     (page == 2) ? 220 : 80);
	dark = gdTrueColor(25, 35, 45);
	light = gdTrueColor(255, 255, 255);

	gdImageFilledRectangle(im, 0, 0, width - 1, height - 1, background);
	gdImageFilledRectangle(im, 18 + page * 8, 18 + page * 6,
	                       width - 20, height / 2, accent);
	gdImageRectangle(im, 8, 8, width - 9, height - 9, dark);
	gdImageLine(im, 12, height - 18, width - 12, 18 + page * 10, dark);
	gdImageFilledEllipse(im, width / 2, height / 2 + 22, 46 + page * 14, 28 + page * 10, light);
	gdImageEllipse(im, width / 2, height / 2 + 22, 46 + page * 14, 28 + page * 10, dark);

	return im;
}

int main(int argc, char **argv)
{
	FILE *out;
	gdTiffWritePtr writer;
	gdTiffWriteOptions options = {0};
	int page;

	if (argc != 2) {
		usage(argv[0]);
		return 1;
	}

	out = fopen(argv[1], "w+b");
	if (out == NULL) {
		fprintf(stderr, "cannot create %s\n", argv[1]);
		return 1;
	}

	options.colorspace = GD_TIFF_RGB;
	options.bitDepth = 8;
	options.compression = COMPRESSION_ADOBE_DEFLATE;
	options.resolutionUnit = GD_TIFF_RESUNIT_INCH;
	options.xResolution = 150.0f;
	options.yResolution = 150.0f;

	writer = gdTiffWriteOpen(out, &options);
	if (writer == NULL) {
		fprintf(stderr, "cannot create TIFF writer\n");
		fclose(out);
		return 1;
	}

	for (page = 0; page < 3; page++) {
		gdImagePtr im = create_page(page, 240, 160);
		if (im == NULL) {
			fprintf(stderr, "cannot create page %d\n", page);
			gdTiffWriteClose(writer);
			fclose(out);
			return 1;
		}

		if (!gdTiffWriteAddImage(writer, im)) {
			fprintf(stderr, "cannot write page %d\n", page);
			gdImageDestroy(im);
			gdTiffWriteClose(writer);
			fclose(out);
			return 1;
		}

		gdImageDestroy(im);
		printf("added page %d\n", page);
	}

	gdTiffWriteClose(writer);
	fclose(out);

	printf("wrote %s\n", argv[1]);
	return 0;
}
