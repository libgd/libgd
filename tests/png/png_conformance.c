#include "gd.h"
#include "gdtest.h"

#include <stdio.h>

static const char *valid_files[] = {
	"14b47384-7042-11e5-801d-804da7b4cbe6.png",
	"Disable_auto_recalculation_26.png",
	"wm_upload_wikimedia_org_3a9fa5185de5c6c8.png",
	"wm_upload_wikimedia_org_45634e241d7821a3.png",
	"wm_upload_wikimedia_org_4edbe895c4c29af5.png",
	"wm_upload_wikimedia_org_8d8a8e9b8a7cf5bf.png",
	"wm_upload_wikimedia_org_a23d1e831e128dff.png",
	"wm_upload_wikimedia_org_c8a458b0cef3d942.png",
	"wm_upload_wikimedia_org_f14b0faca19b77e2.png",
	"wm_upload_wikimedia_org_f6c96971fbd1da0d.png"
};

static const char *invalid_files[] = {
	"badadler.png"
};

static gdImagePtr read_png(const char *dir, const char *filename, int *opened)
{
	FILE *fp;
	gdImagePtr im;

	*opened = 0;
	fp = gdTestFileOpenX("png", "png-conformance", dir, filename, NULL);
	if (fp == NULL) {
		return NULL;
	}
	*opened = 1;
	im = gdImageCreateFromPng(fp);
	fclose(fp);
	return im;
}

static void assert_valid_file(const char *filename)
{
	int opened;
	gdImagePtr im = read_png("valid", filename, &opened);

	gdTestAssertMsg(opened, "cannot open valid PNG conformance file: %s\n", filename);
	if (!opened) {
		return;
	}
	gdTestAssertMsg(im != NULL, "valid PNG failed to decode: %s\n", filename);
	if (im == NULL) {
		return;
	}

	gdTestAssertMsg(gdImageSX(im) > 0 && gdImageSY(im) > 0,
	                "decoded PNG has invalid dimensions: %s\n", filename);
	gdImageDestroy(im);
}

static void assert_invalid_file(const char *filename)
{
	int opened;
	gdImagePtr im = read_png("invalid", filename, &opened);

	gdTestAssertMsg(opened, "cannot open invalid PNG conformance file: %s\n", filename);
	if (!opened) {
		return;
	}
	gdTestAssertMsg(im == NULL, "invalid PNG decoded successfully: %s\n", filename);
	if (im != NULL) {
		gdImageDestroy(im);
	}
}

int main(void)
{
	size_t i;

	gdSetErrorMethod(gdSilence);

	for (i = 0; i < sizeof(valid_files) / sizeof(valid_files[0]); i++) {
		assert_valid_file(valid_files[i]);
	}

	for (i = 0; i < sizeof(invalid_files) / sizeof(invalid_files[0]); i++) {
		assert_invalid_file(invalid_files[i]);
	}

	gdClearErrorMethod();
	return gdNumFailures();
}
