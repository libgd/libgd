#include <gd.h>
#include <stdio.h>
#include <stdlib.h>

static void usage(const char *prog)
{
	fprintf(stderr, "Usage: %s input.tif output-prefix\n", prog);
	fprintf(stderr, "Writes output-prefix_000.png, output-prefix_001.png, ...\n");
}

static int write_png(gdImagePtr im, const char *prefix, int page)
{
	char filename[1024];
	FILE *out;

	if (snprintf(filename, sizeof(filename), "%s_%03d.png", prefix, page) >= (int) sizeof(filename)) {
		fprintf(stderr, "output filename is too long\n");
		return 0;
	}

	out = fopen(filename, "wb");
	if (out == NULL) {
		fprintf(stderr, "cannot create %s\n", filename);
		return 0;
	}

	gdImagePng(im, out);
	fclose(out);
	printf("wrote %s\n", filename);
	return 1;
}

int main(int argc, char **argv)
{
	FILE *in;
	gdTiffReadPtr tiff;
	gdTiffInfo info;
	gdTiffPageInfo pageInfo;
	gdImagePtr image;
	int pageCount = 0;
	int multiPage;

	if (argc != 3) {
		usage(argv[0]);
		return 1;
	}

	in = fopen(argv[1], "rb");
	if (in == NULL) {
		fprintf(stderr, "cannot open %s\n", argv[1]);
		return 1;
	}

	multiPage = gdTiffIsMultiPage(in);
	if (multiPage < 0) {
		fprintf(stderr, "%s is not a readable TIFF\n", argv[1]);
		fclose(in);
		return 1;
	}
	printf("multi-page: %s\n", multiPage ? "yes" : "no");
	fclose(in);

	in = fopen(argv[1], "rb");
	if (in == NULL) {
		fprintf(stderr, "cannot reopen %s\n", argv[1]);
		return 1;
	}
	tiff = gdTiffReadOpen(in);
	fclose(in);
	if (tiff == NULL) {
		fprintf(stderr, "cannot create TIFF reader\n");
		return 1;
	}

	if (gdTiffReadGetInfo(tiff, &info)) {
		printf("first page: %dx%d, pages: %d, bits/sample: %d, samples/pixel: %d, compression: %d, photometric: %d\n",
		       info.width, info.height, info.pageCount,
		       info.bitsPerSample, info.samplesPerPixel,
		       info.compression, info.photometric);
	}

	while (gdTiffReadNextImage(tiff, &pageInfo, &image) == 1) {
		printf("page %d: %dx%d bits/sample=%d samples/pixel=%d compression=%d photometric=%d planar=%d alpha=%d tiled=%d resolution=%.2fx%.2f unit=%d\n",
		       pageInfo.pageIndex, pageInfo.width, pageInfo.height,
		       pageInfo.bitsPerSample, pageInfo.samplesPerPixel,
		       pageInfo.compression, pageInfo.photometric, pageInfo.planar,
		       pageInfo.hasAlpha, pageInfo.isTiled,
		       pageInfo.xResolution, pageInfo.yResolution, pageInfo.resolutionUnit);

		/* image is borrowed from the reader and is valid until the next iterator call. */
		if (!write_png(image, argv[2], pageCount)) {
			gdTiffReadClose(tiff);
			return 1;
		}
		pageCount++;
	}

	gdTiffReadClose(tiff);

	printf("pages written: %d\n", pageCount);
	return 0;
}
