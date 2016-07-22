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

update_os() {
	# Note: Linux deps are maintained in .travis.yml.
	case ${TRAVIS_OS_NAME} in
	osx)
		v --fold="brew_update" brew update
		# We have to hack this package due to bugs in Travis CI.  See:
		# https://github.com/libgd/libgd/issues/266
		# https://github.com/Homebrew/legacy-homebrew/issues/43874
		v --fold="brew_clean" brew uninstall libtool
		# These packages are already installed in Travis, so omit them or brew fails.
		# autoconf automake pkg-config cmake libpng jpeg libtiff
		v --fold="brew_install" brew install \
			gettext freetype fontconfig libtool webp xz
		;;
	esac
}

build_autotools() {
	v --fold="bootstrap" ./bootstrap.sh

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

	# TODO: Re-enable this once out-of-tree tests work.
	m distcheck VERBOSE=1
	m install DESTDIR=$PWD/install-autotools

	# Clean things up for cmake.
	m distclean
}

build_cmake() {
	local args=(
		-DBUILD_SHARED_LIBS=1
		-DBUILD_STATIC_LIBS=1
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

main() {
	update_os
	build_autotools
	build_cmake
	compare_builds
	v --fold="coverity_scan" coverity_scan
}
main "$@"
