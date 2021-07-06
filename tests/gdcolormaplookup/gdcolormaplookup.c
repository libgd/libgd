/**
  * Test API gdColorMapLookup defined in gd_color_map.h
  * Move from src/gd_color_map_test.c
  */

#include "gd_color_map.h"
#include "gdtest.h"

int
main(void)
{
	int r, g, b;
	int i;
	int ret;
	for (i=0; i<GD_COLOR_MAP_X11.num_entries; i++) {
		char *color_name = GD_COLOR_MAP_X11.entries[i].color_name;
		ret = gdColorMapLookup(GD_COLOR_MAP_X11, color_name, &r, &g, &b);
		gdTestAssert(ret == 1);
	}
	ret = gdColorMapLookup(GD_COLOR_MAP_X11, "no such name", &r, &g, &b);
	gdTestAssert(ret == 0);

	return gdNumFailures();
}
