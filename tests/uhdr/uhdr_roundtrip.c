#include "gd.h"
#include "gdtest.h"

#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ultrahdr_api.h>

#ifndef GD_UHDR_MAX_DIMENSION
#define GD_UHDR_MAX_DIMENSION 8192
#endif

static unsigned char *read_binary_file(const char *path, size_t *size) {
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

	data = (unsigned char *)malloc((size_t)len);
	if (!data) {
		fclose(fp);
		return NULL;
	}

	if (fread(data, 1, (size_t)len, fp) != (size_t)len) {
		free(data);
		fclose(fp);
		return NULL;
	}

	fclose(fp);
	*size = (size_t)len;
	return data;
}

static int read_gainmap_jpeg_dimensions(const char *path, int *width,
										int *height) {
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
		gdTestErrorMsg(
			"failed to read UHDR image for gain map dimension check\n");
		return 0;
	}

	dec = uhdr_create_decoder();
	if (!dec) {
		gdTestErrorMsg(
			"failed to create UltraHDR decoder for gain map dimension check\n");
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
		gdTestErrorMsg(
			"uhdr_dec_set_image failed during gain map dimension check: %d\n",
			rc.error_code);
		goto cleanup;
	}

	rc = uhdr_dec_probe(dec);
	if (rc.error_code != UHDR_CODEC_OK) {
		gdTestErrorMsg(
			"uhdr_dec_probe failed during gain map dimension check: %d\n",
			rc.error_code);
		goto cleanup;
	}

	gainmap = uhdr_dec_get_gainmap_image(dec);
	if (!gainmap || !gainmap->data || gainmap->data_sz == 0 ||
		gainmap->data_sz > (size_t)INT_MAX) {
		gdTestErrorMsg("missing or oversized compressed gain map during "
					   "dimension check\n");
		goto cleanup;
	}

	gainmap_image =
		gdImageCreateFromJpegPtr((int)gainmap->data_sz, gainmap->data);
	if (!gainmap_image) {
		gdTestErrorMsg(
			"failed to decode compressed gain map during dimension check\n");
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

static int read_gainmap_dimensions_from_data(void *data, int size, int *width,
												 int *height) {
	uhdr_codec_private_t *dec;
	uhdr_compressed_image_t input;
	uhdr_error_info_t rc;
	int ok = 0;

	if (!data || size <= 0 || !width || !height) {
		return 0;
	}
	dec = uhdr_create_decoder();
	if (!dec) {
		return 0;
	}

	memset(&input, 0, sizeof(input));
	input.data = data;
	input.data_sz = (size_t)size;
	input.capacity = (size_t)size;
	input.cg = UHDR_CG_UNSPECIFIED;
	input.ct = UHDR_CT_UNSPECIFIED;
	input.range = UHDR_CR_FULL_RANGE;
	rc = uhdr_dec_set_image(dec, &input);
	if (rc.error_code == UHDR_CODEC_OK) {
		rc = uhdr_dec_probe(dec);
	}
	if (rc.error_code == UHDR_CODEC_OK) {
		*width = uhdr_dec_get_gainmap_width(dec);
		*height = uhdr_dec_get_gainmap_height(dec);
		ok = *width > 0 && *height > 0;
	}

	uhdr_release_decoder(dec);
	return ok;
}

static unsigned char *create_scaled_gainmap_uhdr(const char *path, int width,
												 int height, int *output_size) {
	unsigned char *source_data = NULL;
	unsigned char *output_data = NULL;
	void *gainmap_jpeg_data = NULL;
	size_t source_size = 0;
	uhdr_codec_private_t *dec = NULL;
	uhdr_codec_private_t *enc = NULL;
	uhdr_compressed_image_t source;
	uhdr_compressed_image_t base_jpeg;
	uhdr_compressed_image_t gainmap_jpeg;
	uhdr_compressed_image_t *encoded;
	uhdr_mem_block_t *base_block;
	uhdr_mem_block_t *gainmap_block;
	uhdr_gainmap_metadata_t *source_metadata;
	uhdr_gainmap_metadata_t metadata;
	uhdr_error_info_t rc;
	gdImagePtr gainmap_image = NULL;
	gdImagePtr scaled_gainmap = NULL;
	int gainmap_jpeg_size = 0;

	if (!path || width <= 0 || height <= 0 || !output_size) {
		return NULL;
	}
	*output_size = 0;

	source_data = read_binary_file(path, &source_size);
	if (!source_data || source_size > (size_t)INT_MAX) {
		goto cleanup;
	}

	dec = uhdr_create_decoder();
	enc = uhdr_create_encoder();
	if (!dec || !enc) {
		goto cleanup;
	}

	memset(&source, 0, sizeof(source));
	source.data = source_data;
	source.data_sz = source_size;
	source.capacity = source_size;
	source.cg = UHDR_CG_UNSPECIFIED;
	source.ct = UHDR_CT_UNSPECIFIED;
	source.range = UHDR_CR_FULL_RANGE;
	rc = uhdr_dec_set_image(dec, &source);
	if (rc.error_code != UHDR_CODEC_OK) {
		gdTestErrorMsg("scaled gain map: uhdr_dec_set_image failed: %d %s\n",
					   rc.error_code, rc.has_detail ? rc.detail : "");
		goto cleanup;
	}
	rc = uhdr_dec_probe(dec);
	if (rc.error_code != UHDR_CODEC_OK) {
		gdTestErrorMsg("scaled gain map: uhdr_dec_probe failed: %d %s\n",
					   rc.error_code, rc.has_detail ? rc.detail : "");
		goto cleanup;
	}

	base_block = uhdr_dec_get_base_image(dec);
	gainmap_block = uhdr_dec_get_gainmap_image(dec);
	source_metadata = uhdr_dec_get_gainmap_metadata(dec);
	if (!base_block || !base_block->data || base_block->data_sz == 0 ||
		!gainmap_block || !gainmap_block->data || gainmap_block->data_sz == 0 ||
		gainmap_block->data_sz > (size_t)INT_MAX || !source_metadata) {
		goto cleanup;
	}
	metadata = *source_metadata;
	metadata.use_base_cg = 1;

	gainmap_image = gdImageCreateFromJpegPtr((int)gainmap_block->data_sz,
											 gainmap_block->data);
	if (!gainmap_image ||
		!gdImageSetInterpolationMethod(gainmap_image, GD_MITCHELL)) {
		goto cleanup;
	}
	scaled_gainmap = gdImageScale(gainmap_image, (unsigned int)width,
								 (unsigned int)height);
	if (!scaled_gainmap) {
		goto cleanup;
	}
	gainmap_jpeg_data = gdImageJpegPtr(scaled_gainmap, &gainmap_jpeg_size, 95);
	if (!gainmap_jpeg_data || gainmap_jpeg_size <= 0) {
		goto cleanup;
	}

	memset(&base_jpeg, 0, sizeof(base_jpeg));
	base_jpeg.data = base_block->data;
	base_jpeg.data_sz = base_block->data_sz;
	base_jpeg.capacity = base_block->data_sz;
	base_jpeg.cg = UHDR_CG_UNSPECIFIED;
	base_jpeg.ct = UHDR_CT_UNSPECIFIED;
	base_jpeg.range = UHDR_CR_FULL_RANGE;
	memset(&gainmap_jpeg, 0, sizeof(gainmap_jpeg));
	gainmap_jpeg.data = gainmap_jpeg_data;
	gainmap_jpeg.data_sz = (size_t)gainmap_jpeg_size;
	gainmap_jpeg.capacity = (size_t)gainmap_jpeg_size;
	gainmap_jpeg.cg = UHDR_CG_UNSPECIFIED;
	gainmap_jpeg.ct = UHDR_CT_UNSPECIFIED;
	gainmap_jpeg.range = UHDR_CR_FULL_RANGE;

	rc = uhdr_enc_set_output_format(enc, UHDR_CODEC_JPG);
	if (rc.error_code != UHDR_CODEC_OK) {
		gdTestErrorMsg("scaled gain map: output format failed: %d %s\n",
					   rc.error_code, rc.has_detail ? rc.detail : "");
		goto cleanup;
	}
	rc = uhdr_enc_set_compressed_image(enc, &base_jpeg, UHDR_BASE_IMG);
	if (rc.error_code != UHDR_CODEC_OK) {
		gdTestErrorMsg("scaled gain map: base image failed: %d %s\n",
					   rc.error_code, rc.has_detail ? rc.detail : "");
		goto cleanup;
	}
	rc = uhdr_enc_set_gainmap_image(enc, &gainmap_jpeg, &metadata);
	if (rc.error_code != UHDR_CODEC_OK) {
		gdTestErrorMsg("scaled gain map: gain map failed: %d %s\n",
					   rc.error_code, rc.has_detail ? rc.detail : "");
		goto cleanup;
	}
	rc = uhdr_encode(enc);
	if (rc.error_code != UHDR_CODEC_OK) {
		gdTestErrorMsg("scaled gain map: encode failed: %d %s\n", rc.error_code,
					   rc.has_detail ? rc.detail : "");
		goto cleanup;
	}
	encoded = uhdr_get_encoded_stream(enc);
	if (!encoded || !encoded->data || encoded->data_sz == 0 ||
		encoded->data_sz > (size_t)INT_MAX) {
		goto cleanup;
	}

	output_data = (unsigned char *)malloc(encoded->data_sz);
	if (!output_data) {
		goto cleanup;
	}
	memcpy(output_data, encoded->data, encoded->data_sz);
	*output_size = (int)encoded->data_sz;

cleanup:
	if (scaled_gainmap) {
		gdImageDestroy(scaled_gainmap);
	}
	if (gainmap_image) {
		gdImageDestroy(gainmap_image);
	}
	gdFree(gainmap_jpeg_data);
	if (enc) {
		uhdr_release_encoder(enc);
	}
	if (dec) {
		uhdr_release_decoder(dec);
	}
	free(source_data);
	return output_data;
}

static int scale_dimension(int value, int from_extent, int to_extent) {
	return (int)(((long long)value * (long long)to_extent) / from_extent);
}

static gdImagePtr expected_primary_transform(gdImagePtr source) {
	gdRect crop = {0, 0, 960, 720};
	gdImagePtr cropped = NULL;
	gdImagePtr resized = NULL;
	gdImagePtr rotated = NULL;

	cropped = gdImageCrop(source, &crop);
	if (!cropped || !gdImageSetInterpolationMethod(cropped, GD_MITCHELL)) {
		gdImageDestroy(cropped);
		return NULL;
	}

	resized = gdImageScale(cropped, 480, 360);
	gdImageDestroy(cropped);
	if (!resized) {
		return NULL;
	}

	/* The public UHDR rotation angle is clockwise. */
	rotated = gdImageRotateInterpolated(resized, 270.0f, 0);
	gdImageDestroy(resized);
	if (!rotated) {
		return NULL;
	}

	gdImageFlipHorizontal(rotated);
	return rotated;
}

static gdImagePtr expected_secondary_transform(gdImagePtr source) {
	gdRect crop = {17, 29, 503, 301};
	gdImagePtr rotated_270 = NULL;
	gdImagePtr cropped = NULL;
	gdImagePtr rotated_180 = NULL;

	/* Clockwise 270 degrees is GD's counterclockwise 90-degree primitive. */
	rotated_270 = gdImageRotateInterpolated(source, 90.0f, 0);
	if (!rotated_270) {
		return NULL;
	}

	cropped = gdImageCrop(rotated_270, &crop);
	gdImageDestroy(rotated_270);
	if (!cropped) {
		return NULL;
	}

	rotated_180 = gdImageRotateInterpolated(cropped, 180.0f, 0);
	gdImageDestroy(cropped);
	if (!rotated_180) {
		return NULL;
	}

	gdImageFlipVertical(rotated_180);
	return rotated_180;
}

int main() {
	gdUhdrImagePtr im = NULL;
	gdUhdrImagePtr invalid_im = NULL;
	gdUhdrImagePtr oversized_im = NULL;
	gdUhdrImagePtr reduced_gainmap_im = NULL;
	gdUhdrImagePtr enlarged_gainmap_im = NULL;
	gdUhdrImagePtr enlarged_gainmap_reloaded = NULL;
	gdUhdrImagePtr secondary_im = NULL;
	gdUhdrImagePtr reloaded = NULL;
	gdImagePtr original_sdr = NULL;
	gdImagePtr expected_sdr = NULL;
	gdImagePtr sdr = NULL;
	gdImagePtr secondary_original_sdr = NULL;
	gdImagePtr secondary_expected_sdr = NULL;
	gdImagePtr secondary_sdr = NULL;
	gdImagePtr sdr_reloaded = NULL;
	gdUhdrError err;
	char *sample_path = NULL;
	char *uhdr_path = NULL;
	char *sdr_path = NULL;
	FILE *fp = NULL;
	int rc;
	int i;
	int src_gainmap_w = 0;
	int src_gainmap_h = 0;
	int out_gainmap_w = 0;
	int out_gainmap_h = 0;
	int crop_gainmap_w;
	int crop_gainmap_h;
	int resized_gainmap_w;
	int resized_gainmap_h;
	void *transaction_output = NULL;
	int transaction_output_size = 0;
	void *boundary_output = NULL;
	int boundary_output_size = 0;
	unsigned char *reduced_gainmap_data = NULL;
	int reduced_gainmap_size = 0;
	void *reduced_gainmap_output = NULL;
	int reduced_gainmap_output_size = 0;
	int reduced_output_gainmap_w = 0;
	int reduced_output_gainmap_h = 0;
	unsigned char *enlarged_gainmap_data = NULL;
	int enlarged_gainmap_size = 0;
	void *enlarged_gainmap_output = NULL;
	int enlarged_gainmap_output_size = 0;
	int enlarged_output_gainmap_w = 0;
	int enlarged_output_gainmap_h = 0;

	sample_path = gdTestFilePath("uhdr/uhdr_sample.jpg");
	if (!gdTestAssertMsg(sample_path != NULL,
						 "failed to resolve UltraHDR sample path\n")) {
		goto cleanup;
	}

	memset(&err, 0, sizeof(err));
	im = gdUhdrImageCreateFromFile(sample_path, GD_UHDR_FORMAT_JPEG, &err);
	if (!gdTestAssertMsg(
			im != NULL,
			"failed to load UltraHDR sample: code=%d provider=%d message=%s\n",
			err.code, err.provider_code, err.message)) {
		goto cleanup;
	}

	gdTestAssertMsg(gdUhdrImageWidth(im) == 1280,
					"expected sample width 1280, got %d\n",
					gdUhdrImageWidth(im));
	gdTestAssertMsg(gdUhdrImageHeight(im) == 720,
					"expected sample height 720, got %d\n",
					gdUhdrImageHeight(im));
	gdTestAssertMsg(gdUhdrImageHasGainMap(im) == 1,
					"expected input gain map\n");

	if (!gdTestAssertMsg(read_gainmap_jpeg_dimensions(
						 sample_path, &src_gainmap_w, &src_gainmap_h),
					 "failed to read source gain map dimensions\n")) {
		goto cleanup;
	}

	memset(&err, 0, sizeof(err));
	original_sdr = gdUhdrImageGetSdr(im, &err);
	if (!gdTestAssertMsg(
			original_sdr != NULL,
			"initial SDR extraction failed: code=%d provider=%d message=%s\n",
			err.code, err.provider_code, err.message)) {
		goto cleanup;
	}

	memset(&err, 0, sizeof(err));
	invalid_im =
		gdUhdrImageCreateFromFile(sample_path, GD_UHDR_FORMAT_JPEG, &err);
	if (!gdTestAssertMsg(
			invalid_im != NULL,
			"failed to load crop-validation handle: code=%d provider=%d message=%s\n",
			err.code, err.provider_code, err.message)) {
		goto cleanup;
	}

	memset(&err, 0, sizeof(err));
	rc = gdUhdrImageCrop(invalid_im, 1279, 0, 2, 1, &err);
	gdTestAssertMsg(rc == GD_UHDR_E_INVALID,
					"out-of-bounds crop was accepted: code=%d message=%s\n",
					err.code, err.message);
	gdTestAssertMsg(gdUhdrImageWidth(invalid_im) == 1280 &&
						gdUhdrImageHeight(invalid_im) == 720,
					"rejected crop changed queued geometry to %dx%d\n",
					gdUhdrImageWidth(invalid_im), gdUhdrImageHeight(invalid_im));

	memset(&err, 0, sizeof(err));
	oversized_im =
		gdUhdrImageCreateFromFile(sample_path, GD_UHDR_FORMAT_JPEG, &err);
	if (!gdTestAssertMsg(oversized_im != NULL,
						 "failed to load resize-validation handle: code=%d message=%s\n",
						 err.code, err.message)) {
		goto cleanup;
	}
	rc = gdUhdrImageResize(oversized_im, GD_UHDR_MAX_DIMENSION, 1, &err);
	if (!gdTestAssertMsg(rc == GD_UHDR_SUCCESS &&
							gdUhdrImageWidth(oversized_im) == GD_UHDR_MAX_DIMENSION &&
							gdUhdrImageHeight(oversized_im) == 1,
						"maximum supported resize was rejected: code=%d message=%s\n",
						err.code, err.message)) {
		goto cleanup;
	}
	boundary_output = gdUhdrImageWritePtr(oversized_im, &boundary_output_size,
									 GD_UHDR_FORMAT_JPEG, 90, &err);
	if (!gdTestAssertMsg(boundary_output != NULL && boundary_output_size > 0,
						"maximum supported resize failed to write: code=%d message=%s\n",
						err.code, err.message)) {
		goto cleanup;
	}
	gdUhdrImageDestroy(oversized_im);
	oversized_im = NULL;
	gdFree(boundary_output);
	boundary_output = NULL;
	oversized_im = gdUhdrImageCreateFromFile(sample_path, GD_UHDR_FORMAT_JPEG,
											 &err);
	if (!gdTestAssertMsg(oversized_im != NULL,
						 "failed to reload resize-validation handle: code=%d message=%s\n",
						 err.code, err.message)) {
		goto cleanup;
	}
	rc = gdUhdrImageResize(oversized_im, GD_UHDR_MAX_DIMENSION + 1, 1, &err);
	gdTestAssertMsg(rc == GD_UHDR_E_INVALID,
					"oversized resize was accepted: code=%d message=%s\n",
					err.code, err.message);
	gdTestAssertMsg(gdUhdrImageWidth(oversized_im) == 1280 &&
						gdUhdrImageHeight(oversized_im) == 720,
					"rejected resize changed queued geometry to %dx%d\n",
					gdUhdrImageWidth(oversized_im), gdUhdrImageHeight(oversized_im));

	memset(&err, 0, sizeof(err));
	rc = gdUhdrImageCrop(invalid_im, 0, 0, 960, 720, &err);
	if (!gdTestAssertMsg(rc == GD_UHDR_SUCCESS,
						 "valid crop after rejection failed: code=%d message=%s\n",
						 err.code, err.message)) {
		goto cleanup;
	}
	transaction_output = gdUhdrImageWritePtr(invalid_im, &transaction_output_size,
										 GD_UHDR_FORMAT_JPEG, 90, &err);
	gdTestAssertMsg(transaction_output != NULL && transaction_output_size > 0,
					"valid write after rejected crops failed: code=%d message=%s\n",
					err.code, err.message);

	reduced_gainmap_data = create_scaled_gainmap_uhdr(
		sample_path, src_gainmap_w / 2, src_gainmap_h / 2,
		&reduced_gainmap_size);
	if (!gdTestAssertMsg(reduced_gainmap_data != NULL && reduced_gainmap_size > 0,
						 "failed to create reduced-resolution gain map fixture\n")) {
		goto cleanup;
	}
	reduced_gainmap_im = gdUhdrImageCreateFromPtr(
		reduced_gainmap_size, reduced_gainmap_data, GD_UHDR_FORMAT_JPEG, &err);
	if (!gdTestAssertMsg(reduced_gainmap_im != NULL,
						 "failed to load reduced-resolution gain map fixture: "
						 "code=%d message=%s\n",
						 err.code, err.message)) {
		goto cleanup;
	}
	memset(&err, 0, sizeof(err));
	rc = gdUhdrImageCrop(reduced_gainmap_im, 0, 0, 1, 1, &err);
	gdTestAssertMsg(rc == GD_UHDR_E_INVALID,
					"gain-map-degenerate crop was accepted: code=%d message=%s\n",
					err.code, err.message);
	gdTestAssertMsg(gdUhdrImageWidth(reduced_gainmap_im) == 1280 &&
						gdUhdrImageHeight(reduced_gainmap_im) == 720,
					"rejected gain-map crop changed queued geometry to %dx%d\n",
					gdUhdrImageWidth(reduced_gainmap_im),
					gdUhdrImageHeight(reduced_gainmap_im));
	memset(&err, 0, sizeof(err));
	rc = gdUhdrImageResize(reduced_gainmap_im, 3, 3, &err);
	if (!gdTestAssertMsg(rc == GD_UHDR_SUCCESS,
						 "reduced gain-map resize failed: code=%d message=%s\n",
						 err.code, err.message)) {
		goto cleanup;
	}
	reduced_gainmap_output = gdUhdrImageWritePtr(
		reduced_gainmap_im, &reduced_gainmap_output_size, GD_UHDR_FORMAT_JPEG, 90,
		&err);
	if (gdTestAssertMsg(reduced_gainmap_output != NULL,
						"reduced gain-map write failed: code=%d message=%s\n",
						err.code, err.message)) {
		gdTestAssertMsg(
			read_gainmap_dimensions_from_data(
				reduced_gainmap_output, reduced_gainmap_output_size,
				&reduced_output_gainmap_w, &reduced_output_gainmap_h) &&
				reduced_output_gainmap_w == 1 && reduced_output_gainmap_h == 1,
			"truncating resize expected a 1x1 gain map, got %dx%d\n",
			reduced_output_gainmap_w, reduced_output_gainmap_h);
	}

	enlarged_gainmap_data = create_scaled_gainmap_uhdr(
		sample_path, 2560, 1440, &enlarged_gainmap_size);
	if (!gdTestAssertMsg(enlarged_gainmap_data != NULL &&
							enlarged_gainmap_size > 0,
						 "failed to create enlarged gain map fixture\n")) {
		goto cleanup;
	}
	enlarged_gainmap_im = gdUhdrImageCreateFromPtr(
		enlarged_gainmap_size, enlarged_gainmap_data, GD_UHDR_FORMAT_JPEG, &err);
	if (!gdTestAssertMsg(enlarged_gainmap_im != NULL,
						 "failed to load enlarged gain map fixture: code=%d message=%s\n",
						 err.code, err.message)) {
		goto cleanup;
	}
	memset(&err, 0, sizeof(err));
	rc = gdUhdrImageResize(enlarged_gainmap_im,
							 GD_UHDR_MAX_DIMENSION / 2 + 1, 1, &err);
	gdTestAssertMsg(rc == GD_UHDR_E_INVALID,
					"resize producing an oversized gain map was accepted: "
					"code=%d message=%s\n",
					err.code, err.message);
	gdTestAssertMsg(gdUhdrImageWidth(enlarged_gainmap_im) == 1280 &&
						gdUhdrImageHeight(enlarged_gainmap_im) == 720,
					"rejected gain map resize changed queued geometry to %dx%d\n",
					gdUhdrImageWidth(enlarged_gainmap_im),
					gdUhdrImageHeight(enlarged_gainmap_im));
	memset(&err, 0, sizeof(err));
	rc = gdUhdrImageResize(enlarged_gainmap_im,
							 GD_UHDR_MAX_DIMENSION / 2, 1, &err);
	if (!gdTestAssertMsg(rc == GD_UHDR_SUCCESS,
						 "gain map boundary resize failed after rejection: "
						 "code=%d message=%s\n",
						 err.code, err.message)) {
		goto cleanup;
	}
	enlarged_gainmap_output = gdUhdrImageWritePtr(
		enlarged_gainmap_im, &enlarged_gainmap_output_size,
		GD_UHDR_FORMAT_JPEG, 90, &err);
	if (!gdTestAssertMsg(enlarged_gainmap_output != NULL &&
							enlarged_gainmap_output_size > 0,
						 "gain map boundary resize failed to write: "
						 "code=%d message=%s\n",
						 err.code, err.message)) {
		goto cleanup;
	}
	if (!gdTestAssertMsg(
			read_gainmap_dimensions_from_data(
				enlarged_gainmap_output, enlarged_gainmap_output_size,
				&enlarged_output_gainmap_w, &enlarged_output_gainmap_h) &&
				enlarged_output_gainmap_w == GD_UHDR_MAX_DIMENSION &&
				enlarged_output_gainmap_h == 2,
			"gain map boundary output expected %dx2, got %dx%d\n",
			GD_UHDR_MAX_DIMENSION, enlarged_output_gainmap_w,
			enlarged_output_gainmap_h)) {
		goto cleanup;
	}
	enlarged_gainmap_reloaded = gdUhdrImageCreateFromPtr(
		enlarged_gainmap_output_size, enlarged_gainmap_output,
		GD_UHDR_FORMAT_JPEG, &err);
	gdTestAssertMsg(enlarged_gainmap_reloaded != NULL,
					"failed to reload gain map boundary output: "
					"code=%d message=%s\n",
					err.code, err.message);

	memset(&err, 0, sizeof(err));
	rc = gdUhdrImageCrop(im, 0, 0, 960, 720, &err);
	if (!gdTestAssertMsg(rc == GD_UHDR_SUCCESS,
						 "crop failed: code=%d provider=%d message=%s\n",
						 err.code, err.provider_code, err.message)) {
		goto cleanup;
	}
	gdTestAssertMsg(gdUhdrImageWidth(im) == 960 &&
						gdUhdrImageHeight(im) == 720,
					"queued crop dimensions are %dx%d instead of 960x720\n",
					gdUhdrImageWidth(im), gdUhdrImageHeight(im));

	memset(&err, 0, sizeof(err));
	rc = gdUhdrImageResize(im, 480, 360, &err);
	if (!gdTestAssertMsg(rc == GD_UHDR_SUCCESS,
						 "resize failed: code=%d provider=%d message=%s\n",
						 err.code, err.provider_code, err.message)) {
		goto cleanup;
	}
	gdTestAssertMsg(gdUhdrImageWidth(im) == 480 &&
						gdUhdrImageHeight(im) == 360,
					"queued resize dimensions are %dx%d instead of 480x360\n",
					gdUhdrImageWidth(im), gdUhdrImageHeight(im));

	memset(&err, 0, sizeof(err));
	rc = gdUhdrImageRotate(im, 90, &err);
	if (!gdTestAssertMsg(rc == GD_UHDR_SUCCESS,
						 "rotate failed: code=%d provider=%d message=%s\n",
						 err.code, err.provider_code, err.message)) {
		goto cleanup;
	}
	gdTestAssertMsg(gdUhdrImageWidth(im) == 360 &&
						gdUhdrImageHeight(im) == 480,
					"queued rotation dimensions are %dx%d instead of 360x480\n",
					gdUhdrImageWidth(im), gdUhdrImageHeight(im));

	memset(&err, 0, sizeof(err));
	rc = gdUhdrImageMirror(im, GD_UHDR_MIRROR_HORIZONTAL, &err);
	if (!gdTestAssertMsg(rc == GD_UHDR_SUCCESS,
						 "mirror failed: code=%d provider=%d message=%s\n",
						 err.code, err.provider_code, err.message)) {
		goto cleanup;
	}
	gdTestAssertMsg(gdUhdrImageWidth(im) == 360 &&
						gdUhdrImageHeight(im) == 480,
					"queued mirror changed geometry to %dx%d\n",
					gdUhdrImageWidth(im), gdUhdrImageHeight(im));

	memset(&err, 0, sizeof(err));
	rc = gdUhdrImageCrop(im, 359, 0, 2, 1, &err);
	gdTestAssertMsg(rc == GD_UHDR_E_INVALID,
					"crop outside queued geometry was accepted: code=%d message=%s\n",
					err.code, err.message);
	gdTestAssertMsg(gdUhdrImageWidth(im) == 360 &&
						gdUhdrImageHeight(im) == 480,
					"rejected queued-state crop changed geometry to %dx%d\n",
					gdUhdrImageWidth(im), gdUhdrImageHeight(im));

	uhdr_path = gdTestTempFile("uhdr_roundtrip.jpg");
	sdr_path = gdTestTempFile("uhdr_roundtrip_sdr.jpg");
	if (!gdTestAssertMsg(uhdr_path != NULL && sdr_path != NULL,
						 "failed to create temp paths\n")) {
		goto cleanup;
	}

	memset(&err, 0, sizeof(err));
	rc = gdUhdrImageFile(im, uhdr_path, GD_UHDR_FORMAT_JPEG, 90, &err);
	if (!gdTestAssertMsg(
			rc == GD_UHDR_SUCCESS,
			"UltraHDR write failed: code=%d provider=%d message=%s\n", err.code,
			err.provider_code, err.message)) {
		goto cleanup;
	}
	memset(&err, 0, sizeof(err));
	secondary_im =
		gdUhdrImageCreateFromFile(sample_path, GD_UHDR_FORMAT_JPEG, &err);
	if (!gdTestAssertMsg(
			secondary_im != NULL,
			"failed to load secondary transform handle: code=%d provider=%d message=%s\n",
			err.code, err.provider_code, err.message)) {
		goto cleanup;
	}
	secondary_original_sdr = gdUhdrImageGetSdr(secondary_im, &err);
	if (!gdTestAssertMsg(secondary_original_sdr != NULL,
						 "failed to decode secondary original SDR: code=%d message=%s\n",
						 err.code, err.message)) {
		goto cleanup;
	}
	rc = gdUhdrImageRotate(secondary_im, 270, &err);
	gdTestAssertMsg(rc == GD_UHDR_SUCCESS &&
						gdUhdrImageWidth(secondary_im) == 720 &&
						gdUhdrImageHeight(secondary_im) == 1280,
					"clockwise 270 rotation did not queue as 720x1280\n");
	rc = gdUhdrImageCrop(secondary_im, 17, 29, 503, 301, &err);
	gdTestAssertMsg(rc == GD_UHDR_SUCCESS &&
						gdUhdrImageWidth(secondary_im) == 503 &&
						gdUhdrImageHeight(secondary_im) == 301,
					"nonzero crop after rotation did not queue as 503x301\n");
	rc = gdUhdrImageRotate(secondary_im, 180, &err);
	gdTestAssertMsg(rc == GD_UHDR_SUCCESS &&
						gdUhdrImageWidth(secondary_im) == 503 &&
						gdUhdrImageHeight(secondary_im) == 301,
					"180 rotation changed queued geometry\n");
	rc = gdUhdrImageMirror(secondary_im, GD_UHDR_MIRROR_VERTICAL, &err);
	gdTestAssertMsg(rc == GD_UHDR_SUCCESS &&
						gdUhdrImageWidth(secondary_im) == 503 &&
						gdUhdrImageHeight(secondary_im) == 301,
					"vertical mirror changed queued geometry\n");
	for (i = 0; i < 3; i++) {
		rc = gdUhdrImageMirror(secondary_im, GD_UHDR_MIRROR_HORIZONTAL, &err);
		gdTestAssertMsg(rc == GD_UHDR_SUCCESS,
						"first mirror in queue growth pair %d failed\n", i);
		rc = gdUhdrImageMirror(secondary_im, GD_UHDR_MIRROR_HORIZONTAL, &err);
		gdTestAssertMsg(rc == GD_UHDR_SUCCESS,
						"second mirror in queue growth pair %d failed\n", i);
	}
	gdTestAssertMsg(gdUhdrImageWidth(secondary_im) == 503 &&
						gdUhdrImageHeight(secondary_im) == 301,
					"queue growth mirrors changed queued geometry\n");
	secondary_expected_sdr =
		expected_secondary_transform(secondary_original_sdr);
	secondary_sdr = gdUhdrImageGetSdr(secondary_im, &err);
	if (gdTestAssertMsg(secondary_expected_sdr != NULL && secondary_sdr != NULL,
						"failed secondary SDR transform: code=%d message=%s\n",
						err.code, err.message)) {
		gdAssertImageEquals(secondary_expected_sdr, secondary_sdr);
	}

	memset(&err, 0, sizeof(err));
	reloaded = gdUhdrImageCreateFromFile(uhdr_path, GD_UHDR_FORMAT_JPEG, &err);
	if (!gdTestAssertMsg(
			reloaded != NULL,
			"reloading output failed: code=%d provider=%d message=%s\n",
			err.code, err.provider_code, err.message)) {
		goto cleanup;
	}
	gdTestAssertMsg(gdUhdrImageWidth(reloaded) == 360,
					"expected output width 360 after resize+rotate, got %d\n",
					gdUhdrImageWidth(reloaded));
	gdTestAssertMsg(
		gdUhdrImageHeight(reloaded) == 480,
		"expected output height 480 after crop+resize+rotate, got %d\n",
		gdUhdrImageHeight(reloaded));
	gdTestAssertMsg(gdUhdrImageHasGainMap(reloaded) == 1,
					"expected output gain map\n");

	crop_gainmap_w = scale_dimension(960, 1280, src_gainmap_w);
	crop_gainmap_h = scale_dimension(720, 720, src_gainmap_h);
	resized_gainmap_w = scale_dimension(crop_gainmap_w, 960, 480);
	resized_gainmap_h = scale_dimension(crop_gainmap_h, 720, 360);
	if (!gdTestAssertMsg(read_gainmap_jpeg_dimensions(uhdr_path, &out_gainmap_w,
													  &out_gainmap_h),
						 "failed to read transformed gain map dimensions\n")) {
		goto cleanup;
	}
	gdTestAssertMsg(out_gainmap_w == resized_gainmap_h,
					"expected transformed gain map width %d, got %d\n",
					resized_gainmap_h, out_gainmap_w);
	gdTestAssertMsg(out_gainmap_h == resized_gainmap_w,
					"expected transformed gain map height %d, got %d\n",
					resized_gainmap_w, out_gainmap_h);

	memset(&err, 0, sizeof(err));
	sdr = gdUhdrImageGetSdr(im, &err);
	if (!gdTestAssertMsg(
			sdr != NULL,
			"SDR extraction failed: code=%d provider=%d message=%s\n", err.code,
			err.provider_code, err.message)) {
		goto cleanup;
	}
	expected_sdr = expected_primary_transform(original_sdr);
	if (!gdTestAssertMsg(expected_sdr != NULL,
						 "failed to construct expected transformed SDR\n")) {
		goto cleanup;
	}
	gdAssertImageEquals(expected_sdr, sdr);

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
	if (!gdTestAssertMsg(sdr_reloaded != NULL,
						 "failed to reload extracted SDR JPEG\n")) {
		goto cleanup;
	}

	if (sdr_reloaded != NULL) {
		gdTestAssertMsg(gdImageSX(sdr_reloaded) > 0 &&
							gdImageSY(sdr_reloaded) > 0,
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
	if (secondary_sdr != NULL) {
		gdImageDestroy(secondary_sdr);
	}
	if (secondary_expected_sdr != NULL) {
		gdImageDestroy(secondary_expected_sdr);
	}
	if (secondary_original_sdr != NULL) {
		gdImageDestroy(secondary_original_sdr);
	}
	if (expected_sdr != NULL) {
		gdImageDestroy(expected_sdr);
	}
	if (original_sdr != NULL) {
		gdImageDestroy(original_sdr);
	}
	if (reloaded != NULL) {
		gdUhdrImageDestroy(reloaded);
	}
	if (secondary_im != NULL) {
		gdUhdrImageDestroy(secondary_im);
	}
	if (invalid_im != NULL) {
		gdUhdrImageDestroy(invalid_im);
	}
	if (oversized_im != NULL) {
		gdUhdrImageDestroy(oversized_im);
	}
	if (reduced_gainmap_im != NULL) {
		gdUhdrImageDestroy(reduced_gainmap_im);
	}
	if (enlarged_gainmap_reloaded != NULL) {
		gdUhdrImageDestroy(enlarged_gainmap_reloaded);
	}
	if (enlarged_gainmap_im != NULL) {
		gdUhdrImageDestroy(enlarged_gainmap_im);
	}
	if (im != NULL) {
		gdUhdrImageDestroy(im);
	}
	gdFree(transaction_output);
	gdFree(boundary_output);
	gdFree(reduced_gainmap_output);
	free(reduced_gainmap_data);
	gdFree(enlarged_gainmap_output);
	free(enlarged_gainmap_data);
	gdFree(sample_path);
	gdFree(uhdr_path);
	gdFree(sdr_path);

	return gdNumFailures();
}
