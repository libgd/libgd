#include "gd.h"

#include <errno.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static void usage(const char *prog)
{
	fprintf(stderr,
		"Usage: %s <input_uhdr.jpg> <output_uhdr.jpg> [options]\n"
		"Options:\n"
		"  --quality <1..95>           JPEG quality for output (default: 90)\n"
		"  --resize <w>x<h>|<percent>%% Queue resize\n"
		"  --crop <x>,<y>,<w>,<h>      Queue crop\n"
		"  --rotate <90|180|270>       Queue clockwise rotation\n"
		"  --mirror <h|v>              Queue mirror horizontal/vertical\n"
		"  --sdr <output_sdr.jpg>      Also extract SDR base and save as JPEG\n",
		prog);
}

static int parse_int(const char *s, int *out)
{
	char *end = NULL;
	long v;

	if (!s || !out) {
		return 0;
	}

	v = strtol(s, &end, 10);
	if (end == s || *end != '\0') {
		return 0;
	}

	if (v < -2147483647L - 1L || v > 2147483647L) {
		return 0;
	}

	*out = (int) v;
	return 1;
}

static int parse_resize(const char *s, gdUhdrImagePtr uhdr, int *out_w, int *out_h)
{
	char *end = NULL;
	long w;
	long h;

	if (!s || !uhdr || !out_w || !out_h) {
		return 0;
	}

	errno = 0;
	w = strtol(s, &end, 10);
	if (errno == ERANGE) {
		return 0;
	}
	if (end != s && *end == 'x') {
		char *height_end = NULL;

		errno = 0;
		h = strtol(end + 1, &height_end, 10);
		if (errno != ERANGE && height_end != end + 1 && *height_end == '\0' &&
				w > 0 && h > 0 && w <= INT_MAX && h <= INT_MAX) {
			*out_w = (int) w;
			*out_h = (int) h;
			return 1;
		}
		return 0;
	}

	if (end != s && *end == '%' && end[1] == '\0' && w > 0) {
		long long scaled_w;
		long long scaled_h;

		if (w > INT_MAX) {
			return 0;
		}
		scaled_w = ((long long) gdUhdrImageWidth(uhdr) * w + 50) / 100;
		scaled_h = ((long long) gdUhdrImageHeight(uhdr) * w + 50) / 100;
		if (scaled_w > INT_MAX || scaled_h > INT_MAX) {
			return 0;
		}
		if (scaled_w <= 0) {
			scaled_w = 1;
		}
		if (scaled_h <= 0) {
			scaled_h = 1;
		}
		*out_w = (int) scaled_w;
		*out_h = (int) scaled_h;
		return 1;
	}

	return 0;
}

static void print_error(const char *where, const gdUhdrError *err)
{
	if (!err) {
		fprintf(stderr, "%s failed\n", where);
		return;
	}
	fprintf(stderr, "%s failed: code=%d provider=%d message=%s\n",
		where,
		err->code,
		err->provider_code,
		err->message[0] ? err->message : "(none)");
}

