#include "gd.h"
#include "gdtest.h"

#include <errno.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ultrahdr_api.h>
#ifndef _WIN32
#include <sys/wait.h>
#endif

#ifndef _WIN32
static int has_single_quote(const char *s)
{
	const char *p;

	if (!s) {
		return 0;
	}

	for (p = s; *p != '\0'; p++) {
		if (*p == '\'') {
			return 1;
		}
	}

	return 0;
}
#endif

#ifdef _WIN32
static int has_double_quote(const char *s)
{
	const char *p;

	if (!s) {
		return 0;
	}

	for (p = s; *p != '\0'; p++) {
		if (*p == '"') {
			return 1;
		}
	}

	return 0;
}
#endif

static int cli_status_succeeded(int status, const char *cmd)
{
#ifdef _WIN32
	if (status != 0) {
		gdTestErrorMsg("CLI invocation failed: exit=%d, command=%s\n", status, cmd);
		return 0;
	}
#else
	if (WIFEXITED(status)) {
		int exit_code = WEXITSTATUS(status);
		if (exit_code != 0) {
			gdTestErrorMsg("CLI invocation failed: exit=%d, command=%s\n", exit_code, cmd);
			return 0;
		}
	} else {
		gdTestErrorMsg("CLI invocation did not exit normally: command=%s\n", cmd);
		return 0;
	}
#endif

	return 1;
}

static int run_cli_decode(const char *input_jpg, const char *metadata_path, const char *raw_path)
{
	char cmd[4096];
	int status;
	int rc;

	if (!input_jpg || !metadata_path || !raw_path) {
		gdTestErrorMsg("CLI invocation has NULL path argument\n");
		return 0;
	}

#ifdef _WIN32
	if (has_double_quote(input_jpg) || has_double_quote(metadata_path) || has_double_quote(raw_path)) {
		gdTestErrorMsg("CLI invocation paths must not include double quote characters\n");
		return 0;
	}

	rc = snprintf(cmd, sizeof(cmd),
		"ultrahdr_app -m 1 -j \"%s\" -f \"%s\" -z \"%s\" >nul 2>&1",
		input_jpg, metadata_path, raw_path);
#else
	if (has_single_quote(input_jpg) || has_single_quote(metadata_path) || has_single_quote(raw_path)) {
		gdTestErrorMsg("CLI invocation paths must not include single quote characters\n");
		return 0;
	}

	rc = snprintf(cmd, sizeof(cmd),
		"ultrahdr_app -m 1 -j '%s' -f '%s' -z '%s' >/dev/null 2>&1",
		input_jpg, metadata_path, raw_path);
#endif
	if (rc < 0 || (size_t) rc >= sizeof(cmd)) {
		gdTestErrorMsg("CLI command buffer overflow while preparing command\n");
		return 0;
	}

	status = system(cmd);
	if (status == -1) {
		gdTestErrorMsg("CLI invocation failed to spawn process: errno=%d\n", errno);
		return 0;
	}

	return cli_status_succeeded(status, cmd);
}

static unsigned char *read_binary_file(const char *path, int *size)
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
	*size = (int) len;
	return data;
}

static int compare_files_exact(const char *lhs_path, const char *rhs_path, const char *label)
{
	unsigned char *lhs = NULL;
	unsigned char *rhs = NULL;
	int lhs_size = 0;
	int rhs_size = 0;
	int i;

	lhs = read_binary_file(lhs_path, &lhs_size);
	rhs = read_binary_file(rhs_path, &rhs_size);
	if (!lhs || !rhs) {
		gdTestErrorMsg("failed to read %s files for exact comparison\n", label);
		free(lhs);
		free(rhs);
		return 0;
	}

	if (lhs_size != rhs_size) {
		gdTestErrorMsg("%s size mismatch: %d vs %d\n", label, lhs_size, rhs_size);
		free(lhs);
		free(rhs);
		return 0;
	}

	for (i = 0; i < lhs_size; i++) {
		if (lhs[i] != rhs[i]) {
			gdTestErrorMsg("%s byte mismatch at index %d: %u vs %u\n",
				label, i, (unsigned) lhs[i], (unsigned) rhs[i]);
			free(lhs);
			free(rhs);
			return 0;
		}
	}

	free(lhs);
	free(rhs);
	return 1;
}

static int read_u16be(const unsigned char *data)
{
	return (int) data[0] * 256 + (int) data[1];
}

