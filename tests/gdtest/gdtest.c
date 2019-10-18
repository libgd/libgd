#ifdef HAVE_CONFIG_H
#include <config.h>
#endif
#include <assert.h>
#include <setjmp.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <limits.h>

#ifdef HAVE_DIRENT_H
#include <dirent.h>
#endif
#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif
#ifdef HAVE_SYS_STAT_H
#include <sys/stat.h>
#endif
#ifdef HAVE_SYS_TYPES_H
#include <sys/types.h>
#endif

#ifdef _WIN32
#include "readdir.h"
#include <errno.h>
#endif

/* GDTEST_TOP_DIR is defined in other compile ways except msys
 * test_config.h is created by windows/msys/run_test.sh*/
#ifndef GDTEST_TOP_DIR
#include <test_config.h>
#endif

#include "gd.h"

#include "gdtest.h"

/* max is already defined in windows/msvc */
#ifndef max
	static inline int max(int a, int b) {return a > b ? a : b;}
#endif

void gdSilence(int priority, const char *format, va_list args)
{
	(void)priority;
	(void)format;
	(void)args;
}

gdImagePtr gdTestImageFromPng(const char *filename)
{
	gdImagePtr image;
	FILE *fp;

	/* If the path is relative, then assume it's in the tests/ dir. */
	if (filename[0] == '/' || filename[0] == '.'
#ifdef _WIN32
	|| filename[1] == ':'
#endif
	) {
		fp = fopen(filename, "rb");
	} else {
		fp = gdTestFileOpen(filename);
	}

	if (fp == NULL) {
		return NULL;
	}

	image = gdImageCreateFromPng(fp);
	fclose(fp);
	return image;
}

static char *tmpdir_base;

/* This is kind of hacky, but it's meant to be simple. */
static void _clean_dir(const char *dir)
{
	DIR *d;
	struct dirent *de;

	d = opendir(dir);
	if (d == NULL)
		return;

	if (chdir(dir) != 0)
		goto done;

	while ((de = readdir(d)) != NULL) {
		struct stat st;

		if (strcmp(de->d_name, ".") == 0 || strcmp(de->d_name, "..") == 0)
			continue;
#ifdef _WIN32
	{
		WIN32_FILE_ATTRIBUTE_DATA data;

		if (!GetFileAttributesEx(de->d_name, GetFileExInfoStandard, &data)) {
			continue;
		}
		if (data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
			_clean_dir(de->d_name);
		} else {
			unlink(de->d_name);
		}
	}
#else
		if (lstat(de->d_name, &st) != 0)
			continue;

		if (S_ISDIR(st.st_mode))
			_clean_dir(de->d_name);
		else
			unlink(de->d_name);
#endif
	}

	if (chdir("..")) {
		/* Ignore. */;
	}

 done:
	closedir(d);
	rmdir(dir);
}

static void tmpdir_cleanup(void)
{
	_clean_dir(tmpdir_base);
	free(tmpdir_base);
}

#ifdef _WIN32
char* strrstr (char* haystack, char* needle)
{
  int needle_length = strlen(needle);
  char * haystack_end = haystack + strlen(haystack) - needle_length;
  char * p;
  int i;

  for(p = haystack_end; p >= haystack; --p)
  {
    for(i = 0; i < needle_length; ++i) {
      if(p[i] != needle[i])
        goto next;
    }
    return p;

    next:;
  }
  return 0;
}


typedef VOID (WINAPI *MyGetSystemTimeAsFileTime)(LPFILETIME lpSystemTimeAsFileTime);

static MyGetSystemTimeAsFileTime get_time_func(void)
{
	MyGetSystemTimeAsFileTime timefunc = NULL;
	HMODULE hMod = GetModuleHandle("kernel32.dll");

	if (hMod) {
		/* Max possible resolution <1us, win8/server2012 */
		timefunc = (MyGetSystemTimeAsFileTime)GetProcAddress(hMod, "GetSystemTimePreciseAsFileTime");

		if(!timefunc) {
			/* 100ns blocks since 01-Jan-1641 */
			timefunc = (MyGetSystemTimeAsFileTime)GetProcAddress(hMod, "GetSystemTimeAsFileTime");
		}
	}

	return timefunc;
}
static MyGetSystemTimeAsFileTime timefunc = NULL;
static int getfilesystemtime(struct timeval *tv)
{
	FILETIME ft;
	unsigned __int64 ff = 0;
	ULARGE_INTEGER fft;

	if (timefunc == NULL) {
		timefunc = get_time_func();
	}
	timefunc(&ft);

    /*
	 * Do not cast a pointer to a FILETIME structure to either a
	 * ULARGE_INTEGER* or __int64* value because it can cause alignment faults on 64-bit Windows.
	 * via  http://technet.microsoft.com/en-us/library/ms724284(v=vs.85).aspx
	 */
	fft.HighPart = ft.dwHighDateTime;
	fft.LowPart = ft.dwLowDateTime;
	ff = fft.QuadPart;

	ff /= 10ULL; /* convert to microseconds */
	ff -= 11644473600000000ULL; /* convert to unix epoch */

	tv->tv_sec = (long)(ff / 1000000ULL);
	tv->tv_usec = (long)(ff % 1000000ULL);

	return 0;
}

