#ifdef HAVE_CONFIG_H
#include "config.h"
#endif /* HAVE_CONFIG_H */

#include "gd.h"


/* These functions return the version information.  We use functions
 * so that changes in the shared library will automatically be
 * reflected in executables using it without recompiling them. */

BGD_DECLARE(int) gdMajorVersion()
{
    return GD_MAJOR_VERSION;
}

BGD_DECLARE(int) gdMinorVersion()
{
    return GD_MINOR_VERSION;
}

BGD_DECLARE(int) gdReleaseVersion()
{
    return GD_RELEASE_VERSION;
}

BGD_DECLARE(const char *) gdExtraVersion() {
    return GD_EXTRA_VERSION;
}

BGD_DECLARE(const char *) gdVersionString() {
    return GD_VERSION_STRING;
}