static int is_sof_marker(int marker)
{
	return (marker >= 0xc0 && marker <= 0xcf &&
		marker != 0xc4 && marker != 0xc8 && marker != 0xcc);
}

static int jpeg_has_no_component_subsampling(const unsigned char *data, size_t size)
{
	size_t pos;

	if (!data || size < 4 || data[0] != 0xff || data[1] != 0xd8) {
		return 0;
	}

	pos = 2;
	while (pos + 4 <= size) {
		int marker;
		int length;

		while (pos < size && data[pos] == 0xff) {
			pos++;
		}
		if (pos >= size) {
			return 0;
		}

		marker = data[pos++];
		if (marker == 0xd9 || marker == 0xda) {
			return 0;
		}
		if ((marker >= 0xd0 && marker <= 0xd7) || marker == 0x01) {
			continue;
		}
		if (pos + 2 > size) {
			return 0;
		}

		length = read_u16be(data + pos);
		if (length < 2 || pos + (size_t) length > size) {
			return 0;
		}

		if (is_sof_marker(marker)) {
			int component_count;
			int i;

			if (length < 8) {
				return 0;
			}
			component_count = data[pos + 7];
			if (component_count <= 0 || length < 8 + 3 * component_count) {
				return 0;
			}
			for (i = 0; i < component_count; i++) {
				unsigned char sampling = data[pos + 8 + 3 * i + 1];
				if (sampling != 0x11) {
					return 0;
				}
			}
			return 1;
		}

		pos += (size_t) length;
	}

	return 0;
}

static int output_gainmap_has_no_subsampling(const char *path)
{
	unsigned char *data = NULL;
	int size = 0;
	uhdr_codec_private_t *dec = NULL;
	uhdr_compressed_image_t input;
	uhdr_mem_block_t *gainmap;
	uhdr_error_info_t rc;
	int ok = 0;

	data = read_binary_file(path, &size);
	if (!data) {
		gdTestErrorMsg("failed to read UHDR output for gain map sampling check\n");
		return 0;
	}

	dec = uhdr_create_decoder();
	if (!dec) {
		gdTestErrorMsg("failed to create UltraHDR decoder for gain map sampling check\n");
		free(data);
		return 0;
	}

	memset(&input, 0, sizeof(input));
	input.data = data;
	input.data_sz = (size_t) size;
	input.capacity = (size_t) size;
	input.cg = UHDR_CG_UNSPECIFIED;
	input.ct = UHDR_CT_UNSPECIFIED;
	input.range = UHDR_CR_FULL_RANGE;

	rc = uhdr_dec_set_image(dec, &input);
	if (rc.error_code != UHDR_CODEC_OK) {
		gdTestErrorMsg("uhdr_dec_set_image failed during gain map sampling check: %d\n", rc.error_code);
		goto cleanup;
	}

	rc = uhdr_dec_probe(dec);
	if (rc.error_code != UHDR_CODEC_OK) {
		gdTestErrorMsg("uhdr_dec_probe failed during gain map sampling check: %d\n", rc.error_code);
		goto cleanup;
	}

	gainmap = uhdr_dec_get_gainmap_image(dec);
	if (!gainmap || !gainmap->data || gainmap->data_sz == 0) {
		gdTestErrorMsg("missing compressed gain map during sampling check\n");
		goto cleanup;
	}

	ok = jpeg_has_no_component_subsampling((const unsigned char *) gainmap->data, gainmap->data_sz);
	if (!ok) {
		gdTestErrorMsg("gain map JPEG uses component subsampling\n");
	}

cleanup:
	uhdr_release_decoder(dec);
	free(data);
	return ok;
}

