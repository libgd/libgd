#ifdef __cplusplus
extern "C" {
#endif

#ifndef GD_COLOR_MAP_H
#define GD_COLOR_MAP_H 1

#include "gd.h"

typedef struct {
  char *color_name;
  int red;
  int green;
  int blue;
} gdColorMapEntry;

typedef struct {
  int num_entries;
  gdColorMapEntry *entries;
} gdColorMap;

BGD_EXPORT_DATA_PROT gdColorMap GD_COLOR_MAP_X11;

BGD_DECLARE(int) gdColorMapLookup(const gdColorMap color_map, const char *color_name, int *r, int *g, int *b);

#endif

#ifdef __cplusplus
}
#endif
