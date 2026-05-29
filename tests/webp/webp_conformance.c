#include "gd.h"
#include "gdtest.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#if defined(_WIN32)
#include "readdir.h"
#else
#include <dirent.h>
#endif

#include <sys/stat.h>

typedef enum {
	WEBP_EXPECT_DECODE,
	WEBP_EXPECT_ROBUST
} webp_expectation;

static int has_webp_suffix(const char *name)
{
	size_t len = strlen(name);
	return len > 5 && strcmp(name + len - 5, ".webp") == 0;
}

static int is_directory(const char *path)
{
	struct stat st;
	return stat(path, &st) == 0 && S_ISDIR(st.st_mode);
}

static unsigned char *read_file(const char *path, int *size)
{
	FILE *fp;
	long len;
	unsigned char *data;

	*size = 0;
	fp = fopen(path, "rb");
	if (fp == NULL) {
		return NULL;
	}
	if (fseek(fp, 0, SEEK_END) != 0) {
		fclose(fp);
		return NULL;
	}
	len = ftell(fp);
	if (len <= 0 || fseek(fp, 0, SEEK_SET) != 0) {
		fclose(fp);
		return NULL;
	}
	data = (unsigned char *)malloc((size_t)len);
	if (data == NULL) {
		fclose(fp);
		return NULL;
	}
	if (fread(data, 1, (size_t)len, fp) != (size_t)len) {
		free(data);
		fclose(fp);
		return NULL;
	}
	fclose(fp);
	*size = (int)len;
	return data;
}

static void assert_animation_iterator(const char *path, int size, unsigned char *data)
{
	gdWebpReadPtr webp;
	gdWebpInfo info;
	gdWebpFrameInfo frameInfo;
	gdImagePtr image;
	int frames = 0;
	int result;

	webp = gdWebpReadOpenPtr(size, data);
	gdTestAssertMsg(webp != NULL, "animated WebP reader rejected valid file: %s\n", path);
	if (webp == NULL) {
		return;
	}
	gdTestAssertMsg(gdWebpReadGetInfo(webp, &info), "cannot read WebP animation info: %s\n", path);
	gdTestAssertMsg(info.width > 0 && info.height > 0, "invalid WebP canvas dimensions: %s\n", path);
	gdTestAssertMsg(info.frameCount > 0, "invalid WebP frame count: %s\n", path);
	while ((result = gdWebpReadNextImage(webp, &frameInfo, &image)) == 1) {
		gdTestAssertMsg(image != NULL, "WebP animation returned NULL frame image: %s\n", path);
		gdTestAssertMsg(gdImageSX(image) == info.width, "WebP animation frame width mismatch: %s\n", path);
		gdTestAssertMsg(gdImageSY(image) == info.height, "WebP animation frame height mismatch: %s\n", path);
		gdTestAssertMsg(frameInfo.duration >= 0, "WebP animation frame has negative duration: %s\n", path);
		frames++;
	}
	gdTestAssertMsg(result == 0, "WebP animation iterator failed before EOF: %s\n", path);
	gdTestAssertMsg(frames == info.frameCount,
	                "WebP animation yielded %d frames, expected %d: %s\n",
	                frames, info.frameCount, path);
	gdWebpReadClose(webp);
}

static void assert_valid_file(const char *path)
{
	unsigned char *data;
	int size;
	int animated;
	gdImagePtr im;

	data = read_file(path, &size);
	gdTestAssertMsg(data != NULL, "cannot read WebP corpus file: %s\n", path);
	if (data == NULL) {
		return;
	}

	animated = gdWebpIsAnimatedPtr(size, data);
	gdTestAssertMsg(animated >= 0, "valid WebP probe failed: %s\n", path);

	im = gdImageCreateFromWebpPtr(size, data);
	gdTestAssertMsg(im != NULL, "valid WebP failed to decode: %s\n", path);
	if (im != NULL) {
		gdTestAssertMsg(gdImageSX(im) > 0 && gdImageSY(im) > 0, "decoded WebP has invalid dimensions: %s\n", path);
		gdImageDestroy(im);
	}

	if (animated == 1) {
		assert_animation_iterator(path, size, data);
	}

	free(data);
}

static void assert_robust_file(const char *path)
{
	unsigned char *data;
	int size;
	gdImagePtr im;
	gdWebpReadPtr webp;

	data = read_file(path, &size);
	gdTestAssertMsg(data != NULL, "cannot read WebP robustness file: %s\n", path);
	if (data == NULL) {
		return;
	}

	(void) gdWebpIsAnimatedPtr(size, data);
	im = gdImageCreateFromWebpPtr(size, data);
	if (im != NULL) {
		gdImageDestroy(im);
	}
	webp = gdWebpReadOpenPtr(size, data);
	if (webp != NULL) {
		gdWebpReadClose(webp);
	}

	free(data);
}

static int scan_directory(const char *dir, webp_expectation expectation)
{
	DIR *handle;
	struct dirent *entry;
	int files = 0;

	handle = opendir(dir);
	if (handle == NULL) {
		gdTestErrorMsg("cannot open WebP conformance directory: %s\n", dir);
		return 0;
	}

	while ((entry = readdir(handle)) != NULL) {
		char path[4096];

		if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) {
			continue;
		}
		if (snprintf(path, sizeof(path), "%s/%s", dir, entry->d_name) >= (int)sizeof(path)) {
			gdTestErrorMsg("WebP corpus path is too long: %s/%s\n", dir, entry->d_name);
			continue;
		}
		if (is_directory(path)) {
			files += scan_directory(path, expectation);
			continue;
		}
		if (!has_webp_suffix(entry->d_name)) {
			continue;
		}
		files++;
		if (expectation == WEBP_EXPECT_DECODE) {
			assert_valid_file(path);
		} else {
			assert_robust_file(path);
		}
	}

	closedir(handle);
	return files;
}

int main(void)
{
	char *valid = gdTestFilePathX("webp", "webp-conformance", "valid", NULL);
	char *non_conformant = gdTestFilePathX("webp", "webp-conformance", "non-conformant", NULL);
	char *invalid = gdTestFilePathX("webp", "webp-conformance", "invalid", NULL);
	int valid_files;
	int non_conformant_files;
	int invalid_files;

	gdSetErrorMethod(gdSilence);
	valid_files = scan_directory(valid, WEBP_EXPECT_DECODE);
	non_conformant_files = scan_directory(non_conformant, WEBP_EXPECT_DECODE);
	invalid_files = scan_directory(invalid, WEBP_EXPECT_ROBUST);
	gdClearErrorMethod();

	gdTestAssertMsg(valid_files > 0,
	                "WebP conformance valid corpus has no .webp files in %s "
	                "(valid=%d non-conformant=%d invalid=%d)\n",
	                valid, valid_files, non_conformant_files, invalid_files);

	free(valid);
	free(non_conformant);
	free(invalid);
	return gdNumFailures();
}
