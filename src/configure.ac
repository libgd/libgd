# Process this file with autoconf to produce a configure script.
# Configure template for gd library

#
AC_PREREQ(2.54)

AC_INIT([GD], [2.0.5], [gd@boutell.com])
AC_CONFIG_SRCDIR([gd.c])
AC_CONFIG_AUX_DIR(config)

GDLIB_MAJOR=2
GDLIB_MINOR=0
GDLIB_REVISION=5
GDLIBNAME=gd

AC_SUBST(GDLIB_MAJOR)
AC_SUBST(GDLIB_MINOR)
AC_SUBST(GDLIB_REVISION)
AC_SUBST(GDLIBNAME)

AM_INIT_AUTOMAKE()
AC_CONFIG_HEADERS([config.h:config.hin])

AC_PROG_CC
AM_PROG_CC_STDC
AC_PROG_INSTALL
AC_PROG_LIBTOOL
AC_PROG_LN_S
AC_PROG_MAKE_SET
AC_PROG_RANLIB

dnl may be required for freetype and Xpm
AC_PATH_X

if test "x$x_includes" != x; then
  CPPFLAGS="$CPPFLAGS -I$x_includes"
fi
if test "x$x_libraries" != x; then
  LDFLAGS="$LDFLAGS -L$x_libraries"
fi

AC_HEADER_STDC
AC_CHECK_HEADERS([errno.h iconv.h limits.h stddef.h stdlib.h string.h unistd.h])

# Checks for typedefs, structures, and compiler characteristics.
#AC_C_CONST
#AC_TYPE_SIZE_T

# Checks for library functions.
#AC_FUNC_ERROR_AT_LINE
#AC_FUNC_MALLOC
#AC_FUNC_REALLOC
#AC_FUNC_VPRINTF
#AC_CHECK_FUNCS([floor memset sqrt strchr strdup strtol])

