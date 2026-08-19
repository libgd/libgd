#!/bin/sh --
# Small shell script to build gd from source

v() { echo "$@"; "$@"; }

if uname | grep -qi '^darwin' ; then
	echo "Having trouble on macOS? Try: brew install autoconf libtool automake gettext pkg-config cmake"
fi

if ! v autoreconf -f -i ; then
	exit 1
fi

(
echo "/* Generated from config.hin via autoheader for cmake; see bootstrap.sh. */"
sed -E \
	-e '1d' \
	-e '/ENABLE_/s:#undef:#cmakedefine01:' \
	-e 's:#undef:#cmakedefine:' \
	src/config.hin
) > src/config.h.cmake
