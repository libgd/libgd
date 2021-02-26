/**
 * File: compare_avif_to_png
 *
 * Thorough check for AVIF encoding and decoding.
 * This test reqiures a set of PNG images that have been losslessly encoded to AVIFs.
 * For each such image, we encode the PNG into an AVIF, with the GD format as an intermediary,
 * then compare the resulting AVIF with the original PNG.
 *
 * We then do this process in reverse, encoding the AVIF into a PNG,
 * and compare the resulting PNG with the original AVIF.
 *
 * We report any discrepancies in the images, or any other errors that may occur.
 */

#include <stdio.h>
#include <string.h>
#include "gd.h"
#include "gdtest.h"

#define PATH "avif/"
#define MAX_FILEPATH_LENGTH 200

int main() {
	FILE *fp;
	gdImagePtr imFromPng = NULL, imFromAvif = NULL;
	void *avifImDataPtr = NULL, *pngImDataPtr = NULL;
	int size;
	char filePath[MAX_FILEPATH_LENGTH], pngFilePath[MAX_FILEPATH_LENGTH], avifFilePath[MAX_FILEPATH_LENGTH];
	char errMsg[MAX_FILEPATH_LENGTH + 100];

	const int filesCount = 4;
	const char *filenames[filesCount] = {"baboon", "dice_with_alpha", "plum_blossom_12bit", "sunset"};

	for (int i = 0; i < filesCount; i++) {

		// First, encode each PNG into an AVIF (with the GD format as an intermediary),
		// then compare the result with the original PNG.

		strcpy(filePath, PATH);
		strcat(filePath, filenames[i]);
		strcat(strcpy(pngFilePath, filePath), ".png");
		strcat(strcpy(avifFilePath, filePath), ".avif");

		fp = gdTestFileOpen(pngFilePath);
		imFromPng = gdImageCreateFromPng(fp);
		fclose(fp);

		strcat(strcpy(errMsg, filenames[i]), ".png: gdImageCreateFromPng failed\n");
		if (!gdTestAssertMsg(imFromPng != NULL, errMsg))
			goto avif2png;

		strcat(strcpy(errMsg, filenames[i]), ": gdImageAvifPtrEx failed\n");
		avifImDataPtr = gdImageAvifPtrEx(imFromPng, &size, 100, 0);
		if (!gdTestAssertMsg(avifImDataPtr != NULL, errMsg))
			goto avif2png;

		strcat(strcpy(errMsg, filenames[i]), ": gdImageCreateFromAvifPtr failed\n");
		imFromAvif = gdImageCreateFromAvifPtr(size, avifImDataPtr);
		if (!gdTestAssertMsg(imFromAvif != NULL, errMsg))
			goto avif2png;

		strcat(strcpy(errMsg, filenames[i]), ".png: Encoded AVIF image did not match original PNG\n");
		gdTestAssertMsg(gdAssertImageEquals(imFromPng, imFromAvif), errMsg);

		// Then, decode each AVIF into a GD format, and compare that with the orginal PNG.

avif2png:

/* Skip this reverse test for now, until we can find images that encode to PNGs
	 losslessly.

		fp = gdTestFileOpen(avifFilePath);
		imFromAvif = gdImageCreateFromAvif(fp);
		fclose(fp);

		strcat(strcpy(errMsg, filenames[i]), ".avif: gdImageCreateFromAvif failed\n");
		if (!gdTestAssertMsg(imFromAvif != NULL, errMsg))
			continue;

		strcat(strcpy(errMsg, filenames[i]), ".avif: Encoded PNG image did not match original AVIF\n");
		gdTestAssertMsg(gdAssertImageEqualsToFile(pngFilePath, imFromAvif), errMsg);
	}
*/

	if (imFromPng)
		gdImageDestroy(imFromPng);

	if (imFromAvif)
		gdImageDestroy(imFromAvif);

	if (avifImDataPtr)
		gdFree(avifImDataPtr);

	if (pngImDataPtr)
		gdFree(pngImDataPtr);

	return gdNumFailures();
}
