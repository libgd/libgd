#include <stdio.h>
#include <stdlib.h>
#include <gd.h>

static void usage(const char *prog)
{
	fprintf(stderr, "Usage: %s input.gif output.gif\n", prog);
	fprintf(stderr, "Re-encodes raw GIF frames with their offsets, delays, disposal, and transparency.\n");
}

static gdImagePtr create_screen_image(const gdGifInfo *info)
{
	gdImagePtr im;

	im = gdImageCreate(info->width, info->height);
	if (!im) {
		return NULL;
	}

	/* The animation writer uses this image for logical screen size.
	 * Frames are written with local color tables below, so this palette is minimal.
	 */
	gdImageColorAllocate(im, 0, 0, 0);
	gdImageColorAllocate(im, 255, 255, 255);
	return im;
}

int main(int argc, char **argv)
{
	FILE *in;
	FILE *out;
	gdGifReadPtr gif;
	gdGifInfo gifInfo;
	gdGifFrameInfo frameInfo;
	gdImagePtr screen;
	gdImagePtr frame;
	int frameCount = 0;
	int result;

	if (argc != 3) {
		usage(argv[0]);
		return 1;
	}

	in = fopen(argv[1], "rb");
	if (!in) {
		fprintf(stderr, "cannot open %s\n", argv[1]);
		return 1;
	}

	gif = gdGifReadOpen(in);
	if (!gif) {
		fprintf(stderr, "cannot create GIF reader\n");
		fclose(in);
		return 1;
	}

	if (!gdGifReadGetInfo(gif, &gifInfo)) {
		fprintf(stderr, "cannot read GIF metadata\n");
		gdGifReadClose(gif);
		fclose(in);
		return 1;
	}

	screen = create_screen_image(&gifInfo);
	if (!screen) {
		fprintf(stderr, "cannot create logical screen image\n");
		gdGifReadClose(gif);
		fclose(in);
		return 1;
	}

	out = fopen(argv[2], "wb");
	if (!out) {
		fprintf(stderr, "cannot create %s\n", argv[2]);
		gdImageDestroy(screen);
		gdGifReadClose(gif);
		fclose(in);
		return 1;
	}

	/* Use no global color table and write each decoded frame with a local table.
	 * This produces a valid re-encoded GIF, but it is not a byte-for-byte copy:
	 * comments, application extensions other than the loop count, and exact LZW
	 * coding are not preserved.
	 */
	gdImageGifAnimBegin(screen, out, 0, gifInfo.loopCount);

	while ((result = gdGifReadNextFrame(gif, &frameInfo, &frame)) == 1) {
		gdImageGifAnimAdd(frame, out, 1, frameInfo.x, frameInfo.y,
		                  frameInfo.delay, frameInfo.disposal, NULL);
		printf("frame %d: rect=%d,%d %dx%d delay=%dcs disposal=%d transparent=%d local-palette=%d\n",
		       frameInfo.frameIndex, frameInfo.x, frameInfo.y,
		       frameInfo.width, frameInfo.height, frameInfo.delay,
		       frameInfo.disposal, frameInfo.transparentIndex,
		       frameInfo.localColorTable);
		frameCount++;
	}

	gdImageGifAnimEnd(out);
	fclose(out);
	gdImageDestroy(screen);
	gdGifReadClose(gif);
	fclose(in);

	if (result < 0) {
		fprintf(stderr, "GIF decode failed while rewriting\n");
		return 1;
	}

	printf("frames rewritten: %d\n", frameCount);
	return 0;
}
