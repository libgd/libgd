#!/bin/bash -e
# The main script for building/testing while under travis ci.
# https://travis-ci.org/libgd/libgd

# TODO: Add support for building/testing w/ASAN/etc... enabled.

. "${0%/*}"/lib.sh

# We have to do this by hand rather than use the coverity addon because of
# matrix explosion: https://github.com/travis-ci/travis-ci/issues/1975
# We also do it by hand because when we're throttled, the addon will exit
# the build immediately and skip the main script!
coverity_scan() {
	local reason
	[[ ${TRAVIS_JOB_NUMBER} != *.1 ]] && reason="not first build job"
	[[ -n ${TRAVIS_TAG} ]] && reason="git tag"
	[[ ${TRAVIS_PULL_REQUEST} == "true" ]] && reason="pull request"
	if [[ -n ${reason} ]] ; then
		echo "Skipping coverity scan due to: ${reason}"
		return
	fi

	export COVERITY_SCAN_PROJECT_NAME="${TRAVIS_REPO_SLUG}"
	export COVERITY_SCAN_NOTIFICATION_EMAIL="pierre.php@gmail.com"
	export COVERITY_SCAN_BUILD_COMMAND="make -j${ncpus}"
	export COVERITY_SCAN_BUILD_COMMAND_PREPEND="git clean -q -x -d -f; git checkout -f; ./bootstrap.sh && ./configure"
	export COVERITY_SCAN_BRANCH_PATTERN="GD-2.2"

	curl -s "https://scan.coverity.com/scripts/travisci_build_coverity_scan.sh" | bash || :
}

check_git_status() {
	local status

	# Make sure our gitignore files are up-to-date, and we aren't
	# forgetting to commit the few generated (e.g. cmake.in) files.
	# Note: We ignore config.h.cmake changes since it indirectly
	# depends on the format of the output of autoheader :/.
	status=$(git status --porcelain | grep -v '^ M src/config\.h\.cmake' | grep -v '^ M docs/naturaldocs/project/Menu\.txt') || :
	if [[ -n ${status} ]]; then
		echo "Missing git repo updates (gitignore/etc...):"
		echo "$ git status"
		echo "${status}"
		echo "$ git diff"
		git diff
		exit 1
	fi
}

build_autotools() {
	v --fold="bootstrap" ./bootstrap.sh

	v --fold="configure" ./configure \
		--prefix=/usr/local \
		--libdir=/usr/local/lib \
		--enable-werror \
		--enable-gd-formats \
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

	# Verify building a release works (also does things like read-only
	# out of tree builds for use).
	m distcheck VERBOSE=1

	# Clean things up for cmake.
	m distclean
}

cmake_args=(
	-DBUILD_SHARED_LIBS=1
	-DBUILD_STATIC_LIBS=1
	-DBUILD_TEST=1
	-DCMAKE_INSTALL_PREFIX=/usr/local
	-DCMAKE_INSTALL_LIBDIR=/usr/local/lib
	-DENABLE_GD_FORMATS=1
	-DENABLE_FONTCONFIG=1
	-DENABLE_FREETYPE=1
	-DENABLE_JPEG=1
	-DENABLE_PNG=1
	-DENABLE_TIFF=1
	-DENABLE_WEBP=1
)

# libxpm-dev is unavaible in brew repo
# Once it gets avaible, please modify this code block.
if [[ ${TRAVIS_OS_NAME} == "linux" ]]; then
	cmake_args+=("-DENABLE_XPM=1")
fi

build_cmake() {
	# First try building out of tree.
	mkdir build
	cd build
	v cmake "${cmake_args[@]}" ..
	m
	v ctest -j${ncpus}
	cd ..
	rm -rf build

	# Then build in-tree.
	v cmake "${cmake_args[@]}" .
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

source_tests() {
	# Run lint/source tests against the codebase.
	# Reset any files in case the build modified them.
	git checkout -f
	./tests/source/run.sh
}

build_codecov() {
	# Only genenrate code coverage report in Linux with gcc
	if [[ ${TRAVIS_OS_NAME} != "linux" || ${TRAVIS_COMPILER} != "gcc" ]]; then
		exit 0
	fi

	# Delete these two files so that we can build out of tree again
	rm -f CMakeCache.txt
	rm -rf CMakeFiles

	# Delete test run time limit. Or tests/gdimageline/gdimgaeline_bug5 will run timeout
	sed -i '/TIMEOUT/d' tests/CMakeLists.txt

	# Build out of tree
	mkdir build
	cd build
	export CFLAGS="-fprofile-arcs -ftest-coverage"
	v cmake "${cmake_args[@]}" ..
	m
	v ctest -j${ncpus}
	bash <(curl -s https://codecov.io/bash)
}

main() {
	build_autotools
	build_cmake
	compare_builds
	v --fold="coverity_scan" coverity_scan
	# Run the source tests last.
	v --fold="source_tests" source_tests
	build_codecov
}
main "$@"
