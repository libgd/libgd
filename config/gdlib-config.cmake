#!/bin/sh
#
# Return information about the local GD library installation
#
# Modeled after pdflib-config

echo "gdlib-config: warning: this script is deprecated;" \
     "please use the pkg-config file instead." >&2

# installation directories
prefix=@CMAKE_INSTALL_PREFIX@
exec_prefix=@CMAKE_INSTALL_PREFIX@
libdir=@CMAKE_INSTALL_FULL_LIBDIR@
includedir=@CMAKE_INSTALL_FULL_INCLUDEDIR@
bindir=@CMAKE_INSTALL_FULL_BINDIR@

usage()
{
	cat <<EOF
Print information on GD library's version, configuration, and use.
Usage: gdlib-config [options]
Options:
	--libdir          # directory where GD library is installed
	--includedir      # directory where GD library headers are installed
	--version         # complete GD library version string
	--majorversion    # GD library major version number
	--minorversion    # GD library minor version number
	--revision        # GD library revision version number
	--ldflags         # options required for linking against GD library
	--libs            # libs required for linking against GD library
	--cflags          # options required for compiling GD library apps
	--includes        # same as --cflags
	--features        # lists features compiled into gd, separated by spaces.
                          # Currently (as of @GD_VERSION@) the optional features
                          # are GD_PNG, GD_JPEG, GD_XPM, GD_FREETYPE, and 
                          # GD_FONTCONFIG. When these features are reported by
                          # --features, it is safe to include calls to the
                          # related functions in your code.
	--all             # print a summary of all GD library configure options
EOF
	exit $1
}

if test $# -eq 0; then
	usage 1 1>&2
fi

while test $# -gt 0; do
    case "$1" in
    -*=*) optarg=`echo "$1" | sed 's/[-_a-zA-Z0-9]*=//'` ;;
    *) optarg= ;;
    esac

    case $1 in
    --libdir)
	echo $libdir
	;;
    --includedir)
	echo $includedir
	;;
    --version)
	echo @GD_VERSION@
	;;
    --majorversion)
	echo @GD_VERSION_MAJOR@
	;;
    --minorversion)
	echo @GD_VERSION_MINOR@
	;;
    --revision)
	echo @GD_VERSION_PATCH@
	;;
    --ldflags)
	echo -L$libdir
	;;
    --libs)
	echo -lgd
	;;
    --cflags|--includes)
	echo -I$includedir
	;;
    --features)
	echo @FEATURES@
	;;
    --all)
	echo "GD library  @GD_VERSION@"
	echo "includedir: $includedir"
	echo "cflags:     -I$includedir"
	echo "ldflags:    -L$libdir"
	echo "libs:       "
	echo "libdir:     $libdir"
	echo "features:   @FEATURES@"
	;;
    *)
	usage 1 1>&2
	;;
    esac
    shift
done
