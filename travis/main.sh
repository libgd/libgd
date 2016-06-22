#!/bin/bash -e
# The main script for building/testing while under travis ci.
# https://travis-ci.org/libgd/libgd

. "${0%/*}"/lib.sh

update_os() {
	# Note: Linux deps are maintained in .travis.yml.
	case ${TRAVIS_OS_NAME} in
	osx)
		v --fold="brew_update" brew update
		# These packages are already installed in Travis, so omit them or brew fails.
		# autoconf automake libtool pkg-config cmake libpng jpeg libtiff
		v --fold="brew_install" brew install \
			gettext freetype fontconfig webp xz
		;;
	esac
}

check_git_status() {
	local status

	# Make sure our gitignore files are up-to-date, and we aren't
	# forgetting to commit the few generated (e.g. cmake.in) files.
	# Note: We ignore config.h.cmake changes since it indirectly
	# depends on the format of the output of autoheader :/.
	status=$(git status --porcelain | grep -v '^ M src/config.h.cmake') || :
	if [[ -n ${status} ]]; then
		echo "Missing git repo updates (gitignore/etc...):"
		echo "$ git status"
		echo "${status}"
		echo "$ git diff"
		git diff
		exit 1
	fi
}

set_default_compiler_settings() {
	: ${CFLAGS=-O2 -pipe}
	export CFLAGS
}

build_autotools() {
	v --fold="bootstrap" ./bootstrap.sh

	case ${TRAVIS_BUILD_TYPE} in
	sanitizers)
		set_default_compiler_settings
		SANITIZE_FLAGS=(
			-fsanitize=address
			-fsanitize=leak
			-fsanitize=alignment
			-fsanitize=bool
			-fsanitize=enum
			-fsanitize=float-cast-overflow
			-fsanitize=float-divide-by-zero
			-fsanitize=function
			-fsanitize=integer-divide-by-zero
			-fsanitize=null
			-fsanitize=object-size
			-fsanitize=return
			-fsanitize=signed-integer-overflow
			-fsanitize=unreachable
			-fsanitize=unsigned-integer-overflow)
		CFLAGS+=" ${SANITIZE_FLAGS[*]}"
		;;
	coverage)
		set_default_compiler_settings
		COVERAGE_FLAGS="-fprofile-arcs -ftest-coverage"
		CFLAGS+=" $COVERAGE_FLAGS"
		;;
	esac

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
	# Make sure failures are shown in the log.
	m check VERBOSE=1

	check_git_status

	# Coverage builds leave behind coverage information that is needed later
	# for upload, so skip cleanup.  The presence of the coverage files also
	# causes the distcheck target to fail, so skip that too.
	if [[ "${TRAVIS_BUILD_TYPE}" != "coverage" ]]; then
		# Verify building a release works (also does things like read-only
		# out of tree builds for use).
		m distcheck VERBOSE=1

		# Clean things up for cmake.
		m distclean
	fi
}

build_cmake() {
	local args=(
		-DBUILD_TEST=1
		-DCMAKE_INSTALL_PREFIX=/usr/local
		-DCMAKE_INSTALL_LIBDIR=/usr/local/lib
		-DENABLE_FONTCONFIG=1
		-DENABLE_FREETYPE=1
		-DENABLE_JPEG=1
		-DENABLE_PNG=1
		-DENABLE_TIFF=1
		-DENABLE_WEBP=1
		-DENABLE_XPM=1
		-DENABLE_ZLIB=1
	)

	# First try building out of tree.
	mkdir build
	cd build
	v cmake "${args[@]}" ..
	m
	v ctest -j${ncpus}
	cd ..
	rm -rf build

	# Then build in-tree.
	v cmake "${args[@]}" .
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

upload_coverage() {
	v --fold="upload_coverage" cpp-coveralls --exclude tests --gcov-options '\-lp'
}

main() {
	update_os
	build_autotools
	case ${TRAVIS_BUILD_TYPE} in
	"")
		build_cmake
		compare_builds
		;;
	coverage)
		upload_coverage
		;;
	esac
}
main "$@"
