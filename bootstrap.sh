#!/bin/sh --
# Small shell script to build gd from source

v() { echo "$@"; "$@"; }

# Generate the manual (unless naturaldocs isn't installed).  Source
# dists should include the docs so that end users don't need to
# install naturaldocs.  At the same time, we tolerate it being missing
# so that random hackers don't need it just to build the code.
v docs/naturaldocs/run_docs.sh --nonfatal

if echo "${OSTYPE:-$(uname)}" | grep -q '^darwin' ; then
	echo "Having trouble on OS X? Try: brew install autoconf libtool automake gettext pkg-config cmake"
fi

if ! v autoreconf -f -i ; then
	exit 1
fi

(
echo "/* Generated from config.hin via autoheader for cmake; see bootstrap.sh. */"
sed -E \
	-e '1d' \
	-e '/ENABLE_/{s:#undef:#define:;s:([^ ]*)$:\1 @\1@:;}' \
	-e 's:#undef:#cmakedefine:' \
	src/config.hin
) > src/config.h.cmake
