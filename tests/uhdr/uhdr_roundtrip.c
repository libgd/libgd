#include "gd.h"
#include "gdtest.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main()
{
	gdUhdrImagePtr im = NULL;
	gdUhdrImagePtr reloaded = NULL;
	gdImagePtr sdr = NULL;
	gdImagePtr sdr_reloaded = NULL;
	gdUhdrError err;
	char *sample_path = NULL;
	char *uhdr_path = NULL;
	char *sdr_path = NULL;
	FILE *fp = NULL;
	int rc;

	sample_path = gdTestFilePath("uhdr/uhdr_sample.jpg");
	if (!gdTestAssertMsg(sample_path != NULL, "failed to resolve UltraHDR sample path\n")) {
		goto cleanup;
	}

	memset(&err, 0, sizeof(err));
	im = gdUhdrImageCreateFromFile(sample_path, GD_UHDR_FORMAT_JPEG, &err);
	if (!gdTestAssertMsg(im != NULL, "failed to load UltraHDR sample: code=%d provider=%d message=%s\n",
		err.code, err.provider_code, err.message)) {
		goto cleanup;
	}

	gdTestAssertMsg(gdUhdrImageWidth(im) == 1280, "expected sample width 1280, got %d\n", gdUhdrImageWidth(im));
	gdTestAssertMsg(gdUhdrImageHeight(im) == 720, "expected sample height 720, got %d\n", gdUhdrImageHeight(im));
	gdTestAssertMsg(gdUhdrImageHasGainMap(im) == 1, "expected input gain map\n");

	memset(&err, 0, sizeof(err));
	rc = gdUhdrImageResize(im, 640, 360, &err);
	if (!gdTestAssertMsg(rc == GD_UHDR_SUCCESS, "resize failed: code=%d provider=%d message=%s\n",
		err.code, err.provider_code, err.message)) {
		goto cleanup;
	}

	memset(&err, 0, sizeof(err));
	rc = gdUhdrImageRotate(im, 90, &err);
	if (!gdTestAssertMsg(rc == GD_UHDR_SUCCESS, "rotate failed: code=%d provider=%d message=%s\n",
		err.code, err.provider_code, err.message)) {
		goto cleanup;
	}

	memset(&err, 0, sizeof(err));
	rc = gdUhdrImageMirror(im, GD_UHDR_MIRROR_HORIZONTAL, &err);
	if (!gdTestAssertMsg(rc == GD_UHDR_SUCCESS, "mirror failed: code=%d provider=%d message=%s\n",
		err.code, err.provider_code, err.message)) {
		goto cleanup;
	}

	uhdr_path = gdTestTempFile("uhdr_roundtrip.jpg");
	sdr_path = gdTestTempFile("uhdr_roundtrip_sdr.jpg");
	if (!gdTestAssertMsg(uhdr_path != NULL && sdr_path != NULL, "failed to create temp paths\n")) {
		goto cleanup;
	}

	memset(&err, 0, sizeof(err));
	rc = gdUhdrImageFile(im, uhdr_path, GD_UHDR_FORMAT_JPEG, 90, &err);
	if (!gdTestAssertMsg(rc == GD_UHDR_SUCCESS, "UltraHDR write failed: code=%d provider=%d message=%s\n",
		err.code, err.provider_code, err.message)) {
		goto cleanup;
	}

	memset(&err, 0, sizeof(err));
	reloaded = gdUhdrImageCreateFromFile(uhdr_path, GD_UHDR_FORMAT_JPEG, &err);
	if (!gdTestAssertMsg(reloaded != NULL, "reloading output failed: code=%d provider=%d message=%s\n",
		err.code, err.provider_code, err.message)) {
		goto cleanup;
	}
	gdTestAssertMsg(gdUhdrImageWidth(reloaded) == 360, "expected output width 360 after resize+rotate, got %d\n", gdUhdrImageWidth(reloaded));
	gdTestAssertMsg(gdUhdrImageHeight(reloaded) == 640, "expected output height 640 after resize+rotate, got %d\n", gdUhdrImageHeight(reloaded));
	gdTestAssertMsg(gdUhdrImageHasGainMap(reloaded) == 1, "expected output gain map\n");

	memset(&err, 0, sizeof(err));
	sdr = gdUhdrImageGetSdr(im, &err);
	if (!gdTestAssertMsg(sdr != NULL, "SDR extraction failed: code=%d provider=%d message=%s\n",
		err.code, err.provider_code, err.message)) {
		goto cleanup;
	}

	fp = fopen(sdr_path, "wb");
	if (!gdTestAssertMsg(fp != NULL, "failed to open temp SDR output\n")) {
		goto cleanup;
	}
	if (fp != NULL && sdr != NULL) {
		gdImageJpeg(sdr, fp, 90);
		fclose(fp);
		fp = NULL;
	}

	fp = fopen(sdr_path, "rb");
	if (!gdTestAssertMsg(fp != NULL, "failed to reopen extracted SDR JPEG\n")) {
		goto cleanup;
	}
	sdr_reloaded = gdImageCreateFromJpeg(fp);
	fclose(fp);
	fp = NULL;
	if (!gdTestAssertMsg(sdr_reloaded != NULL, "failed to reload extracted SDR JPEG\n")) {
		goto cleanup;
	}

	if (sdr_reloaded != NULL) {
		gdTestAssertMsg(gdImageSX(sdr_reloaded) > 0 && gdImageSY(sdr_reloaded) > 0,
			"reloaded SDR image has invalid dimensions\n");
	}

	cleanup:
	if (fp != NULL) {
		fclose(fp);
	}
	if (sdr_reloaded != NULL) {
		gdImageDestroy(sdr_reloaded);
	}
	if (sdr != NULL) {
		gdImageDestroy(sdr);
	}
	if (reloaded != NULL) {
		gdUhdrImageDestroy(reloaded);
	}
	if (im != NULL) {
		gdUhdrImageDestroy(im);
	}
	gdFree(sample_path);
	gdFree(uhdr_path);
	gdFree(sdr_path);

	return gdNumFailures();
}
