#include "gd.h"
#include "gdtest.h"

static void test_two_ifds_fixture(void)
{
	FILE *fp;
	gdTiffReadPtr reader;
	gdTiffInfo info;
	gdTiffPageInfo page;
	gdImagePtr im;
	int pages = 0;

	fp = gdTestFileOpenX("tiff", "tiff-conformance", "edge-cases", "test_two_ifds.tif", NULL);
	gdTestAssertMsg(fp != NULL, "cannot open test_two_ifds.tif");
	if (fp == NULL) {
		return;
	}

	reader = gdTiffReadOpen(fp);
	fclose(fp);
	gdTestAssertMsg(reader != NULL, "cannot open multipage TIFF fixture");
	if (reader == NULL) {
		return;
	}

	gdTestAssert(gdTiffReadGetInfo(reader, &info) == 1);
	gdTestAssertMsg(info.pageCount == 2, "expected 2 pages, got %d", info.pageCount);

	while (gdTiffReadNextImage(reader, &page, &im) == 1) {
		gdTestAssertMsg(page.pageIndex == pages, "expected page index %d, got %d", pages, page.pageIndex);
		gdTestAssertMsg(im != NULL, "page %d decoded to NULL image", pages);
		if (im != NULL) {
			gdTestAssertMsg(gdImageSX(im) == page.width && gdImageSY(im) == page.height,
			                "page %d dimensions disagree with page info", pages);
		}
		pages++;
	}

	gdTestAssertMsg(pages == 2, "expected to decode 2 pages, got %d", pages);
	gdTiffReadClose(reader);
}

int main(void)
{
	gdSetErrorMethod(gdSilence);
	test_two_ifds_fixture();
	gdClearErrorMethod();
	return gdNumFailures();
}
