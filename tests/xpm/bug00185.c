/* $Id$ */
#include "gd.h"
#include <stdio.h>
#include <stdlib.h>
#include "gdtest.h"

/* To check memory leaks, run such as 'valgrind --leak-check=full ./bug00185' */
int
main()
{
	gdImagePtr im;
	char path[1024];

	sprintf(path, "%s/xpm/bug00185.xpm", GDTEST_TOP_DIR);
	im = gdImageCreateFromXpm(path);
	if (!im) return 1;
	gdImageDestroy(im);

	sprintf(path, "%s/xpm/bug00185_damaged.xpm", GDTEST_TOP_DIR);
	im = gdImageCreateFromXpm(path);
	if (!im) return 0;
	gdImageDestroy(im);

	return 2;
}
