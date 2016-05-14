#!/bin/bash -e
# The main script for building/testing while under travis ci.
# https://travis-ci.org/libgd/libgd

# TODO: Add support for building/testing w/ASAN/etc... enabled.

. "${0%/*}"/lib.sh

build_autotools() {
	v --fold="autoreconf" autoreconf -fi

	v --fold="configure" ./configure \
		--prefix=/usr/local \
		--libdir=/usr/local/lib \
		--enable-werror \
		--with-fontconfig \
		--with-freetype \
		--with-jpeg \
		--with-png \
		--with-tiff \
		--with-webp \
		--with-xpm \
		--with-zlib
	m
	m check

	# TODO: Re-enable this once out-of-tree tests work.
	#m distcheck
	m install DESTDIR=$PWD/install-autotools

	# Clean things up for cmake.
	m distclean
}

build_cmake() {
	v cmake \
		-DBUILD_TEST=1 \
		-DCMAKE_INSTALL_PREFIX=/usr/local \
		-DCMAKE_INSTALL_LIBDIR=/usr/local/lib \
		-DENABLE_FONTCONFIG=1 \
		-DENABLE_FREETYPE=1 \
		-DENABLE_JPEG=1 \
		-DENABLE_PNG=1 \
		-DENABLE_TIFF=1 \
		-DENABLE_WEBP=1 \
		-DENABLE_XPM=1 \
		-DENABLE_ZLIB=1 \
		.
	m
	v ctest -j${ncpus}
	m install DESTDIR=$PWD/install-cmake
}

compare_builds() {
	# Make sure the two install the same set of files.
	# TODO: cmake is not yet on-par with autotools.
	# TODO: Should verify symbol export list in libgd too.
	diff -ur install-autotools install-cmake || true
}

build_osx() {
	v ./thumbs.sh make
	v ./thumbs.sh check
}

main() {
	if [[ ${TRAVIS_OS_NAME} == "osx" ]] ; then
		# TODO: Should convert this to autotools/cmake.
		build_osx
		return
	fi

	build_autotools
	build_cmake
	compare_builds
}
main "$@"
