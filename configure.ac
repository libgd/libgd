# Process this file with autoconf to produce a configure script.
# Configure template for gd library

AC_PREREQ(2.54)

# Some m4 magic to have correct version number everywhere
m4_define([gd_MAJOR],[2])dnl
m4_define([gd_MINOR],[1])dnl
m4_define([gd_REVISION],[0])dnl
dnl m4_define([gd_EXTRA],[rc2])

m4_ifdef([gd_EXTRA],
  [m4_define([gd_PKG_VERSION],[gd_MAJOR.gd_MINOR.gd_REVISION-gd_EXTRA])],
  [m4_define([gd_PKG_VERSION],[gd_MAJOR.gd_MINOR.gd_REVISION])]
)dnl

AC_INIT([GD], gd_PKG_VERSION, [https://bitbucket.org/libgd/gd-libgd/issues], [libgd], [http://lib.gd])
AC_CONFIG_SRCDIR([src/gd.c])
AC_CONFIG_AUX_DIR(config)
AC_CONFIG_MACRO_DIR([m4])
AC_CANONICAL_HOST

# This is not used anywhere
GDLIB_MAJOR=gd_MAJOR
GDLIB_MINOR=gd_MINOR
GDLIB_REVISION=gd_REVISION
GDLIB_EXTRA=gd_EXTRA
AC_SUBST(GDLIB_MAJOR)
AC_SUBST(GDLIB_MINOR)
AC_SUBST(GDLIB_REVISION)
AC_SUBST(GDLIB_EXTRA)

# Dynamic library version information
# See http://www.gnu.org/software/libtool/manual/libtool.html#Updating-version-info
GDLIB_CURRENT=3
GDLIB_REVISION=0
GDLIB_AGE=0
AC_SUBST(GDLIB_CURRENT)
AC_SUBST(GDLIB_REVISION)
AC_SUBST(GDLIB_AGE)

#Expanded by tests later in this file. TBB 2.0.26
#2.0.28: GIF is standard now. Doesn't depend on anything else,
#so we always build it.
FEATURES="GD_GIF GD_GIFANIM GD_OPENPOLYGON"
AC_SUBST(FEATURES)

AM_INIT_AUTOMAKE([foreign dist-bzip2 dist-xz -Wall -Werror])
AC_CONFIG_HEADERS([src/config.h:src/config.hin])

dnl newer automake wants this, but we still want to work with older
m4_ifndef([AM_PROG_AR],[m4_define([AM_PROG_AR],[:])])
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

AC_HEADER_STDC
AC_CHECK_HEADERS([errno.h limits.h stddef.h stdlib.h string.h unistd.h])

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

dnl zlib is required
AC_ARG_WITH(zlib,
[  --with-zlib=DIR         where to find the zlib library])
if test "$with_zlib" != no; then
  AC_CHECK_LIB(z,deflate,
    [LIBS="-lz $LIBS"
     AC_DEFINE(HAVE_LIBZ, 1, [Define if you have zlib.])
     with_zlib=yes],
    [AC_MSG_WARN([zlib is required - see http://www.gzip.org/zlib/])
     with_zlib=no])
fi
AM_CONDITIONAL([HAVE_LIBZ], test "$with_zlib" = yes)

dnl libpng is required
AC_ARG_WITH(png,dnl
[  --with-png=DIR          where to find the png library])

case $with_png in
no) ;;
yes|"")
  PKG_CHECK_MODULES([LIBPNG], libpng, [
    with_png=yes
  ],[
    if test "$with_png" = yes; then
      AC_MSG_ERROR([png support requested, but not found - see http://www.libpng.org/pub/png/])
    fi
    with_png=no
  ])
  ;;
*)
  AC_MSG_CHECKING([libpng-config script])
  LIBPNG_CONFIG=$with_png/bin/libpng-config
  if test -e "$LIBPNG_CONFIG"; then
    LIBPNG_CFLAGS=`$LIBPNG_CONFIG --cflags`
    LIBPNG_LIBS=`$LIBPNG_CONFIG --ldflags`
    with_png=yes
    AC_MSG_RESULT([$LIBPNG_CONFIG, cflags: $LIBPNG_CFLAGS, libs: $LIBPNG_LIBS])
  else
    AC_MSG_ERROR([png support requested, but not found at requested location: $LIBPNG_CONFIG])
  fi
  ;;
esac
if test "$with_png" != no; then
  CPPFLAGS="$CPPFLAGS $LIBPNG_CFLAGS"
  LIBS="$LIBS $LIBPNG_LIBS"
  FEATURES="GD_PNG $FEATURES"
  AC_DEFINE(HAVE_LIBPNG, 1, [Define if you have the png library.])
fi
AM_CONDITIONAL([HAVE_LIBPNG], test "$with_png" = yes)

dnl FreeType configure tests snarfed from libwmf ..
AC_ARG_WITH(freetype,dnl
[  --with-freetype=DIR     where to find the freetype 2.x library]])

case $with_freetype in
no) ;;
yes|"")
  dnl All freetype2 versions are numerically much larger than the actual
  dnl version number you see in the tarball.  This is because the number
  dnl corresponds to the shared library version rather than the release.
  dnl 9.8.3 is the version that freetype2-2.1.10 used.
  PKG_CHECK_MODULES([LIBFREETYPE], [freetype2 >= 9.8.3], [
    with_freetype=yes
  ],[
    if test "$with_freetype" = yes; then
      AC_MSG_ERROR([freetype support requested, but not found])
    fi
    with_freetype=no
  ])
  ;;
