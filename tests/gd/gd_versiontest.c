#include <string.h>

#include "gd.h"
#include "gdtest.h"


int main()
{
    char buffer[100];

    gdTestAssert(GD_MAJOR_VERSION == gdMajorVersion());
    gdTestAssert(GD_MINOR_VERSION == gdMinorVersion());
    gdTestAssert(GD_RELEASE_VERSION == gdReleaseVersion());
    gdTestAssert(strcmp(GD_EXTRA_VERSION, gdExtraVersion()) == 0);

    sprintf(buffer, "%d.%d.%d%s", GD_MAJOR_VERSION, GD_MINOR_VERSION,
             GD_RELEASE_VERSION, GD_EXTRA_VERSION);
    gdTestAssert(strcmp(GD_VERSION_STRING, gdVersionString()) == 0);

    return gdNumFailures();
}