static int read_uhdr_base_jpeg_metadata(const char *path, gdImageMetadata *metadata)
{
	unsigned char *data = NULL;
	int size = 0;
	uhdr_codec_private_t *dec = NULL;
	uhdr_compressed_image_t input;
	uhdr_mem_block_t *base;
	uhdr_error_info_t rc;
	gdImagePtr decoded = NULL;
	int ok = 0;

	data = read_binary_file(path, &size);
	if (!data) {
		gdTestErrorMsg("failed to read UHDR image for base JPEG metadata check\n");
		return 0;
	}

	dec = uhdr_create_decoder();
	if (!dec) {
		gdTestErrorMsg("failed to create UltraHDR decoder for base JPEG metadata check\n");
		free(data);
		return 0;
	}

	memset(&input, 0, sizeof(input));
	input.data = data;
	input.data_sz = (size_t) size;
	input.capacity = (size_t) size;
	input.cg = UHDR_CG_UNSPECIFIED;
	input.ct = UHDR_CT_UNSPECIFIED;
	input.range = UHDR_CR_FULL_RANGE;

	rc = uhdr_dec_set_image(dec, &input);
	if (rc.error_code != UHDR_CODEC_OK) {
		gdTestErrorMsg("uhdr_dec_set_image failed during base JPEG metadata check: %d\n", rc.error_code);
		goto cleanup;
	}

	rc = uhdr_dec_probe(dec);
	if (rc.error_code != UHDR_CODEC_OK) {
		gdTestErrorMsg("uhdr_dec_probe failed during base JPEG metadata check: %d\n", rc.error_code);
		goto cleanup;
	}

	base = uhdr_dec_get_base_image(dec);
	if (!base || !base->data || base->data_sz == 0 || base->data_sz > (size_t) INT_MAX) {
		gdTestErrorMsg("missing or oversized compressed base image during metadata check\n");
		goto cleanup;
	}

	decoded = gdImageCreateFromJpegPtrWithMetadata((int) base->data_sz, base->data, metadata);
	if (!decoded) {
		gdTestErrorMsg("failed to decode compressed base image during metadata check\n");
		goto cleanup;
	}

	ok = 1;

cleanup:
	if (decoded) {
		gdImageDestroy(decoded);
	}
	uhdr_release_decoder(dec);
	free(data);
	return ok;
}

static gdImagePtr read_uhdr_base_jpeg_image(const char *path)
{
	unsigned char *data = NULL;
	int size = 0;
	uhdr_codec_private_t *dec = NULL;
	uhdr_compressed_image_t input;
	uhdr_mem_block_t *base;
	uhdr_error_info_t rc;
	gdImagePtr decoded = NULL;

	data = read_binary_file(path, &size);
	if (!data) {
		gdTestErrorMsg("failed to read UHDR image for base JPEG image check\n");
		return NULL;
	}

	dec = uhdr_create_decoder();
	if (!dec) {
		gdTestErrorMsg("failed to create UltraHDR decoder for base JPEG image check\n");
		free(data);
		return NULL;
	}

	memset(&input, 0, sizeof(input));
	input.data = data;
	input.data_sz = (size_t) size;
	input.capacity = (size_t) size;
	input.cg = UHDR_CG_UNSPECIFIED;
	input.ct = UHDR_CT_UNSPECIFIED;
	input.range = UHDR_CR_FULL_RANGE;

	rc = uhdr_dec_set_image(dec, &input);
	if (rc.error_code != UHDR_CODEC_OK) {
		gdTestErrorMsg("uhdr_dec_set_image failed during base JPEG image check: %d\n", rc.error_code);
		goto cleanup;
	}

	rc = uhdr_dec_probe(dec);
	if (rc.error_code != UHDR_CODEC_OK) {
		gdTestErrorMsg("uhdr_dec_probe failed during base JPEG image check: %d\n", rc.error_code);
		goto cleanup;
	}

	base = uhdr_dec_get_base_image(dec);
	if (!base || !base->data || base->data_sz == 0 || base->data_sz > (size_t) INT_MAX) {
		gdTestErrorMsg("missing or oversized compressed base image during image check\n");
		goto cleanup;
	}

	decoded = gdImageCreateFromJpegPtr((int) base->data_sz, base->data);
	if (!decoded) {
		gdTestErrorMsg("failed to decode compressed base image during image check\n");
	}

cleanup:
	uhdr_release_decoder(dec);
	free(data);
	return decoded;
}