*)
  FREETYPE_CONFIG=$with_freetype/bin/freetype-config
  if test -e "$FREETYPE_CONFIG"; then
    LIBFREETYPE_CFLAGS=`$FREETYPE_CONFIG --cflags`
    LIBFREETYPE_LIBS=`$FREETYPE_CONFIG --libs`
    with_freetype=yes
  else
    AC_MSG_ERROR([freetype support requested, but not found: $FREETYPE_CONFIG])
  fi
  ;;
esac
if test "$with_freetype" = yes; then
  CPPFLAGS="$CPPFLAGS $LIBFREETYPE_CFLAGS"
  LIBS="$LIBS $LIBFREETYPE_LIBS"
  FEATURES="GD_FREETYPE $FEATURES"
  AC_DEFINE(HAVE_LIBFREETYPE, 1, [Define if you have the freetype library.])
  AC_DEFINE(HAVE_FT2BUILD_H, 1, [Define if you have the ft2build.h header.])
fi
AM_CONDITIONAL([HAVE_LIBFREETYPE], test "$with_freetype" = yes)

dnl check for libfontconfig by default
AC_ARG_WITH(fontconfig,dnl
[  --with-fontconfig=DIR         where to find the fontconfig library])

case $with_fontconfig in
no) ;;
yes|"")
  PKG_CHECK_MODULES([LIBFONTCONFIG], fontconfig, [with_fontconfig=yes],
  [
    if test "$with_fontconfig" = yes; then
      AC_MSG_ERROR([fontconfig support requested, but not found])
    fi
    with_fontconfig=no
  ])
  ;;
*)
  save_LIBS="$LIBS"
  save_CPPFLAGS="$CPPFLAGS"

  if test -d "$with_fontconfig"; then
    LIBFONTCONFIG_CFLAGS="-I$with_fontconfig/include"
    LIBFONTCONFIG_LIBS="-L$with_fontconfig/lib"
  fi

  CPPFLAGS="$CPPFLAGS $LIBFONTCONFIG_CFLAGS"
  LIBS="$LIBS $LIBFONTCONFIG_LIBS"

  AC_CHECK_LIB(fontconfig, FcInit,
    [
      LIBFONTCONFIG_LIBS="$LIBFONTCONFIG_LIBS -lfontconfig"
      with_fontconfig=yes
    ],[
      if test "$with_fontconfig" != ""; then
        AC_MSG_ERROR([fontconfig support requested, but not found])
      else
        with_fontconfig=no
      fi
    ])

  CPPFLAGS="$save_CPPFLAGS"
  LIBS="$save_LIBS"
  ;;
esac

