/**
 * Testing the basic operation of the gdImageFlip*() functions
 */

#include "gd.h"
#include "gdtest.h"

static void test_flip(void (BGD_STDCALL *func)(gdImagePtr), const char *filename)
{
	gdImagePtr im;
	FILE *fp;
	char *path;

	fp = gdTestFileOpen2("gdimageflip", "remi.png");
	im = gdImageCreateFromPng(fp);
	fclose(fp);

	func(im);

	path = gdTestFilePath2("gdimageflip", filename);
	gdAssertImageEqualsToFile(path, im);
	gdFree(path);

	gdImageDestroy(im);
}

int main()
{
	test_flip(gdImageFlipVertical, "gdimageflipvertical_exp.png");
	test_flip(gdImageFlipHorizontal, "gdimagefliphorizontal_exp.png");
	test_flip(gdImageFlipBoth, "gdimageflipboth_exp.png");

	return gdNumFailures();
}
