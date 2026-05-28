#include <gd.h>
#include "gdtest.h"
#include <stdlib.h>
#include <string.h>

typedef struct {
	int frame_count;
	int total_delay;
	int one_by_one_frames;
} GifInfo;

static int read_word(const unsigned char *data, int size, int pos, int *value)
{
	if (pos < 0 || pos + 1 >= size) {
		return 0;
	}
	*value = data[pos] | (data[pos + 1] << 8);
	return 1;
}

static int skip_sub_blocks(const unsigned char *data, int size, int *pos)
{
	int block_size;

	while (*pos < size) {
		block_size = data[*pos];
		(*pos)++;
		if (block_size == 0) {
			return 1;
		}
		if (*pos + block_size > size) {
			return 0;
		}
		*pos += block_size;
	}

	return 0;
}

static int parse_gif_info(const unsigned char *data, int size, GifInfo *info)
{
	int pos, packed, global_color_entries;
	int pending_delay = 0;

	if (size < 13 || data[0] != 'G' || data[1] != 'I' || data[2] != 'F') {
		return 0;
	}

	info->frame_count = 0;
	info->total_delay = 0;
	info->one_by_one_frames = 0;

	packed = data[10];
	global_color_entries = (packed & 0x80) ? (2 << (packed & 0x07)) : 0;
	pos = 13 + 3 * global_color_entries;

	while (pos < size) {
		if (data[pos] == ';') {
			return 1;
		}

		if (data[pos] == '!') {
			int label;

			pos++;
			if (pos >= size) {
				return 0;
			}
			label = data[pos++];
			if (label == 0xf9) {
				int delay;

				if (pos + 5 >= size || data[pos] != 4) {
					return 0;
				}
				if (!read_word(data, size, pos + 2, &delay)) {
					return 0;
				}
				pending_delay = delay;
				pos += 6;
			} else {
				if (!skip_sub_blocks(data, size, &pos)) {
					return 0;
				}
			}
			continue;
		}

		if (data[pos] == ',') {
			int width, height, local_color_entries;

			if (pos + 9 >= size) {
				return 0;
			}
			if (!read_word(data, size, pos + 5, &width) ||
			        !read_word(data, size, pos + 7, &height)) {
				return 0;
			}
			packed = data[pos + 9];
			local_color_entries = (packed & 0x80) ? (2 << (packed & 0x07)) : 0;
			pos += 10 + 3 * local_color_entries;
			if (pos >= size) {
				return 0;
			}

			pos++; /* LZW minimum code size */
			if (!skip_sub_blocks(data, size, &pos)) {
				return 0;
			}

			info->frame_count++;
			info->total_delay += pending_delay;
			if (width == 1 && height == 1) {
				info->one_by_one_frames++;
			}
			pending_delay = 0;
			continue;
		}

		return 0;
	}

	return 0;
}

static gdImagePtr create_frame(int r, int g, int b)
{
	gdImagePtr im;

	im = gdImageCreate(100, 100);
	if (im == NULL) {
		return NULL;
	}
	gdImageColorAllocate(im, r, g, b);
	return im;
}

int main()
{
	gdImagePtr redFrame = NULL, greenFrame1 = NULL, greenFrame2 = NULL, blueFrame = NULL;
	void *begin = NULL, *red = NULL, *green1 = NULL, *green2 = NULL, *blue = NULL, *end = NULL;
	int begin_size = 0, red_size = 0, green1_size = 0, green2_size = 0, blue_size = 0, end_size = 0;
	int total_size;
	unsigned char *gif;
	GifInfo info;

	redFrame = create_frame(255, 0, 0);
	greenFrame1 = create_frame(0, 255, 0);
	greenFrame2 = create_frame(0, 255, 0);
	blueFrame = create_frame(0, 0, 255);
	if (!gdTestAssert(redFrame != NULL && greenFrame1 != NULL && greenFrame2 != NULL && blueFrame != NULL)) {
		goto done;
	}

	begin = gdImageGifAnimBeginPtr(redFrame, &begin_size, 0, 0);
	red = gdImageGifAnimAddPtr(redFrame, &red_size, 1, 0, 0, 100, gdDisposalNone, NULL);
	green1 = gdImageGifAnimAddPtr(greenFrame1, &green1_size, 1, 0, 0, 100, gdDisposalNone, redFrame);
	green2 = gdImageGifAnimAddPtr(greenFrame2, &green2_size, 1, 0, 0, 100, gdDisposalNone, greenFrame1);
	blue = gdImageGifAnimAddPtr(blueFrame, &blue_size, 1, 0, 0, 100, gdDisposalNone, greenFrame2);
	end = gdImageGifAnimEndPtr(&end_size);

	gdTestAssertMsg(begin != NULL, "expected GIF animation header data");
	gdTestAssertMsg(red != NULL, "expected red frame data");
	gdTestAssertMsg(green1 != NULL, "expected first green frame data");
	gdTestAssertMsg(green2 != NULL, "expected duplicate green placeholder frame data");
	gdTestAssertMsg(blue != NULL, "expected blue frame data");
	gdTestAssertMsg(end != NULL, "expected GIF animation terminator data");
	if (begin == NULL || red == NULL || green1 == NULL || green2 == NULL || blue == NULL || end == NULL) {
		goto done;
	}

	total_size = begin_size + red_size + green1_size + green2_size + blue_size + end_size;
	gif = malloc(total_size);
	if (!gdTestAssert(gif != NULL)) {
		goto done;
	}

	memcpy(gif, begin, begin_size);
	memcpy(gif + begin_size, red, red_size);
	memcpy(gif + begin_size + red_size, green1, green1_size);
	memcpy(gif + begin_size + red_size + green1_size, green2, green2_size);
	memcpy(gif + begin_size + red_size + green1_size + green2_size, blue, blue_size);
	memcpy(gif + begin_size + red_size + green1_size + green2_size + blue_size, end, end_size);

	gdTestAssertMsg(parse_gif_info(gif, total_size, &info), "expected parseable GIF animation");
	gdTestAssertMsg(info.frame_count == 4, "expected 4 image frames, got %d", info.frame_count);
	gdTestAssertMsg(info.total_delay == 400, "expected total delay 400, got %d", info.total_delay);
	gdTestAssertMsg(info.one_by_one_frames == 1,
	                "expected one 1x1 transparent placeholder frame, got %d", info.one_by_one_frames);

	free(gif);

done:
	if (begin != NULL) gdFree(begin);
	if (red != NULL) gdFree(red);
	if (green1 != NULL) gdFree(green1);
	if (green2 != NULL) gdFree(green2);
	if (blue != NULL) gdFree(blue);
	if (end != NULL) gdFree(end);
	if (redFrame != NULL) gdImageDestroy(redFrame);
	if (greenFrame1 != NULL) gdImageDestroy(greenFrame1);
	if (greenFrame2 != NULL) gdImageDestroy(greenFrame2);
	if (blueFrame != NULL) gdImageDestroy(blueFrame);

	return gdNumFailures();
}
