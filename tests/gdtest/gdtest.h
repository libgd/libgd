#ifndef GD_TEST_H
#define GD_TEST_H

#include <stdarg.h>

typedef struct CuTestImageResult CuTestImageResult;
struct CuTestImageResult {
	unsigned int pixels_changed;
	unsigned int max_diff;
};


/* Internal versions of assert functions -- use the public versions */
gdImagePtr gdTestImageFromPng(const char *filename);

/* Return a path to a writable temp dir.  The common test code will make sure
 * it's cleaned up when the test exits.  Feel free to write whatever in here.
 */
const char *gdTestTempDir(void);

/* Return a path to a writable file inside of the tempdir (see above).
 * You should free the pointer when you're done.
 * If |template| is NULL, you'll get a random file name, otherwise you'll get
 * that under the tempdir.
 */
char *gdTestTempFile(const char *template);

/* Return an open (writable) file handle to a temp file. */
FILE *gdTestTempFp(void);

/* Return the full path to a test file.  The path should be relative
 * to the tests/ dir.  The caller should free the pointer when finished.
 */
char *gdTestFilePathV(const char *path, va_list args);
char *gdTestFilePathX(const char *path, ...);
#define gdTestFilePath(p)       gdTestFilePathX(p, NULL)
#define gdTestFilePath2(p1, p2) gdTestFilePathX(p1, p2, NULL)

/* Return an open (read-only) file handle to a test file.
 * The path should be relative to the tests/ dir.
 */
FILE *gdTestFileOpenX(const char *path, ...);
#define gdTestFileOpen(p)       gdTestFileOpenX(p, NULL)
#define gdTestFileOpen2(p1, p2) gdTestFileOpenX(p1, p2, NULL)

void gdTestImageDiff(gdImagePtr buf_a, gdImagePtr buf_b,
                     gdImagePtr buf_diff, CuTestImageResult *result_ret);

int gdTestImageCompareToImage(const char* file, unsigned int line, const char* message,
                              gdImagePtr expected, gdImagePtr actual);

int gdTestImageCompareToFile(const char* file, unsigned int line, const char* message,
                             const char *expected_file, gdImagePtr actual);

unsigned int gdMaxPixelDiff(gdImagePtr a, gdImagePtr b);

int _gdTestAssert(const char* file, unsigned int line, int condition);

int _gdTestAssertMsg(const char* file, unsigned int line, int condition, const char* message, ...);


int _gdTestErrorMsg(const char* file, unsigned int line, const char* string, ...);

/* public assert functions */
#define gdAssertImageEqualsToFile(ex,ac) gdTestImageCompareToFile(__FILE__,__LINE__,NULL,(ex),(ac))

#define gdAssertImageEquals(ex,ac) gdTestImageCompareToImage(__FILE__,__LINE__,NULL,(ex),(ac))

#define gdTestAssert(cond) _gdTestAssert(__FILE__, __LINE__, (cond))

#define gdTestAssertMsg(cond, message, ...) _gdTestAssertMsg(__FILE__, __LINE__, (cond), (message), ## __VA_ARGS__)


#define gdTestErrorMsg(...) _gdTestErrorMsg(__FILE__, __LINE__, __VA_ARGS__)

void gdSilence(int priority, const char *format, va_list args);

int gdNumFailures(void);

#endif /* GD_TEST_H */
