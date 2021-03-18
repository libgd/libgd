/**
 * Validation test that checks if the custom memory management methods are
 * actually being set, called and cleared correctly.
 */

#include "gd.h"
#include "gdhelpers.h"
#include "gdtest.h"

static int testedCalloc = GD_FALSE;
static int testedMalloc = GD_FALSE;
static int testedRealloc = GD_FALSE;
static int testedFree = GD_FALSE;

static void *testcalloc(size_t nmemb, size_t size)
{
	char *ptr;

	ptr = calloc(nmemb, size);

	testedCalloc = GD_TRUE;

	return (void *)ptr;
}

static void *testmalloc(size_t size)
{
	char *ptr;

	ptr = malloc(size);

	testedMalloc = GD_TRUE;

	return (void *)ptr;
}

static void *testrealloc(void *ptr, size_t size)
{
	char *ret;

	ret = realloc(ptr, size);

	testedRealloc = GD_TRUE;

	return (void *)ret;
}

static void testfree(void *ptr)
{
	free(ptr);

	testedFree = GD_TRUE;
}

int main()
{
	gdImagePtr im1, im2, im3;
	FILE *fp;
	CuTestImageResult result = {0, 0};

	fp = gdTestFileOpen2("gdmem", "coin.png");
	if (!gdTestAssert(fp != NULL))
		return gdNumFailures();

	im1 = gdImageCreateFromPng(fp);
	if (!gdTestAssert(im1 != NULL))
		goto door1;

	gdSetMemoryCallocMethod(testcalloc);
	gdSetMemoryMallocMethod(testmalloc);
	gdSetMemoryReallocMethod(testrealloc);
	gdSetMemoryFreeMethod(testfree);
	fseek(fp, 0, SEEK_SET);
	im2 = gdImageCreateFromPng(fp);
	if (!gdTestAssert(im2 != NULL))
		goto door2;

	gdTestImageDiff(im1, im2, NULL, &result);
	gdTestAssertMsg(result.pixels_changed == 0, "pixels changed: %d\n", result.pixels_changed);

	im3 = gdImageCreatePaletteFromTrueColor(im2, GD_FALSE, 256);
	if (gdTestAssert(im3 != NULL))
		gdImageDestroy(im3);

	gdTestAssertMsg(testedCalloc, "Custom calloc hasn't been called!");
	gdTestAssertMsg(testedMalloc, "Custom malloc hasn't been called!");
	gdTestAssertMsg(testedFree, "Custom free hasn't been called!");

	testedFree = GD_FALSE;

	void *reallocData = gdRealloc(NULL, 16);
	gdTestAssert(reallocData != NULL);
	gdFree(reallocData);
	gdTestAssertMsg(testedRealloc, "Custom realloc hasn't been called!");
	gdTestAssertMsg(testedFree, "Custom free hasn't been called!");

	gdImageDestroy(im2);
door2:
	gdClearMemoryCallocMethod();
	gdClearMemoryMallocMethod();
	gdClearMemoryReallocMethod();
	gdClearMemoryFreeMethod();
	gdImageDestroy(im1);
door1:
	fclose(fp);

	return gdNumFailures();
}
