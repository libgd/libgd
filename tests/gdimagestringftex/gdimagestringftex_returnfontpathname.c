#include <string.h>
#include "gd.h"
#include "gdtest.h"

int main()
{
	gdFTStringExtra strex;
	char path[2048];

	sprintf(path, "%s/freetype/DejaVuSans.ttf", GDTEST_TOP_DIR);
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
	return 0;
}