int main(int argc, char **argv)
{
	const char *in_path;
	const char *out_path;
	const char *sdr_path = NULL;
	int quality = 90;
	int i;
	gdUhdrError err;
	gdUhdrImagePtr uhdr = NULL;
	int rc;

	if (argc < 3) {
		usage(argv[0]);
		return 1;
	}

	in_path = argv[1];
	out_path = argv[2];
	memset(&err, 0, sizeof(err));

	if (!gdUhdrIsAvailable()) {
		fprintf(stderr, "UltraHDR support is not available in this libgd build.\n");
		return 2;
	}

	uhdr = gdUhdrImageCreateFromFile(in_path, GD_UHDR_FORMAT_JPEG, &err);
	if (!uhdr) {
		print_error("gdUhdrImageCreateFromFile", &err);
		return 1;
	}

	fprintf(stdout, "Loaded: %dx%d gain_map=%d\n",
		gdUhdrImageWidth(uhdr), gdUhdrImageHeight(uhdr), gdUhdrImageHasGainMap(uhdr));

	for (i = 3; i < argc; i++) {
		if (strcmp(argv[i], "--quality") == 0) {
			if (i + 1 >= argc || !parse_int(argv[i + 1], &quality)) {
				fprintf(stderr, "Invalid --quality value\n");
				gdUhdrImageDestroy(uhdr);
				return 1;
			}
			i++;
			continue;
		}

		if (strcmp(argv[i], "--resize") == 0) {
			int w, h;
			if (i + 1 >= argc || !parse_resize(argv[i + 1], uhdr, &w, &h)) {
				fprintf(stderr, "Invalid --resize value, expected <w>x<h> or <percent>%%\n");
				gdUhdrImageDestroy(uhdr);
				return 1;
			}
			rc = gdUhdrImageResize(uhdr, w, h, &err);
			if (rc != GD_UHDR_SUCCESS) {
				print_error("gdUhdrImageResize", &err);
				gdUhdrImageDestroy(uhdr);
				return 1;
			}
			i++;
			continue;
		}

		if (strcmp(argv[i], "--crop") == 0) {
			int x, y, w, h;
			if (i + 1 >= argc || sscanf(argv[i + 1], "%d,%d,%d,%d", &x, &y, &w, &h) != 4) {
				fprintf(stderr, "Invalid --crop value, expected <x>,<y>,<w>,<h>\n");
				gdUhdrImageDestroy(uhdr);
				return 1;
			}
			rc = gdUhdrImageCrop(uhdr, x, y, w, h, &err);
			if (rc != GD_UHDR_SUCCESS) {
				print_error("gdUhdrImageCrop", &err);
				gdUhdrImageDestroy(uhdr);
				return 1;
			}
			i++;
			continue;
		}

		if (strcmp(argv[i], "--rotate") == 0) {
			int deg;
			if (i + 1 >= argc || !parse_int(argv[i + 1], &deg)) {
				fprintf(stderr, "Invalid --rotate value\n");
				gdUhdrImageDestroy(uhdr);
				return 1;
			}
			rc = gdUhdrImageRotate(uhdr, deg, &err);
			if (rc != GD_UHDR_SUCCESS) {
				print_error("gdUhdrImageRotate", &err);
				gdUhdrImageDestroy(uhdr);
				return 1;
			}
			i++;
			continue;
		}

		if (strcmp(argv[i], "--mirror") == 0) {
			int axis;
			if (i + 1 >= argc) {
				fprintf(stderr, "Invalid --mirror value\n");
				gdUhdrImageDestroy(uhdr);
				return 1;
			}
			if (argv[i + 1][0] == 'h' || argv[i + 1][0] == 'H') {
				axis = GD_UHDR_MIRROR_HORIZONTAL;
			} else if (argv[i + 1][0] == 'v' || argv[i + 1][0] == 'V') {
				axis = GD_UHDR_MIRROR_VERTICAL;
			} else {
				fprintf(stderr, "Invalid --mirror value, expected h or v\n");
				gdUhdrImageDestroy(uhdr);
				return 1;
			}
			rc = gdUhdrImageMirror(uhdr, axis, &err);
			if (rc != GD_UHDR_SUCCESS) {
				print_error("gdUhdrImageMirror", &err);
				gdUhdrImageDestroy(uhdr);
				return 1;
			}
			i++;
			continue;
		}

		if (strcmp(argv[i], "--sdr") == 0) {
			if (i + 1 >= argc) {
				fprintf(stderr, "Invalid --sdr value\n");
				gdUhdrImageDestroy(uhdr);
				return 1;
			}
			sdr_path = argv[i + 1];
			i++;
			continue;
		}

		fprintf(stderr, "Unknown option: %s\n", argv[i]);
		usage(argv[0]);
		gdUhdrImageDestroy(uhdr);
		return 1;
	}

	if (quality < 1 || quality > 95) {
		fprintf(stderr, "quality must be in [1..95]\n");
		gdUhdrImageDestroy(uhdr);
		return 1;
	}

	rc = gdUhdrImageFile(uhdr, out_path, GD_UHDR_FORMAT_JPEG, quality, &err);
	if (rc != GD_UHDR_SUCCESS) {
		print_error("gdUhdrImageFile", &err);
		gdUhdrImageDestroy(uhdr);
		return 1;
	}

	fprintf(stdout, "Wrote UltraHDR output: %s\n", out_path);

	if (sdr_path) {
		FILE *sdr_fp;
		gdImagePtr sdr = gdUhdrImageGetSdr(uhdr, &err);
		if (!sdr) {
			print_error("gdUhdrImageGetSdr", &err);
			gdUhdrImageDestroy(uhdr);
			return 1;
		}

		sdr_fp = fopen(sdr_path, "wb");
		if (!sdr_fp) {
			fprintf(stderr, "Failed to open SDR output file: %s\n", sdr_path);
			gdImageDestroy(sdr);
			gdUhdrImageDestroy(uhdr);
			return 1;
		}

		gdImageJpeg(sdr, sdr_fp, quality);
		fclose(sdr_fp);
		gdImageDestroy(sdr);
		fprintf(stdout, "Wrote SDR output: %s\n", sdr_path);
	}

	gdUhdrImageDestroy(uhdr);
	return 0;
}