static int resized_base_matches_direct_resize(const char *source_path, const char *output_path,
	int width, int height)
{
	gdImagePtr source_base = NULL;
	gdImagePtr expected = NULL;
	gdImagePtr actual = NULL;
	int x_step;
	int y_step;
	int x;
	int y;
	int samples = 0;
	long long total_delta = 0;
	int ok = 0;

	source_base = read_uhdr_base_jpeg_image(source_path);
	actual = read_uhdr_base_jpeg_image(output_path);
	if (!source_base || !actual) {
		goto cleanup;
	}

	if (!gdImageSetInterpolationMethod(source_base, GD_MITCHELL)) {
		gdTestErrorMsg("failed to set direct GD resize interpolation method\n");
		goto cleanup;
	}
	expected = gdImageScale(source_base, (unsigned int) width, (unsigned int) height);
	if (!expected) {
		gdTestErrorMsg("failed to create direct GD resize reference\n");
		goto cleanup;
	}

	if (gdImageSX(actual) != width || gdImageSY(actual) != height) {
		gdTestErrorMsg("resized base dimensions mismatch: %dx%d vs %dx%d\n",
			gdImageSX(actual), gdImageSY(actual), width, height);
		goto cleanup;
	}

	x_step = width / 32;
	y_step = height / 24;
	if (x_step <= 0) {
		x_step = 1;
	}
	if (y_step <= 0) {
		y_step = 1;
	}

	for (y = 0; y < height; y += y_step) {
		for (x = 0; x < width; x += x_step) {
			int expected_px = gdImageGetTrueColorPixel(expected, x, y);
			int actual_px = gdImageGetTrueColorPixel(actual, x, y);
			total_delta += abs(gdTrueColorGetRed(expected_px) - gdTrueColorGetRed(actual_px));
			total_delta += abs(gdTrueColorGetGreen(expected_px) - gdTrueColorGetGreen(actual_px));
			total_delta += abs(gdTrueColorGetBlue(expected_px) - gdTrueColorGetBlue(actual_px));
			samples++;
		}
	}

	if (samples <= 0) {
		gdTestErrorMsg("no samples collected during resized base comparison\n");
		goto cleanup;
	}

	if (total_delta / (samples * 3) > 35) {
		gdTestErrorMsg("resized base image differs too much from direct GD resize: average delta=%lld\n",
			total_delta / (samples * 3));
		goto cleanup;
	}

	ok = 1;

cleanup:
	if (actual) {
		gdImageDestroy(actual);
	}
	if (expected) {
		gdImageDestroy(expected);
	}
	if (source_base) {
		gdImageDestroy(source_base);
	}
	return ok;
}

static int metadata_profile_matches(const gdImageMetadata *lhs, const gdImageMetadata *rhs,
	const char *key, const char *label)
{
	const unsigned char *lhs_profile;
	const unsigned char *rhs_profile;
	size_t lhs_size = 0;
	size_t rhs_size = 0;

	lhs_profile = gdImageMetadataGetProfile(lhs, key, &lhs_size);
	rhs_profile = gdImageMetadataGetProfile(rhs, key, &rhs_size);
	if (!lhs_profile || !rhs_profile) {
		gdTestErrorMsg("missing %s profile during metadata check\n", label);
		return 0;
	}
	if (lhs_size != rhs_size || memcmp(lhs_profile, rhs_profile, lhs_size) != 0) {
		gdTestErrorMsg("%s profile changed during metadata check\n", label);
		return 0;
	}

	return 1;
}

