#include "gd.h"
#include "gdtest.h"


int main()
{
	gdImagePtr src, dst;
	int size;

	src = gdImageCreateTrueColor(1, 10);
	gdTestAssert(src != NULL);

	src->sx = 0;

	dst = gdImageAvifPtrEx(src, &size, 0, GD_HEIF_CHROMA_444);
	gdTestAssert(dst == NULL);

	gdImageDestroy(src);

	return gdNumFailures();
}
