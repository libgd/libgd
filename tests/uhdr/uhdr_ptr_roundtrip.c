#include "gd.h"
#include "gdtest.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static unsigned char *read_sample_file(int *size)
{
	FILE *fp;
	long len;
	unsigned char *buf;

	if (!size) {
		return NULL;
	}

	fp = gdTestFileOpen("uhdr/uhdr_sample.jpg");
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

	buf = (unsigned char *) malloc((size_t) len);
	if (!buf) {
		fclose(fp);
		return NULL;
	}

	if (fread(buf, 1, (size_t) len, fp) != (size_t) len) {
		free(buf);
		fclose(fp);
		return NULL;
	}

	fclose(fp);
	*size = (int) len;
	return buf;
}

int main()
{
	unsigned char *input = NULL;
	void *output = NULL;
	gdUhdrImagePtr im = NULL;
	gdUhdrImagePtr out_im = NULL;
	gdUhdrError err;
	int input_size = 0;
	int output_size = 0;
	int rc;

	input = read_sample_file(&input_size);
	if (!gdTestAssertMsg(input != NULL, "failed to read UltraHDR sample fixture\n")) {
		goto cleanup;
	}

	memset(&err, 0, sizeof(err));
	im = gdUhdrImageCreateFromPtr(input_size, input, GD_UHDR_FORMAT_JPEG, &err);
	if (!gdTestAssertMsg(im != NULL, "create from ptr failed: code=%d provider=%d message=%s\n",
		err.code, err.provider_code, err.message)) {
		goto cleanup;
	}

	memset(&err, 0, sizeof(err));
	rc = gdUhdrImageCrop(im, 0, 0, 640, 360, &err);
	if (!gdTestAssertMsg(rc == GD_UHDR_SUCCESS, "crop failed: code=%d provider=%d message=%s\n",
		err.code, err.provider_code, err.message)) {
		goto cleanup;
	}

	memset(&err, 0, sizeof(err));
	output = gdUhdrImageWritePtr(im, &output_size, GD_UHDR_FORMAT_JPEG, 90, &err);
	if (!gdTestAssertMsg(output != NULL, "write ptr failed: code=%d provider=%d message=%s\n",
		err.code, err.provider_code, err.message)) {
		goto cleanup;
	}
	if (!gdTestAssertMsg(output_size > 0, "write ptr returned non-positive size\n")) {
		goto cleanup;
	}

	memset(&err, 0, sizeof(err));
	out_im = gdUhdrImageCreateFromPtr(output_size, output, GD_UHDR_FORMAT_JPEG, &err);
	if (!gdTestAssertMsg(out_im != NULL, "reload from ptr failed: code=%d provider=%d message=%s\n",
		err.code, err.provider_code, err.message)) {
		goto cleanup;
	}
	gdTestAssertMsg(gdUhdrImageWidth(out_im) == 640, "expected width 640, got %d\n", gdUhdrImageWidth(out_im));
	gdTestAssertMsg(gdUhdrImageHeight(out_im) == 360, "expected height 360, got %d\n", gdUhdrImageHeight(out_im));
	gdTestAssertMsg(gdUhdrImageHasGainMap(out_im) == 1, "expected output gain map\n");

	cleanup:
	if (out_im != NULL) {
		gdUhdrImageDestroy(out_im);
	}
	if (output != NULL) {
		gdFree(output);
	}
	if (im != NULL) {
		gdUhdrImageDestroy(im);
	}
	free(input);

	return gdNumFailures();
}
