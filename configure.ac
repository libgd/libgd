# Process this file with autoconf to produce a configure script.
# Configure template for gd library

AC_PREREQ(2.64)

# We extract version numbers from src/versions.h
define([gv],[perl config/getver.pl ]$1)

m4_define([gd_MAJOR],esyscmd(gv(MAJOR)))dnl
m4_define([gd_MINOR],esyscmd(gv(MINOR)))dnl
m4_define([gd_REVISION],esyscmd(gv(RELEASE)))dnl
m4_define([gd_EXTRA],esyscmd(gv(EXTRA)))dnl
m4_define([gd_PKG_VERSION],[gd_MAJOR.gd_MINOR.gd_REVISION]gd_EXTRA)]dnl

AC_INIT([GD], gd_PKG_VERSION, [https://github.com/libgd/libgd/issues], [libgd], [http://lib.gd])
AC_CONFIG_SRCDIR([src/gd.c])
AC_CONFIG_AUX_DIR(config)
AC_CONFIG_MACRO_DIR([m4])
AC_CANONICAL_HOST

# This is not used anywhere.  However, Makefile.netware searches
# through configure for these definitions to find the version numbers.
# (Assuming anyone still uses Netware, that should be changed to use
# gd.h via getver.pl instead.)
GDLIB_MAJOR=gd_MAJOR
GDLIB_MINOR=gd_MINOR
GDLIB_REVISION=gd_REVISION
GDLIB_EXTRA=gd_EXTRA
GDLIB_VERSION=gd_PKG_VERSION

AC_SUBST(GDLIB_MAJOR)
AC_SUBST(GDLIB_MINOR)
AC_SUBST(GDLIB_REVISION)
AC_SUBST(GDLIB_EXTRA)
AC_SUBST(GDLIB_VERSION)

# Dynamic library version information
# See http://www.gnu.org/software/libtool/manual/libtool.html#Updating-version-info

GDLIB_LT_CURRENT=3
dnl This is the version where the soname (current above) changes.  We use it
dnl to reset the revision base back to zero.  It's a bit of a pain, but some
dnl systems restrict the revision range below to [0..255] (like OS X).
GDLIB_PREV_MAJOR=2
GDLIB_PREV_MINOR=2
dnl This isn't 100% correct, but it tends to be a close enough approximation
dnl for how we manage the codebase.  It's rare to do a release that doesn't
dnl modify the library since this project is centered around the library.
GDLIB_LT_REVISION=$(( ((GDLIB_MAJOR - GDLIB_PREV_MAJOR) << 6) | ((GDLIB_MINOR - GDLIB_PREV_MINOR) << 3) | GDLIB_REVISION ))
GDLIB_LT_AGE=0
AC_SUBST(GDLIB_LT_CURRENT)
AC_SUBST(GDLIB_LT_REVISION)
AC_SUBST(GDLIB_LT_AGE)

#Expanded by tests later in this file. TBB 2.0.26
#2.0.28: GIF is standard now. Doesn't depend on anything else,
#so we always build it.
FEATURES="GD_GIF GD_GIFANIM GD_OPENPOLYGON"
AC_SUBST(FEATURES)

AM_INIT_AUTOMAKE([1.11 foreign dist-xz -Wall -Werror subdir-objects])
AC_CONFIG_HEADERS([src/config.h:src/config.hin])

AM_PROG_AR
AC_PROG_CC_STDC
AM_PROG_CC_C_O
AC_PROG_INSTALL
AC_PROG_LIBTOOL
AC_PROG_LN_S
AC_PROG_MAKE_SET
LT_INIT([win32-dll])
PKG_PROG_PKG_CONFIG

dnl may be required for freetype and Xpm
AC_PATH_X

if test -n "$x_includes" && test "x$x_includes" != xNONE ; then
  CFLAGS="$CFLAGS -I$x_includes"
fi
if test -n "$x_libraries" && test "x$x_libraries" != xNONE ; then
  LDFLAGS="$LDFLAGS -L$x_libraries"
fi

dnl Keep in sync with cmake/modules/AC_HEADER_STDC.cmake.
AC_HEADER_STDC
AC_CHECK_HEADERS_ONCE(m4_flatten([
	dirent.h
	errno.h
	inttypes.h
	limits.h
	stddef.h
	stdint.h
	stdlib.h
	string.h
	strings.h
	unistd.h
	sys/stat.h
	sys/types.h
]))

dnl if we're configuring on a system w/out gettext, don't fall over
m4_ifndef([AM_ICONV],[m4_define([AM_ICONV],[:])])
AM_ICONV
# if test -n "$LIBICONV" ; then
#   LIBS="$LIBS $LIBICONV"
# fi

AC_CHECK_HEADERS(iconv.h,
  [AC_MSG_CHECKING(whether iconv.h defines iconv_t)
   AC_EGREP_HEADER([typedef.*iconv_t],iconv.h,
     [AC_MSG_RESULT(yes)
      AC_DEFINE(HAVE_ICONV_T_DEF, 1,
                [Define if <iconv.h> defines iconv_t.])],
      AC_MSG_RESULT(no))])

# Checks for typedefs, structures, and compiler characteristics.
#AC_C_CONST
#AC_TYPE_SIZE_T

# Checks for library functions.
#AC_FUNC_ERROR_AT_LINE
#AC_FUNC_MALLOC
#AC_FUNC_REALLOC
#AC_FUNC_VPRINTF
#AC_CHECK_FUNCS([floor memset sqrt strchr strdup strtol])

dnl do we need to specify -lm explicitly?
AC_CHECK_FUNC(sin,,[AC_CHECK_LIB(m,sin)])

AX_PTHREAD()
AX_OPENMP()

dnl Helper macro for working with external libraries.
dnl GD_LIB_CHECK([SYM], [FEATURE], [name], [...test...])
dnl $1 - upper case symbol
dnl $2 - GD feature name to track
dnl $3 - lower case name for user
dnl $4 - test for the feature
dnl The test code should set gd_found_lib=yes to indicate success.
m4_define([GD_LIB_CHECK], [dnl
  dnl Export the flag for the user to leverage.  No other logic here.
  AC_MSG_CHECKING([whether to support $3])
  AC_ARG_WITH([$3],
    [AS_HELP_STRING([--with-$3@<:@=DIR@:>@], [Support $3 (optionally in DIR)])],
    [gd_with_lib=$withval],
    [gd_with_lib=auto])
  AC_MSG_RESULT([$gd_with_lib])

  gd_found_lib=no
  if test "$gd_with_lib" != "no"; then
    save_CPPFLAGS=$CPPFLAGS
    save_LDFLAGS=$LDFLAGS
    dnl Set up default libs/cflags vars based on the path if user gave us one.
    gd_lib_ldflags=
    gd_lib_cflags=
    case $gd_with_lib in
    yes|no|auto|"") ;;
    *)
      if test -d "$gd_with_lib/lib"; then
        gd_lib_ldflags="-L$gd_with_lib/lib"
        AS_VAR_APPEND([LDFLAGS], [" $gd_lib_ldflags"])
      fi
      if test -d "$gd_with_lib/include"; then
        gd_lib_cflags="-I$gd_with_lib/include"
        AS_VAR_APPEND([CPPFLAGS], [" $gd_lib_cflags"])
      fi
      ;;
    esac
    dnl Run the test for this feature.
    $4
    CPPFLAGS=$save_CPPFLAGS
    LDFLAGS=$save_LDFLAGS

    if test "$gd_found_lib" = "yes"; then
      AC_DEFINE([HAVE_$1], [1], [Define if you have $3])
      AS_VAR_APPEND([FEATURES], [" GD_$2"])
      dnl Merge the flags into the main LIBS/CPPFLAGS.
      $1][_LIBS="$gd_lib_ldflags $][$1][_LIBS"
      $1][_CFLAGS="$gd_lib_cflags $][$1][_CFLAGS"
      AS_VAR_APPEND([LIBS], [" $][$1][_LIBS"])
      AS_VAR_APPEND([CPPFLAGS], [" $][$1][_CFLAGS"])
    elif test "$gd_with_lib" = "yes"; then
      AC_MSG_ERROR([$3 requested but not found])
    else
      AC_MSG_NOTICE([Disabling support for $3])
    fi
  fi
  AM_CONDITIONAL([HAVE_$1], test "$gd_found_lib" = yes)
  gd_with_$1=$gd_found_lib
])

