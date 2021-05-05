/**
 * Basic test for gdImageConvolution() and related functions
 */

#include "gd.h"
#include "gdtest.h"

static void test_convolution(void (*convolution_func)(gdImagePtr im), const char *expected)
{
	gdImagePtr im;
	FILE *fp;
	char *path;

	fp = gdTestFileOpen2("gdimageconvolution", "basic.png");
	im = gdImageCreateFromPng(fp);
	fclose(fp);

	convolution_func(im);

	path = gdTestFilePath2("gdimageconvolution", expected);
	gdAssertImageEqualsToFile(path, im);
	gdFree(path);

	gdImageDestroy(im);
}

static void test_edge_detect_quick(gdImagePtr im)
{
	gdImageEdgeDetectQuick(im);
}

static void test_smooth(gdImagePtr im)
{
	gdImageSmooth(im, 5);
}

static void test_emboss(gdImagePtr im)
{
	gdImageEmboss(im);
}

static void test_mean_removal(gdImagePtr im)
{
	gdImageMeanRemoval(im);
}

int main()
{
	test_convolution(&test_edge_detect_quick, "basic_edge_detect_quick.png");
	test_convolution(&test_smooth, "basic_smooth.png");
	test_convolution(&test_emboss, "basic_emboss.png");
	test_convolution(&test_mean_removal, "basic_mean_removal.png");

	return gdNumFailures();
}
