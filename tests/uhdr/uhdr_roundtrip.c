#include "gd.h"
#include "gdtest.h"

#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ultrahdr_api.h>

static unsigned char *read_binary_file(const char *path, size_t *size)
{
	FILE *fp;
	long len;
	unsigned char *data;

	if (!path || !size) {
		return NULL;
	}

	fp = fopen(path, "rb");
	if (!fp) {
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

	data = (unsigned char *) malloc((size_t) len);
	if (!data) {
		fclose(fp);
		return NULL;
	}

	if (fread(data, 1, (size_t) len, fp) != (size_t) len) {
		free(data);
		fclose(fp);
		return NULL;
	}

	fclose(fp);
	*size = (size_t) len;
	return data;
}

static int read_gainmap_jpeg_dimensions(const char *path, int *width, int *height)
{
	unsigned char *data = NULL;
	size_t size = 0;
	uhdr_codec_private_t *dec = NULL;
	uhdr_compressed_image_t input;
	uhdr_mem_block_t *gainmap;
	uhdr_error_info_t rc;
	gdImagePtr gainmap_image = NULL;
	int ok = 0;

	if (!width || !height) {
		return 0;
	}

	data = read_binary_file(path, &size);
	if (!data) {
		gdTestErrorMsg("failed to read UHDR image for gain map dimension check\n");
		return 0;
	}

	dec = uhdr_create_decoder();
	if (!dec) {
		gdTestErrorMsg("failed to create UltraHDR decoder for gain map dimension check\n");
		goto cleanup;
	}

	memset(&input, 0, sizeof(input));
	input.data = data;
	input.data_sz = size;
	input.capacity = size;
	input.cg = UHDR_CG_UNSPECIFIED;
	input.ct = UHDR_CT_UNSPECIFIED;
	input.range = UHDR_CR_FULL_RANGE;

	rc = uhdr_dec_set_image(dec, &input);
	if (rc.error_code != UHDR_CODEC_OK) {
		gdTestErrorMsg("uhdr_dec_set_image failed during gain map dimension check: %d\n", rc.error_code);
		goto cleanup;
	}

	rc = uhdr_dec_probe(dec);
	if (rc.error_code != UHDR_CODEC_OK) {
		gdTestErrorMsg("uhdr_dec_probe failed during gain map dimension check: %d\n", rc.error_code);
		goto cleanup;
	}

	gainmap = uhdr_dec_get_gainmap_image(dec);
	if (!gainmap || !gainmap->data || gainmap->data_sz == 0 || gainmap->data_sz > (size_t) INT_MAX) {
		gdTestErrorMsg("missing or oversized compressed gain map during dimension check\n");
		goto cleanup;
	}

	gainmap_image = gdImageCreateFromJpegPtr((int) gainmap->data_sz, gainmap->data);
	if (!gainmap_image) {
		gdTestErrorMsg("failed to decode compressed gain map during dimension check\n");
		goto cleanup;
	}

	*width = gdImageSX(gainmap_image);
	*height = gdImageSY(gainmap_image);
	ok = 1;

cleanup:
	if (gainmap_image) {
		gdImageDestroy(gainmap_image);
	}
	if (dec) {
		uhdr_release_decoder(dec);
	}
	free(data);
	return ok;
}

static int scale_dimension(int value, int from_extent, int to_extent)
{
	return (int) (((long long) value * (long long) to_extent + from_extent / 2) / from_extent);
}

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
	int src_gainmap_w = 0;
	int src_gainmap_h = 0;
	int out_gainmap_w = 0;
	int out_gainmap_h = 0;
	int crop_gainmap_w;
	int crop_gainmap_h;
	int resized_gainmap_w;
	int resized_gainmap_h;

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
	rc = gdUhdrImageCrop(im, 0, 0, 960, 720, &err);
	if (!gdTestAssertMsg(rc == GD_UHDR_SUCCESS, "crop failed: code=%d provider=%d message=%s\n",
		err.code, err.provider_code, err.message)) {
		goto cleanup;
	}

	if (!gdTestAssertMsg(read_gainmap_jpeg_dimensions(sample_path, &src_gainmap_w, &src_gainmap_h),
		"failed to read source gain map dimensions\n")) {
		goto cleanup;
	}

	memset(&err, 0, sizeof(err));
	rc = gdUhdrImageResize(im, 480, 360, &err);
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
	gdTestAssertMsg(gdUhdrImageHeight(reloaded) == 480, "expected output height 480 after crop+resize+rotate, got %d\n", gdUhdrImageHeight(reloaded));
	gdTestAssertMsg(gdUhdrImageHasGainMap(reloaded) == 1, "expected output gain map\n");

	crop_gainmap_w = scale_dimension(960, 1280, src_gainmap_w);
	crop_gainmap_h = scale_dimension(720, 720, src_gainmap_h);
	resized_gainmap_w = scale_dimension(crop_gainmap_w, 960, 480);
	resized_gainmap_h = scale_dimension(crop_gainmap_h, 720, 360);
	if (!gdTestAssertMsg(read_gainmap_jpeg_dimensions(uhdr_path, &out_gainmap_w, &out_gainmap_h),
		"failed to read transformed gain map dimensions\n")) {
		goto cleanup;
	}
	gdTestAssertMsg(out_gainmap_w == resized_gainmap_h,
		"expected transformed gain map width %d, got %d\n", resized_gainmap_h, out_gainmap_w);
	gdTestAssertMsg(out_gainmap_h == resized_gainmap_w,
		"expected transformed gain map height %d, got %d\n", resized_gainmap_w, out_gainmap_h);

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
