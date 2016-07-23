#!/bin/sh

GETVER="${0%/*}/getver.pl"
GDLIB_MAJOR=$("${GETVER}" MAJOR)
GDLIB_MINOR=$("${GETVER}" MINOR)
GDLIB_REVISION=$("${GETVER}" RELEASE)

# Dynamic library version information
# See http://www.gnu.org/software/libtool/manual/libtool.html#Updating-version-info

GDLIB_LT_CURRENT=3
# This is the version where the soname (current above) changes.  We use it
# to reset the revision base back to zero.  It's a bit of a pain, but some
# systems restrict the revision range below to [0..255] (like OS X).
GDLIB_PREV_MAJOR=2
GDLIB_PREV_MINOR=2
# This isn't 100% correct, but it tends to be a close enough approximation
# for how we manage the codebase.  It's rare to do a release that doesn't
# modify the library since this project is centered around the library.
GDLIB_LT_REVISION=$(( ((GDLIB_MAJOR - GDLIB_PREV_MAJOR) << 6) | ((GDLIB_MINOR - GDLIB_PREV_MINOR) << 3) | GDLIB_REVISION ))
GDLIB_LT_AGE=0

# The first three fields we feed into libtool and the OS target determines how
# they get used.  The last two fields we feed into cmake.  We use the same rules
# as Linux SONAME versioning in libtool, but cmake should handle it for us.
case $1 in
CURRENT)
	printf '%s' "${GDLIB_LT_CURRENT}"
	;;
REVISION)
	printf '%s' "${GDLIB_LT_REVISION}"
	;;
AGE)
	printf '%s' "${GDLIB_LT_AGE}"
	;;
VERSION)
	printf '%s' "$(( GDLIB_LT_CURRENT - GDLIB_LT_AGE )).${GDLIB_LT_AGE}.${GDLIB_LT_REVISION}"
	;;
SONAME)
	printf '%s' "$(( GDLIB_LT_CURRENT - GDLIB_LT_AGE ))"
	;;
esac
