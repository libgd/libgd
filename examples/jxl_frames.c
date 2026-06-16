#include <gd.h>
#include <stdio.h>

static int write_png(gdImagePtr im, const char *prefix, int frame) {
	char filename[1024];
	FILE *out;

	if (snprintf(filename, sizeof(filename), "%s_%03d.png", prefix, frame) >=
		(int)sizeof(filename)) {
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
	return 1;
}

int main(int argc, char **argv) {
	FILE *in;
	gdJxlAnimReaderPtr reader;
	gdImagePtr image;
	int frame = 0;
	int delay_ms = 0;

	if (argc != 3) {
		fprintf(stderr, "Usage: %s input.jxl output-prefix\n", argv[0]);
		return 1;
	}

	in = fopen(argv[1], "rb");
	if (in == NULL) {
		fprintf(stderr, "cannot open %s\n", argv[1]);
		return 1;
	}

	reader = gdImageJxlAnimReaderCreate(in);
	fclose(in);
	if (reader == NULL) {
		fprintf(stderr, "cannot create JXL animation reader\n");
		return 1;
	}

	while ((image = gdJxlReadNextImage(reader, &delay_ms)) != NULL) {
		printf("frame %d: %dx%d duration=%dms\n", frame, gdImageSX(image),
			   gdImageSY(image), delay_ms);
		if (!write_png(image, argv[2], frame)) {
			gdImageDestroy(image);
			gdImageJxlAnimReaderDestroy(reader);
			return 1;
		}
		gdImageDestroy(image);
		frame++;
	}

	gdImageJxlAnimReaderDestroy(reader);
	printf("frames written: %d\n", frame);
	return 0;
}
