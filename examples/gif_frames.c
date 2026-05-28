#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <gd.h>

static void usage(const char *prog)
{
	fprintf(stderr, "Usage: %s input.gif output-prefix\n", prog);
	fprintf(stderr, "Writes output-prefix_000.png, output-prefix_001.png, ...\n");
}

static int write_png(gdImagePtr im, const char *prefix, int frame)
{
	char filename[1024];
	FILE *out;

	if (snprintf(filename, sizeof(filename), "%s_%03d.png", prefix, frame) >= (int) sizeof(filename)) {
		fprintf(stderr, "output filename is too long\n");
		return 0;
	}

	out = fopen(filename, "wb");
	if (!out) {
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
	gdGifReadPtr gif;
	gdGifInfo gifInfo;
	gdGifFrameInfo frameInfo;
	gdImagePtr frameImage;
	int frameCount = 0;
	int animated;

	if (argc != 3) {
		usage(argv[0]);
		return 1;
	}

	in = fopen(argv[1], "rb");
	if (!in) {
		fprintf(stderr, "cannot open %s\n", argv[1]);
		return 1;
	}

	animated = gdGifIsAnimated(in);
	if (animated < 0) {
		fprintf(stderr, "%s is not a readable GIF\n", argv[1]);
		fclose(in);
		return 1;
	}
	printf("animated: %s\n", animated ? "yes" : "no");

	gif = gdGifReadOpen(in);
	if (!gif) {
		fprintf(stderr, "cannot create GIF reader\n");
		fclose(in);
		return 1;
	}

	if (gdGifReadGetInfo(gif, &gifInfo)) {
		printf("canvas: %dx%d, background index: %d, loop count: %d\n",
		       gifInfo.width, gifInfo.height, gifInfo.backgroundIndex, gifInfo.loopCount);
	}

	while (gdGifReadNextImage(gif, &frameInfo, &frameImage) == 1) {
		printf("frame %d: rect=%d,%d %dx%d delay=%dcs disposal=%d transparent=%d local-palette=%d interlace=%d\n",
		       frameInfo.frameIndex, frameInfo.x, frameInfo.y,
		       frameInfo.width, frameInfo.height, frameInfo.delay,
		       frameInfo.disposal, frameInfo.transparentIndex,
		       frameInfo.localColorTable, frameInfo.interlace);

		/* frameImage is borrowed from the reader and is valid until the next iterator call. */
		if (!write_png(frameImage, argv[2], frameCount)) {
			gdGifReadClose(gif);
			fclose(in);
			return 1;
		}
		frameCount++;
	}

	gdGifReadClose(gif);
	fclose(in);

	printf("frames written: %d\n", frameCount);
	return 0;
}