int main(void)
{
	char *sample_path = NULL;
	char *src_meta = NULL;
	char *src_raw = NULL;
	char *gd_out_jpg = NULL;
	char *gd_meta = NULL;
	char *gd_raw = NULL;
	gdUhdrImagePtr im = NULL;
	gdUhdrImagePtr reloaded = NULL;
	gdImageMetadata *src_base_metadata = NULL;
	gdImageMetadata *gd_base_metadata = NULL;
	gdUhdrError err;
	int rc;

	if (!gdTestAssertMsg(gdUhdrIsAvailable() == 1, "UltraHDR support should be enabled for this test\n")) {
		goto cleanup;
	}

	sample_path = gdTestFilePath("uhdr/uhdr_sample.jpg");
	if (!gdTestAssertMsg(sample_path != NULL, "failed to resolve UltraHDR sample path\n")) {
		goto cleanup;
	}

	src_meta = gdTestTempFile("uhdr_resize_src_metadata.cfg");
	src_raw = gdTestTempFile("uhdr_resize_src_dump.raw");
	gd_out_jpg = gdTestTempFile("uhdr_resize_output.jpg");
	gd_meta = gdTestTempFile("uhdr_resize_gd_metadata.cfg");
	gd_raw = gdTestTempFile("uhdr_resize_gd_dump.raw");
	if (!gdTestAssertMsg(src_meta && src_raw && gd_out_jpg && gd_meta && gd_raw,
		"failed to allocate temp file paths\n")) {
		goto cleanup;
	}
	src_base_metadata = gdImageMetadataCreate();
	gd_base_metadata = gdImageMetadataCreate();
	if (!gdTestAssertMsg(src_base_metadata && gd_base_metadata,
		"failed to allocate JPEG metadata containers\n")) {
		goto cleanup;
	}

	if (!gdTestAssertMsg(run_cli_decode(sample_path, src_meta, src_raw),
		"CLI decode for baseline image failed\n")) {
		goto cleanup;
	}
	if (!gdTestAssertMsg(read_uhdr_base_jpeg_metadata(sample_path, src_base_metadata),
		"failed to read source base JPEG metadata\n")) {
		goto cleanup;
	}

	memset(&err, 0, sizeof(err));
	im = gdUhdrImageCreateFromFile(sample_path, GD_UHDR_FORMAT_JPEG, &err);
	if (!gdTestAssertMsg(im != NULL,
		"gdUhdrImageCreateFromFile failed: code=%d provider=%d message=%s\n",
		err.code, err.provider_code, err.message)) {
		goto cleanup;
	}

	memset(&err, 0, sizeof(err));
	rc = gdUhdrImageResize(im, 640, 360, &err);
	if (!gdTestAssertMsg(rc == GD_UHDR_SUCCESS,
		"gdUhdrImageResize failed: code=%d provider=%d message=%s\n",
		err.code, err.provider_code, err.message)) {
		goto cleanup;
	}

	memset(&err, 0, sizeof(err));
	rc = gdUhdrImageFile(im, gd_out_jpg, GD_UHDR_FORMAT_JPEG, 50, &err);
	if (!gdTestAssertMsg(rc == GD_UHDR_SUCCESS,
		"gdUhdrImageFile failed: code=%d provider=%d message=%s\n",
		err.code, err.provider_code, err.message)) {
		goto cleanup;
	}

	if (!gdTestAssertMsg(run_cli_decode(gd_out_jpg, gd_meta, gd_raw),
		"CLI decode for GD resized image failed\n")) {
		goto cleanup;
	}

	if (!gdTestAssertMsg(compare_files_exact(src_meta, gd_meta, "gain map metadata"),
		"resized output changed gain map metadata\n")) {
		goto cleanup;
	}
	if (!gdTestAssertMsg(output_gainmap_has_no_subsampling(gd_out_jpg),
		"resized output gain map should not be chroma subsampled\n")) {
		goto cleanup;
	}
	if (!gdTestAssertMsg(read_uhdr_base_jpeg_metadata(gd_out_jpg, gd_base_metadata),
		"failed to read resized output base JPEG metadata\n")) {
		goto cleanup;
	}
	if (!gdTestAssertMsg(metadata_profile_matches(src_base_metadata, gd_base_metadata, "icc", "base ICC"),
		"resized output should preserve the base ICC profile exactly\n")) {
		goto cleanup;
	}
	if (!gdTestAssertMsg(gdImageMetadataGetProfile(gd_base_metadata, "exif", NULL) == NULL,
		"resized output should not copy stale source EXIF into the transformed base JPEG\n")) {
		goto cleanup;
	}
	if (!gdTestAssertMsg(resized_base_matches_direct_resize(sample_path, gd_out_jpg, 640, 360),
		"resized output base should match direct GD resize rather than a crop\n")) {
		goto cleanup;
	}

	memset(&err, 0, sizeof(err));
	reloaded = gdUhdrImageCreateFromFile(gd_out_jpg, GD_UHDR_FORMAT_JPEG, &err);
	if (!gdTestAssertMsg(reloaded != NULL,
		"failed to reload GD output as UHDR: code=%d provider=%d message=%s\n",
		err.code, err.provider_code, err.message)) {
		goto cleanup;
	}

	gdTestAssertMsg(gdUhdrImageWidth(reloaded) == 640,
		"expected resized output width 640, got %d\n", gdUhdrImageWidth(reloaded));
	gdTestAssertMsg(gdUhdrImageHeight(reloaded) == 360,
		"expected resized output height 360, got %d\n", gdUhdrImageHeight(reloaded));
	gdTestAssertMsg(gdUhdrImageHasGainMap(reloaded) == 1, "expected resized output gain map\n");

cleanup:
	if (reloaded) {
		gdUhdrImageDestroy(reloaded);
	}
	if (im) {
		gdUhdrImageDestroy(im);
	}
	if (gd_base_metadata) {
		gdImageMetadataFree(gd_base_metadata);
	}
	if (src_base_metadata) {
		gdImageMetadataFree(src_base_metadata);
	}
	gdFree(sample_path);
	gdFree(src_meta);
	gdFree(src_raw);
	gdFree(gd_out_jpg);
	gdFree(gd_meta);
	gdFree(gd_raw);

	return gdNumFailures();
}