dnl Helper macro for working with external libraries with pkg-config support.
dnl GD_LIB_PKG_CHECK([SYM], [FEATURE], [name], [pkg-config module], [...fallback test...])
dnl $1 - upper case symbol
dnl $2 - GD feature name to track
dnl $3 - lower case name for user
dnl $4 - pkg-config module to look for
dnl $5 - fallback test for the feature
m4_define([GD_LIB_PKG_CHECK], [dnl
  GD_LIB_CHECK([$1], [$2], [$3], [dnl
    PKG_CHECK_MODULES([$1], [$4], [gd_found_lib=yes], [$5])
  ])
])

dnl Check for zlib support.
GD_LIB_PKG_CHECK([LIBZ], [ZLIB], [zlib], [zlib], [dnl
  AC_CHECK_LIB([z], [deflate], [dnl
    AS_VAR_APPEND([LIBZ_LIBS], [" -lz"])
    gd_found_lib=yes
  ])
])

dnl Check for libpng support.
GD_LIB_PKG_CHECK([LIBPNG], [PNG], [png], [libpng], [
  LIBPNG_CONFIG=$gd_with_lib/bin/libpng-config
  if test -e "$LIBPNG_CONFIG"; then
    LIBPNG_CFLAGS=`$LIBPNG_CONFIG --cflags`
    LIBPNG_LIBS=`$LIBPNG_CONFIG --ldflags`
    gd_found_lib=yes
  fi
])

