#include <string.h>
#include "gd.h"
#include "gdtest.h"

int main()
{
	gdFTStringExtra strex;
	char *path;

	path = gdTestFilePath("freetype/DejaVuSans.ttf");
	strex.flags = gdFTEX_RETURNFONTPATHNAME;
	strex.fontpath = NULL;
	gdImageStringFTEx(NULL, NULL, 0, path, 72, 0, 0, 0, "hello, gd", &strex);
	if (!strex.fontpath) {
		return 1;
	}
	if (strcmp(path, strex.fontpath) != 0) {
		return 2;
	}
	gdFree(strex.fontpath);
	gdFontCacheShutdown();
	free(path);
	return 0;
}
