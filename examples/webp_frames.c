#include <gd.h>
#include <stdio.h>
#include <stdlib.h>

static int write_png(gdImagePtr im, const char *prefix, int frame)
{
	char filename[1024];
	FILE *out;

	if (snprintf(filename, sizeof(filename), "%s_%03d.png", prefix, frame) >= (int) sizeof(filename)) {
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

int main(int argc, char **argv)
{
	FILE *in;
	gdWebpReadPtr webp;
	gdWebpInfo info;
	gdWebpFrameInfo frameInfo;
	gdImagePtr image;
	int frameCount = 0;

	if (argc != 3) {
		fprintf(stderr, "Usage: %s input.webp output-prefix\n", argv[0]);
		return 1;
	}
	in = fopen(argv[1], "rb");
	if (in == NULL) {
		fprintf(stderr, "cannot open %s\n", argv[1]);
		return 1;
	}
	printf("animated: %s\n", gdWebpIsAnimated(in) == 1 ? "yes" : "no");
	webp = gdWebpReadOpen(in);
	fclose(in);
	if (webp == NULL) {
		fprintf(stderr, "cannot create WebP reader\n");
		return 1;
	}
	if (gdWebpReadGetInfo(webp, &info)) {
		printf("canvas: %dx%d, frames: %d, loop count: %d\n",
		       info.width, info.height, info.frameCount, info.loopCount);
	}
	while (gdWebpReadNextImage(webp, &frameInfo, &image) == 1) {
		printf("frame %d: rect=%d,%d %dx%d duration=%dms dispose=%d blend=%d alpha=%d\n",
		       frameInfo.frameIndex, frameInfo.x, frameInfo.y,
		       frameInfo.width, frameInfo.height, frameInfo.duration,
		       frameInfo.dispose, frameInfo.blend, frameInfo.hasAlpha);
		if (!write_png(image, argv[2], frameCount++)) {
			gdWebpReadClose(webp);
			return 1;
		}
	}
	gdWebpReadClose(webp);
	printf("frames written: %d\n", frameCount);
	return 0;
}
