#include <gd.h>
#include <stdio.h>
#include <string.h>

int main(int argc, char **argv)
{
	FILE *in, *out;
	gdWebpReadPtr reader;
	gdWebpWritePtr writer;
	gdWebpInfo info;
	gdWebpFrameInfo frameInfo;
	gdWebpWriteOptions options;
	gdImagePtr image;
	int result;

	if (argc != 3) {
		fprintf(stderr, "Usage: %s input.webp output.webp\n", argv[0]);
		return 1;
	}
	in = fopen(argv[1], "rb");
	if (in == NULL) {
		fprintf(stderr, "cannot open %s\n", argv[1]);
		return 1;
	}
	reader = gdWebpReadOpen(in);
	fclose(in);
	if (reader == NULL || !gdWebpReadGetInfo(reader, &info)) {
		fprintf(stderr, "cannot read animated WebP\n");
		if (reader != NULL) {
			gdWebpReadClose(reader);
		}
		return 1;
	}

	memset(&options, 0, sizeof(options));
	options.canvasWidth = info.width;
	options.canvasHeight = info.height;
	options.loopCount = info.loopCount;
	options.backgroundColor = info.backgroundColor;
	options.quality = gdWebpLossless;

	out = fopen(argv[2], "wb");
	if (out == NULL) {
		fprintf(stderr, "cannot create %s\n", argv[2]);
		gdWebpReadClose(reader);
		return 1;
	}
	writer = gdWebpWriteOpen(out, &options);
	if (writer == NULL) {
		fprintf(stderr, "cannot create WebP writer\n");
		fclose(out);
		gdWebpReadClose(reader);
		return 1;
	}

	while ((result = gdWebpReadNextImage(reader, &frameInfo, &image)) == 1) {
		if (!gdWebpWriteAddImage(writer, image, frameInfo.duration)) {
			fprintf(stderr, "cannot add frame %d\n", frameInfo.frameIndex);
			gdWebpWriteClose(writer);
			fclose(out);
			gdWebpReadClose(reader);
			return 1;
		}
	}
	gdWebpWriteClose(writer);
	fclose(out);
	gdWebpReadClose(reader);
	return result < 0 ? 1 : 0;
}