if test "$with_fontconfig" != no; then
  CPPFLAGS="$CPPFLAGS $LIBFONTCONFIG_CFLAGS"
  LIBS="$LIBS $LIBFONTCONFIG_LIBS -lfontconfig"
  FEATURES="GD_FONTCONFIG $FEATURES"
  AC_DEFINE(HAVE_LIBFONTCONFIG, 1, [ Define if you have the fontconfig library. ])
fi
AM_CONDITIONAL([HAVE_LIBFONTCONFIG], test "$with_fontconfig" = yes)

dnl check for libjpeg by default
AC_ARG_WITH(jpeg,dnl
[  --with-jpeg=DIR         where to find the jpeg library],
  [if test -d "$withval"; then
     LDFLAGS="$LDFLAGS -L$withval/lib"
     CFLAGS="$CFLAGS -I$withval/include"
   fi],
  withval=yes)

if test "$withval" != no; then
  AC_CHECK_LIB(jpeg,jpeg_set_defaults,
    [LIBS="-ljpeg $LIBS" 
     FEATURES="GD_JPEG $FEATURES"
     AC_DEFINE(HAVE_LIBJPEG, 1, [ Define if you have the jpeg library. ])])
else
  ac_cv_lib_jpeg_jpeg_set_defaults=no
fi
AM_CONDITIONAL([HAVE_LIBJPEG], test "$ac_cv_lib_jpeg_jpeg_set_defaults" = yes)

dnl check for libXpm by default
AC_ARG_WITH(xpm,dnl
[  --with-xpm=DIR          where to find the xpm library])

case $with_xpm in
no) ;;
yes|"")
  PKG_CHECK_MODULES([LIBXPM], xpm, [with_xpm=yes],
  [
    if test "$with_xpm" = yes; then
      AC_MSG_ERROR([Xpm support requested, but not found])
    fi
    with_xpm=no
  ])
  ;;
*)
  save_LIBS="$LIBS"
  save_CPPFLAGS="$CPPFLAGS"

  if test -d "$with_xpm"; then
    LIBXPM_CFLAGS="-I$with_xpm/include"
    LIBXPM_LIBS="-L$with_xpm/lib -lXpm"
  fi

  CPPFLAGS="$CPPFLAGS $LIBXPM_CFLAGS"
  LIBS="$LIBS $LIBXPM_LIBS"

  AC_CHECK_LIB(Xpm,XpmReadFileToXpmImage,
    [
      if test -z "$LIBXPM_LIBS"; then
        LIBXPM_LIBS="-lXpm"
      fi
      with_xpm=yes
    ],[
      if test "$with_xpm" != ""; then
        AC_MSG_ERROR([Xpm support requested, but not found])
      else
        with_xpm=no
      fi
    ])

  CPPFLAGS="$save_CPPFLAGS"
  LIBS="$save_LIBS"
  ;;
esac

if test "$with_xpm" != no; then
  CPPFLAGS="$CPPFLAGS $LIBXPM_CFLAGS"
  LIBS="$LIBS $LIBXPM_LIBS"
  FEATURES="GD_XPM $FEATURES"
  AC_DEFINE(HAVE_LIBXPM, 1, [ Define if you have the Xpm library. ])
fi
AM_CONDITIONAL([HAVE_LIBXPM], test "$with_xpm" = yes)

dnl check for libvpx by default
AC_ARG_WITH(vpx,dnl
[  --with-vpx=DIR         where to find the vpx library])

case $with_vpx in
no) ;;
yes|"")
  PKG_CHECK_MODULES([LIBVPX], vpx, [with_vpx=yes],
  [
    PKG_CHECK_MODULES([LIBVPX], libvpx, [with_vpx=yes],
    [
      if test "$with_vpx" = yes; then
        AC_MSG_ERROR([VPX support requested, but not found])
      fi
      with_vpx=no
    ])
  ])
  ;;
