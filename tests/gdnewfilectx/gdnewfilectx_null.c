#include "gd.h"

int main()
{
	if (gdNewFileCtx(NULL) != NULL) return 1;
	return 0;
}
