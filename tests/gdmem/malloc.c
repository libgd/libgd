#include "gd.h"
#include "gdtest.h"

void *testcalloc(size_t nmemb, size_t size)
{
	char *ptr;

	ptr = calloc(nmemb, size);

	for (size_t i = 0; i < size; i++)
		*(ptr + i) = 0;

	return (void *)ptr;
}

void *testmalloc(size_t size)
{
	char *ptr;

	ptr = malloc(size);

	for (size_t i = 0; i < size; i++)
		*(ptr + i) = 0;

	return (void *)ptr;
}

void *testrealloc(void *ptr, size_t size)
{
	char *ret;

	ret = realloc(ptr, size);

	return (void *)ret;
}

void testfree(void *ptr)
{
	if (ptr != NULL)
		free(ptr);
}

int main()
{
	gdImagePtr im1, im2;
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

	gdImageDestroy(im2);
door2:
	gdSetMemoryFreeMethod(free);
	gdImageDestroy(im1);
door1:
	fclose(fp);

	return gdNumFailures();
}