*)
  save_LIBS="$LIBS"
  save_CPPFLAGS="$CPPFLAGS"

  if test -d "$with_vpx"; then
    LIBVPX_CFLAGS="-I$with_vpx/include"
    LIBVPX_LIBS="-L$with_vpx/lib -lvpx"
  fi

  CPPFLAGS="$CPPFLAGS $LIBVPX_CFLAGS"
  LIBS="$LIBS $LIBVPX_LIBS"

  AC_CHECK_LIB(vpx,vpx_codec_destroy,
    [
      if test -z "$LIBVPX_LIBS"; then
        LIBVPX_LIBS="-lvpx"
      fi
      with_vpx=yes
    ],[
      if test "$with_vpx" != ""; then
        AC_MSG_ERROR([vpx support requested, but not found])
      else
        with_vpx=no
      fi
    ])

  CPPFLAGS="$save_CPPFLAGS"
  LIBS="$save_LIBS"
  ;;
esac

if test "$with_vpx" != no; then
  CPPFLAGS="$CPPFLAGS $LIBVPX_CFLAGS"
  LIBS="$LIBS $LIBVPX_LIBS"
  FEATURES="GD_VPX $FEATURES"
  AC_DEFINE(HAVE_LIBVPX, 1, [ Define if you have the VPX library. ])
fi
AM_CONDITIONAL([HAVE_LIBVPX], test "$with_vpx" = yes)

dnl check for libtiff by default
AC_ARG_WITH(tiff,dnl
[  --with-tiff=DIR         where to find the TIFF library])

case $with_tiff in
no) ;;
yes|"")
  PKG_CHECK_MODULES([LIBTIFF], libtiff-4, [with_tiff=yes],
  [
    if test "$with_tiff" = yes; then
      AC_MSG_ERROR([TIFF support requested, but not found])
    fi
    with_tiff=no
  ])
  ;;
*)
  save_CPPFLAGS="$CPPFLAGS"
  save_LIBS="$LIBS"

  if test -d "$with_tiff"; then
    LIBTIFF_CFLAGS="-I$with_tiff/include"
    LIBTIFF_LIBS="-L$with_tiff/lib"
  fi

  CPPFLAGS="$CPPFLAGS $LIBTIFF_CFLAGS"
  LIBS="$LIBS $LIBTIFF_LIBS"

  AC_CHECK_LIB(tiff,TIFFClientOpen,
    [
      LIBTIFF_LIBS="$LIBTIFF_LIBS -ltiff"
      with_tiff=yes
    ],[
      if test "$with_tiff" != ""; then
        AC_MSG_ERROR([tiff support requested, but not found])
      else
        with_tiff=no
      fi
    ])

  CPPFLAGS="$save_CPPFLAGS"
  LIBS="$save_LIBS"
  ;;
esac

if test "$with_tiff" != no; then
  CPPFLAGS="$CPPFLAGS $LIBTIFF_CFLAGS"
  LIBS="$LIBS $LIBTIFF_LIBS"
  FEATURES="GD_TIFF $FEATURES"
  AC_DEFINE(HAVE_LIBTIFF, 1, [ Define if you have the Tiff library. ])
fi
AM_CONDITIONAL([HAVE_LIBTIFF], test "$with_tiff" = yes)

gl_VISIBILITY()
CFLAGS="$CFLAGS $CFLAG_VISIBILITY"

AX_PTHREAD()

MINGW_AC_WIN32_NATIVE_HOST()

if test "$mingw_cv_win32_host" = yes; then
AC_DEFINE([BGDWIN32], [], [Define is you are building for Win32 API])
fi

dnl report configuration
AC_MSG_RESULT([
** Configuration summary for $PACKAGE $VERSION:

   Support for Zlib:                 $with_zlib
   Support for PNG library:          $with_png
   Support for JPEG library:         $ac_cv_lib_jpeg_jpeg_set_defaults
   Support for VPX library:          $with_vpx
   Support for TIFF library:         $with_tiff
   Support for Freetype 2.x library: $with_freetype
   Support for Fontconfig library:   $with_fontconfig
   Support for Xpm library:          $with_xpm
   Support for pthreads:             $ax_pthread_ok
])

AC_CONFIG_FILES([Makefile
                src/Makefile
		tests/Makefile
		tests/test_config.h
                config/Makefile
                config/gdlib-config
		config/gdlib.pc])

AC_OUTPUT