dnl zlib is required
AC_CHECK_LIB(z,deflate,
  [LIBS="-lz -lm $LIBS"
   AC_DEFINE(HAVE_LIBZ, 1, [ Define if you have zlib. ])],
  [AC_MSG_WARN([zlib is required - see http://www.gzip.org/zlib/])],[-lm])

dnl libpng is required
AC_ARG_WITH(png,dnl
[  --with-png=DIR          where to find the png library],,
  withval=yes)

if test "$withval" != no; then
  AC_PATH_PROG([LIBPNG_CONFIG], [libpng-config])
  if test -n "$LIBPNG_CONFIG"; then
    libpng_CPPFLAGS=`libpng-config --cflags`
    # should be --ldopts, but it's currently broken
    libpng_LDFLAGS=`libpng-config --ldflags | sed 's/ *-l.*$//'`
  elif test -d "$withval"; then
    libpng_CPPFLAGS="-I$withval/include"
    libpng_LDFLAGS="-L$withval/lib"
  fi
  _cppflags="$CPPFLAGS"
  _ldflags="$LDFLAGS"
  CPPFLAGS="$CPPFLAGS $libpng_CPPFLAGS"
  LDFLAGS="$LDFLAGS $libpng_LDFLAGS"

  AC_CHECK_HEADERS(png.h,,
    [AC_CHECK_HEADERS(libpng/png.h,
       [CPPFLAGS="$CPPFLAGS $libpng_CPPFLAGS/libpng"],
       [CPPFLAGS="$_cppflags"])])

  AC_CHECK_LIB(png,png_create_read_struct,,
    [LDFLAGS="$_ldflags"  
     AC_MSG_WARN([libpng is required - see http://www.libpng.org/pub/png/])])
else
  ac_cv_lib_png_png_create_read_struct=no
  AC_MSG_WARN([libpng is required - see http://www.libpng.org/pub/png/])
fi

dnl FreeType configure tests snarfed from libwmf ..
AC_ARG_WITH(freetype,dnl
[  --with-freetype=DIR     where to find the freetype 2.x library],
  [if test "$withval" != no; then
     if test "$withval" != yes; then
       FREETYPE_DIR=$withval
     fi
   fi])

if test "$withval" != no; then

  if test -n "$FREETYPE_DIR"; then
    AC_PATH_PROG(FREETYPE_CONFIG,freetype-config,,[$FREETYPE_DIR/bin:$PATH])
  else
    AC_PATH_PROG(FREETYPE_CONFIG,freetype-config)
  fi

  if test -n "$FREETYPE_CONFIG"; then
    if test -n "$FREETYPE_DIR"; then
      libft_INCLUDES="`$FREETYPE_CONFIG --cflags` -I$FREETYPE_DIR/include"
      libft_LDFLAGS=`$FREETYPE_CONFIG --libs`
    else
      libft_INCLUDES=`$FREETYPE_CONFIG --cflags`
      libft_LDFLAGS=`$FREETYPE_CONFIG --libs`
    fi
  else
    if test -n "$FREETYPE_DIR"; then
      libft_INCLUDES="-I$FREETYPE_DIR/include/freetype2 -I$FREETYPE_DIR/include"
      libft_LDFLAGS="-L$FREETYPE_DIR/lib -lfreetype"
    else
      libft_INCLUDES=""
      libft_LDFLAGS="-lfreetype"
    fi
  fi

  _cppflags="$CPPFLAGS"
  _ldflags="$LDFLAGS"
  CPPFLAGS="$libft_INCLUDES $CPPFLAGS"
  LDFLAGS="$libft_LDFLAGS $LDFLAGS"

  AC_CHECK_LIB(freetype,FT_Init_FreeType,,
    [LDFLAGS="$_ldflags"
     AC_MSG_WARN([not found - disabling freetype support])])

  AC_CHECK_HEADERS(freetype/freetype.h,
    [AC_MSG_CHECKING([whether it's freetype 2])
     AC_EGREP_HEADER(FT_Library,freetype/freetype.h,
       [AC_MSG_RESULT([yes])
       ],
       [AC_MSG_RESULT([no])
        CPPFLAGS="$_cppflags"
       ])
    ])

else
  ac_cv_lib_freetype_FT_Init_FreeType=no
fi

dnl check for libjpeg by default
AC_ARG_WITH(jpeg,dnl
[  --with-jpeg=DIR         where to find the jpeg library],
  [if test -d "$withval"; then
     LDFLAGS="$LDFLAGS -L$withval/lib"
     CPPFLAGS="$CPPFLAGS -I$withval/include"
   fi],
  withval=yes)

if test "$withval" != no; then
  AC_CHECK_LIB(jpeg,jpeg_set_defaults)
else
  ac_cv_lib_jpeg_jpeg_set_defaults=no
fi

dnl check for libXpm by default
AC_ARG_WITH(xpm,dnl
[  --with-xpm=DIR          where to find the xpm library],
  [if test -d "$withval"; then
     LDFLAGS="$LDFLAGS -L$withval/lib"
     CPPFLAGS="$CPPFLAGS -I$withval/include"
   fi],
  withval=yes)

if test "$withval" != no; then
  AC_CHECK_LIB(Xpm,XpmReadFileToXpmImage,
    [LIBS="-lXpm -lX11 $LIBS"
     AC_DEFINE(HAVE_LIBXPM, 1, [ Define if you have the Xpm library. ])],,
    [-lX11])
else
  ac_cv_lib_Xpm_XpmReadFileToXpmImage=no
fi

dnl report configuration
AC_MSG_RESULT([
** Configuration summary for $PACKAGE $VERSION:

   Support for PNG library:          $ac_cv_lib_png_png_create_read_struct
   Support for JPEG library:         $ac_cv_lib_jpeg_jpeg_set_defaults
   Support for Freetype 2.x library: $ac_cv_lib_freetype_FT_Init_FreeType
   Support for Xpm library:          $ac_cv_lib_Xpm_XpmReadFileToXpmImage
])

AC_CONFIG_FILES([Makefile config/Makefile config/gdlib-config test/Makefile])

AC_OUTPUT