dnl Check for FreeType support.
GD_LIB_PKG_CHECK([LIBFREETYPE], [FREETYPE], [freetype], [freetype2 >= 9.8.3], [
  FREETYPE_CONFIG=$gd_with_lib/bin/freetype-config
  if test -e "$FREETYPE_CONFIG"; then
    LIBFREETYPE_CFLAGS=`$FREETYPE_CONFIG --cflags`
    LIBFREETYPE_LIBS=`$FREETYPE_CONFIG --libs`
    gd_found_lib=yes
  fi
])
if test "$gd_with_LIBFREETYPE" = "yes"; then
  AC_DEFINE([HAVE_FT2BUILD_H], [1], [Define if you have the ft2build.h header.])
fi

dnl Check for fontconfig support.
GD_LIB_PKG_CHECK([LIBFONTCONFIG], [FONTCONFIG], [fontconfig], [fontconfig], [
  AC_CHECK_LIB([fontconfig], [FcInit], [dnl
    AS_VAR_APPEND([LIBFONTCONFIG_LIBS], [" -lfontconfig"])
    gd_found_lib=yes
  ])
])

dnl Check for jpeg support.
GD_LIB_CHECK([LIBJPEG], [JPEG], [jpeg], [
  AC_CHECK_LIB([jpeg], [jpeg_set_defaults], [dnl
    AS_VAR_APPEND([LIBJPEG_LIBS], [" -ljpeg"])
    gd_found_lib=yes
  ])
])

dnl Check for libimagequant support.
GD_LIB_CHECK([LIBIMAGEQUANT], [LIQ], [liq], [
  AC_CHECK_HEADER([libimagequant.h], [
    save_CFLAGS=$CFLAGS
    CFLAGS="$CFLAGS $OPENMP_CFLAGS"
    AC_CHECK_LIB([imagequant], [liq_attr_create_with_allocator], [dnl
      AS_VAR_APPEND([LIBIMAGEQUANT_LIBS], [" -limagequant $OPENMP_CFLAGS"])
      gd_found_lib=yes
    ])
    CFLAGS=$save_CFLAGS
  ])
])

dnl Check for xpm support.
GD_LIB_PKG_CHECK([LIBXPM], [XPM], [xpm], [xpm], [
  AC_CHECK_LIB([Xpm], [XpmReadFileToXpmImage], [dnl
    AS_VAR_APPEND([LIBXPM_LIBS], [" -lXpm"])
    gd_found_lib=yes
  ])
])

dnl Check for tiff support.
GD_LIB_PKG_CHECK([LIBTIFF], [TIFF], [tiff], [libtiff-4], [
  AC_CHECK_LIB([tiff], [TIFFClientOpen], [dnl
    AS_VAR_APPEND([LIBTIFF_LIBS], [" -ltiff"])
    gd_found_lib=yes
  ])
])

dnl Check for webp support.
GD_LIB_CHECK([LIBWEBP], [WEBP], [webp], [
  AC_CHECK_LIB([webp], [WebPGetInfo], [dnl
    AS_VAR_APPEND([LIBWEBP_LIBS], [" -lwebp"])
    gd_found_lib=yes
  ])
])

gl_VISIBILITY()
CFLAGS="$CFLAGS $CFLAG_VISIBILITY"

MINGW_AC_WIN32_NATIVE_HOST()

if test "$mingw_cv_win32_host" = yes; then
AC_DEFINE([BGDWIN32], [], [Define is you are building for Win32 API])
fi

dnl Enable -Wall if possible.  Do it after all other tests.
AX_CFLAGS_WARN_ALL

dnl Enable -Werror if possible.  Do it after all other tests.
AC_ARG_ENABLE([werror], [AS_HELP_STRING([--enable-werror], [treat compile warnings as errors])])
if test "$enable_werror" = "yes" || \
   test "$GCC" = "yes" -a "$enable_werror" != "no" -a -d "$srcdir/.git"; then
  CFLAGS="-Werror $CFLAGS"
  CXXFLAGS="-Werror $CXXFLAGS"
fi

dnl report configuration
AC_MSG_RESULT([
** Configuration summary for $PACKAGE $VERSION:

   Support for Zlib:                 $gd_with_LIBZ
   Support for PNG library:          $gd_with_LIBPNG
   Support for JPEG library:         $gd_with_LIBJPEG
   Support for WebP library:         $gd_with_LIBWEBP
   Support for TIFF library:         $gd_with_LIBTIFF
   Support for Freetype 2.x library: $gd_with_LIBFREETYPE
   Support for Fontconfig library:   $gd_with_LIBFONTCONFIG
   Support for Xpm library:          $gd_with_LIBXPM
   Support for liq library:          $gd_with_LIBIMAGEQUANT
   Support for pthreads:             $ax_pthread_ok
])

AC_CONFIG_FILES([Makefile
                src/Makefile
                tests/Makefile
                config/Makefile
                config/gdlib-config
                config/gdlib.pc])

AC_OUTPUT
