#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <stdio.h>
#include "gd.h"
#include "gd_color_map.h"

int
main(void)
{
	int r, g, b;
	int i;
	for (i=0; i<GD_COLOR_MAP_X11.num_entries; i++) {
		char *color_name = GD_COLOR_MAP_X11.entries[i].color_name;
		if (gdColorMapLookup(GD_COLOR_MAP_X11, color_name, &r, &g, &b)) {
			printf("%s found: #%02x%02x%02x\n", color_name, r, g, b);
		} else {
			fprintf(stderr, "%s not found\n", color_name);
			return 1;
		}
	}
	if (gdColorMapLookup(GD_COLOR_MAP_X11, "no such name", &r, &g, &b)) {
		return 2;
	}
	return 0;
}