static char *
mkdtemp (char *tmpl)
{
	static const char letters[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";
	static const int NLETTERS = sizeof (letters) - 1;
	static int counter = 0;
	char *XXXXXX;
	struct timeval tv;
	__int64 value;
	int count;

	/* find the last occurrence of "XXXXXX" */
	XXXXXX = strrstr(tmpl, "XXXXXX");

	if (!XXXXXX || strncmp (XXXXXX, "XXXXXX", 6)) {
		errno = EINVAL;
		return NULL;
	}

	/* Get some more or less random data.  */
	getfilesystemtime(&tv);
	value = (tv.tv_usec ^ tv.tv_sec) + counter++;

	for (count = 0; count < 100; value += 7777, ++count) {
		__int64 v = value;

		/* Fill in the random bits.  */
		XXXXXX[0] = letters[v % NLETTERS];
		v /= NLETTERS;
		XXXXXX[1] = letters[v % NLETTERS];
		v /= NLETTERS;
		XXXXXX[2] = letters[v % NLETTERS];
		v /= NLETTERS;
		XXXXXX[3] = letters[v % NLETTERS];
		v /= NLETTERS;
		XXXXXX[4] = letters[v % NLETTERS];
		v /= NLETTERS;
		XXXXXX[5] = letters[v % NLETTERS];

		/* tmpl is in UTF-8 on Windows, thus use g_mkdir() */
		if (mkdir(tmpl) == 0) {
			return tmpl;
		}
		printf("failed to create directory\n");
		if (errno != EEXIST)
			/* Any other error will apply also to other names we might
			 *  try, and there are 2^32 or so of them, so give up now.
			 */
			return NULL;
	}

	/* We got out of the loop because we ran out of combinations to try.  */
	errno = EEXIST;
	return NULL;
}
#endif

const char *gdTestTempDir(void)
{
	if (tmpdir_base == NULL) {
		char *tmpdir;
#ifdef _WIN32
		char tmpdir_root[MAX_PATH];
		size_t tmpdir_root_len = GetTempPath(MAX_PATH, tmpdir_root);
		gdTestAssert(!(tmpdir_root_len > MAX_PATH || (tmpdir_root_len == 0)));
		gdTestAssert((tmpdir_root_len + 30 < MAX_PATH));
#else
		char *tmpdir_root;
		tmpdir_root = getenv("TMPDIR");
		if (tmpdir_root == NULL)
			tmpdir_root = "/tmp";
#endif

		/* The constant here is a lazy over-estimate. */
		tmpdir = malloc(strlen(tmpdir_root) + 30);
		gdTestAssert(tmpdir != NULL);
#ifdef _WIN32
		sprintf(tmpdir, "%sgdtest.XXXXXX", tmpdir_root);
#else
		sprintf(tmpdir, "%s/gdtest.XXXXXX", tmpdir_root);
#endif
		tmpdir_base = mkdtemp(tmpdir);
		gdTestAssert(tmpdir_base != NULL);

		atexit(tmpdir_cleanup);
	}

	return tmpdir_base;
}

char *gdTestTempFile(const char *template)
{
	const char *tempdir = gdTestTempDir();
	char *ret;

#ifdef _WIN32
	{
		char *tmpfilename;
		UINT error;

		ret = malloc(MAX_PATH);
		gdTestAssert(ret != NULL);
		if (template == NULL) {
			error = GetTempFileName(tempdir,
										  "gdtest",
										  0,
										  ret);
				gdTestAssert(error != 0);
		} else {
			sprintf(ret, "%s\\%s", tempdir, template);
		}
	}
#else
	if (template == NULL) {
		template = "gdtemp.XXXXXX";
	}
	ret = malloc(strlen(tempdir) + 10 + strlen(template));
	gdTestAssert(ret != NULL);
	sprintf(ret, "%s/%s", tempdir, template);

	if (strstr(template, "XXXXXX") != NULL) {
		int fd = mkstemp(ret);
		gdTestAssert(fd != -1);
		close(fd);
	}
#endif
	return ret;
}

FILE *gdTestTempFp(void)
{
	char *file = gdTestTempFile(NULL);
	FILE *fp = fopen(file, "wb");
	gdTestAssert(fp != NULL);
	free(file);
	return fp;
}

char *gdTestFilePathV(const char *path, va_list args)
{
	size_t len;
	const char *p;
	char *file;
	va_list args_len;

	/* Figure out how much space we need. */
	va_copy(args_len, args);
	len = strlen(GDTEST_TOP_DIR) + 1;
	p = path;
	do {
		len += strlen(p) + 1;
	} while ((p = va_arg(args_len, const char *)) != NULL);
	va_end(args_len);

	/* Now build the path. */
	file = malloc(len);
	gdTestAssert(file != NULL);
	strcpy(file, GDTEST_TOP_DIR);
	p = path;
	do {
#ifdef _WIN32
		strcat(file, "\\");
#else
		strcat(file, "/");
#endif
		strcat(file, p);

	} while ((p = va_arg(args, const char *)) != NULL);
	va_end(args);

	return file;
}

char *gdTestFilePathX(const char *path, ...)
{
	va_list args;
	va_start(args, path);
	return gdTestFilePathV(path, args);
}

FILE *gdTestFileOpenX(const char *path, ...)
{
	va_list args;
	FILE *fp;
	char *file;

	va_start(args, path);
	file = gdTestFilePathV(path, args);
	fp = fopen(file, "rb");
	gdTestAssert(fp != NULL);
	free(file);
	return fp;
}

/* Compare two buffers, returning the number of pixels that are
 * different and the maximum difference of any single color channel in
 * result_ret.
 *
 * This function should be rewritten to compare all formats supported by
 * cairo_format_t instead of taking a mask as a parameter.
 */
void gdTestImageDiff(gdImagePtr buf_a, gdImagePtr buf_b,
                     gdImagePtr buf_diff, CuTestImageResult *result_ret)
{
	int x, y;
	int c1, c2;
#   define UP_DIFF(var) result_ret->max_diff = max((var), result_ret->max_diff)

	for (y = 0; y < gdImageSY(buf_a); y++) {
		for (x = 0; x < gdImageSX(buf_a); x++) {
			c1 = gdImageGetTrueColorPixel(buf_a, x, y);
			c2 = gdImageGetTrueColorPixel(buf_b, x, y);

			/* check if the pixels are the same */
			if (c1 != c2) {
				int r1,b1,g1,a1,r2,b2,g2,a2;
				unsigned int diff_a,diff_r,diff_g,diff_b;

				a1 = gdTrueColorGetAlpha(c1);
				a2 = gdTrueColorGetAlpha(c2);
				diff_a = abs (a1 - a2);
				diff_a *= 4;  /* emphasize */

				if (diff_a) {
					diff_a += 128; /* make sure it's visible */
				}
				if (diff_a > gdAlphaMax) {
					diff_a = gdAlphaMax/2;
				}

				r1 = gdTrueColorGetRed(c1);
				r2 = gdTrueColorGetRed(c2);
				diff_r = abs (r1 - r2);
				// diff_r *= 4;  /* emphasize */
				if (diff_r) {
					diff_r += gdRedMax/2; /* make sure it's visible */
				}
				if (diff_r > 255) {
					diff_r = 255;
				}
				UP_DIFF(diff_r);

				g1 = gdTrueColorGetGreen(c1);
				g2 = gdTrueColorGetGreen(c2);
				diff_g = abs (g1 - g2);

				diff_g *= 4;  /* emphasize */
				if (diff_g) {
					diff_g += gdGreenMax/2; /* make sure it's visible */
				}
				if (diff_g > 255) {
					diff_g = 255;
				}
				UP_DIFF(diff_g);

				b1 = gdTrueColorGetBlue(c1);
				b2 = gdTrueColorGetBlue(c2);
				diff_b = abs (b1 - b2);
				diff_b *= 4;  /* emphasize */
				if (diff_b) {
					diff_b += gdBlueMax/2; /* make sure it's visible */
				}
				if (diff_b > 255) {
					diff_b = 255;
				}
				UP_DIFF(diff_b);

				result_ret->pixels_changed++;
				if (buf_diff) gdImageSetPixel(buf_diff, x,y, gdTrueColorAlpha(diff_r, diff_g, diff_b, diff_a));
			} else {
				if (buf_diff) gdImageSetPixel(buf_diff, x,y, gdTrueColorAlpha(255,255,255,0));
			}
		}
	}
#   undef UP_DIFF
}


/* Return the largest difference between two corresponding pixels and
 * channels. */
unsigned int gdMaxPixelDiff(gdImagePtr a, gdImagePtr b)
{
    int diff = 0;
    int x, y;

    if (a == NULL || b == NULL || a->sx != b->sx || a->sy != b->sy)
        return UINT_MAX;

    for (x = 0; x < a->sx; x++) {
        for (y = 0; y < a->sy; y++) {
            int c1, c2;

			c1 = gdImageGetTrueColorPixel(a, x, y);
			c2 = gdImageGetTrueColorPixel(b, x, y);
            if (c1 == c2) continue;

            diff = max(diff, abs(gdTrueColorGetAlpha(c1) - gdTrueColorGetAlpha(c2)));
            diff = max(diff, abs(gdTrueColorGetRed(c1)   - gdTrueColorGetRed(c2)));
            diff = max(diff, abs(gdTrueColorGetGreen(c1) - gdTrueColorGetGreen(c2)));
            diff = max(diff, abs(gdTrueColorGetBlue(c1)  - gdTrueColorGetBlue(c2)));
        }/* for */
    }/* for */

    return diff;
}

int gdTestImageCompareToImage(const char* file, unsigned int line, const char* message,
                              gdImagePtr expected, gdImagePtr actual)
{
	unsigned int width_a, height_a;
	unsigned int width_b, height_b;
	gdImagePtr surface_diff = NULL;
	CuTestImageResult result = {0, 0};

	(void)message;

	if (!actual) {
		_gdTestErrorMsg(file, line, "Image is NULL\n");
		goto fail;
	}

	width_a  = gdImageSX(expected);
	height_a = gdImageSY(expected);
	width_b  = gdImageSX(actual);
	height_b = gdImageSY(actual);

	if (width_a  != width_b  || height_a != height_b) {
		_gdTestErrorMsg(file, line,
				"Image size mismatch: (%ux%u) vs. (%ux%u)\n       for %s vs. buffer\n",
				width_a, height_a,
				width_b, height_b,
				file);
		goto fail;
	}

	surface_diff = gdImageCreateTrueColor(width_a, height_a);

	gdTestImageDiff(expected, actual, surface_diff, &result);
	if (result.pixels_changed>0) {
		char file_diff[255];
		char file_out[1024];
		FILE *fp;
		int len, p;

		_gdTestErrorMsg(file, line,
				"Total pixels changed: %d with a maximum channel difference of %d.\n",
				result.pixels_changed,
				result.max_diff
			);

		p = len = strlen(file);
		p--;

		/* Use only the filename (and store it in the bld dir not the src dir
		 */
		while(p > 0 && (file[p] != '/' && file[p] != '\\')) {
			p--;
		}
		sprintf(file_diff, "%s_%u_diff.png", file + p  + 1, line);
		sprintf(file_out, "%s_%u_out.png", file + p  + 1, line);

		fp = fopen(file_diff, "wb");
		if (!fp) goto fail;
		gdImagePng(surface_diff,fp);
		fclose(fp);
		gdImageDestroy(surface_diff);

		fp = fopen(file_out, "wb");
		if (!fp) goto fail;
		gdImagePng(actual, fp);
		fclose(fp);
		return 0;
	} else {
		if (surface_diff) {
			gdImageDestroy(surface_diff);
		}
		return 1;
	}

fail:
	if (surface_diff) {
		gdImageDestroy(surface_diff);
	}
	return 1;
}

int gdTestImageCompareToFile(const char* file, unsigned int line, const char* message,
                             const char *expected_file, gdImagePtr actual)
{
	gdImagePtr expected = 0;
	int res = 1;

	expected = gdTestImageFromPng(expected_file);

	if (!expected) {
		_gdTestErrorMsg(file, line, "Cannot open PNG <%s>\n", expected_file);
		res = 0;
	} else {
		res = gdTestImageCompareToImage(file, line, message, expected, actual);
		gdImageDestroy(expected);
	}
	return res;
}

static int failureCount = 0;

int gdNumFailures() {
    return failureCount;
}

int _gdTestAssert(const char* file, unsigned int line, int condition)
{
	if (condition) return 1;
	_gdTestErrorMsg(file, line, "Assert failed in <%s:%i>\n", file, line);

    ++failureCount;

	return 0;
}

int _gdTestAssertMsg(const char* file, unsigned int line, int condition, const char* message, ...)
{
	va_list args;

	if (condition) return 1;

	fprintf(stderr, "%s:%u: ", file, line);
	va_start(args, message);
	vfprintf(stderr, message, args);
	va_end(args);

	fflush(stderr);

	++failureCount;

	return 0;
}

int _gdTestErrorMsg(const char* file, unsigned int line, const char* format, ...) /* {{{ */
{
	va_list args;

	fprintf(stderr, "%s:%u: ", file, line);
	va_start(args, format);
	vfprintf(stderr, format, args);
	va_end(args);
	fflush(stderr);

    ++failureCount;

	return 0;
}
/* }}} */
