# Process this file with autoconf to produce a configure script.
# Configure template for gd library

#
AC_PREREQ(2.54)

#HEY! Change BOTH the version number and the GDLIB_REVISION setting!
AC_INIT([GD], [2.1.0], [https://bitbucket.org/libgd/gd-libgd/issues])
AC_CONFIG_SRCDIR([src/gd.c])
AC_CONFIG_AUX_DIR(config)
AC_CONFIG_MACRO_DIR([m4])
AC_CANONICAL_TARGET([])

GDLIB_MAJOR=2
GDLIB_MINOR=1
GDLIB_REVISION=0
GDLIBNAME=gd
AC_SUBST(GDLIB_MAJOR)
AC_SUBST(GDLIB_MINOR)
AC_SUBST(GDLIB_REVISION)
AC_SUBST(GDLIBNAME)

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

AM_INIT_AUTOMAKE([foreign -Wall -Werror])
AC_CONFIG_HEADERS([src/config.h:src/config.hin])

AC_MSG_CHECKING([if we are building a Cygwin target])
case "$target" in
  *-*-cygwin*) XTRA_LDFLAGS="-no-undefined"
               os_cygwin=yes
               ;;
           *)  XTRA_LDFLAGS=
               os_cygwin=no
               ;;
esac
AC_MSG_RESULT([$os_cygwin])
AC_SUBST([XTRA_LDFLAGS])

AC_PROG_CC
AM_PROG_CC_STDC
AC_PROG_INSTALL
AC_PROG_LIBTOOL
AC_PROG_LN_S
AC_PROG_MAKE_SET
LT_INIT

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
AC_CHECK_LIB(z,deflate,
  [LIBS="-lz $LIBS"
   AC_DEFINE(HAVE_LIBZ, 1, [ Define if you have zlib. ])],
  [AC_MSG_WARN([zlib is required - see http://www.gzip.org/zlib/])])

dnl libpng is required
AC_ARG_WITH(png,dnl
[  --with-png=DIR          where to find the png library],,
  withval=yes)

# libpng12.so is a nightmare. We have to test separately for
# libpng12-config and libpng-config. If we find libpng12-config,
# we don't look for the other, which will just be a symlink to it,
# confusing autoconf into thinking the library itself will be named -lpng,
# which it won't be. Awful. This code will suffice unless the libpng
# authors decide to do this AGAIN. Which I really hope they won't. TBB

if test "$withval" != no; then
  if test -d "$withval"; then
    if test -e "$withval/bin/libpng12-config"; then
      LIBPNG12_CONFIG="$withval/bin/libpng12-config"
    fi
    if test -e "$withval/bin/libpng-config"; then
      LIBPNG_CONFIG="$withval/bin/libpng-config"
    fi
  else
      AC_PATH_PROG([LIBPNG12_CONFIG], [libpng12-config])
      AC_PATH_PROG([LIBPNG_CONFIG], [libpng-config])
  fi

  if test -n "$LIBPNG12_CONFIG"; then
    libpng_CFLAGS=`$LIBPNG12_CONFIG --cflags`
    # should be --ldopts, but it's currently broken
    libpng_LDFLAGS=`$LIBPNG12_CONFIG --ldflags`
    libpng_LDFLAGS=`echo " $libpng_LDFLAGS" | sed 's/ -l[[^ ]][[^ ]]*//g'`
  elif test -n "$LIBPNG_CONFIG"; then
    libpng_CFLAGS=`$LIBPNG_CONFIG --cflags`
    # should be --ldopts, but it's currently broken
    libpng_LDFLAGS=`$LIBPNG_CONFIG --ldflags`
    libpng_LDFLAGS=`echo " $libpng_LDFLAGS" | sed 's/ -l[[^ ]][[^ ]]*//g'`
  fi

  AC_CHECK_HEADERS(png.h,,
    [AC_CHECK_HEADERS(libpng/png.h,
       [CFLAGS="$CFLAGS $libpng_CFLAGS/libpng"],
       [CFLAGS="$_cppflags"])])

  _ldflags="$LDFLAGS"
  LDFLAGS="$libpng_LDFLAGS $LDFLAGS"

  #TBB: png12's png_create_read_struct is just as good 
  if test -n "$LIBPNG12_CONFIG"; then
    AC_CHECK_LIB(png12,png_create_read_struct,
      [LIBS="-lpng12 $LIBS"
       FEATURES="GD_PNG $FEATURES"
       AC_DEFINE(HAVE_LIBPNG, 1, [ Define if you have the png library. ])])
  fi
  # could be reached here in some case like the FreeBSD port of libpng;
  # libpng12-config but '-lpng' (not '-lpng12').
  #2.0.23: should be = not ==  
  if test "$ac_cv_lib_png12_png_create_read_struct" = yes; then
    ac_cv_lib_png_png_create_read_struct=yes
  else
    AC_CHECK_LIB(png,png_create_read_struct,
      [LIBS="-lpng $LIBS" 
       FEATURES="GD_PNG $FEATURES"
       AC_DEFINE(HAVE_LIBPNG, 1, [ Define if you have the png library. ])],
      [LDFLAGS="$_ldflags"])
  fi
else
  ac_cv_lib_png_png_create_read_struct=no
fi

if test "$ac_cv_lib_png_png_create_read_struct" != yes; then
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
    else
      libft_INCLUDES=`$FREETYPE_CONFIG --cflags`
    fi
    libft_LDFLAGS=`$FREETYPE_CONFIG --libs`
    libft_LDFLAGS=`echo " $libft_LDFLAGS" |sed 's/ -l[[^ ]][[^ ]]*//g'`
  else
    if test -n "$FREETYPE_DIR"; then
      libft_INCLUDES="-I$FREETYPE_DIR/include/freetype2 -I$FREETYPE_DIR/include"
      libft_LDFLAGS="-L$FREETYPE_DIR/lib"
    else
      libft_INCLUDES=""
      libft_LDFLAGS=""
    fi
  fi

  _cppflags="$CFLAGS"
  _ldflags="$LDFLAGS"
  CFLAGS="$libft_INCLUDES $CFLAGS"
  LDFLAGS="$libft_LDFLAGS $LDFLAGS"
  SAVE_FEATURES=$FEATURES
  AC_CHECK_LIB(freetype,FT_Init_FreeType,
    [LIBS="-lfreetype $LIBS" 
     FEATURES="GD_FREETYPE $FEATURES"
     AC_DEFINE(HAVE_LIBFREETYPE, 1, [ Define if you have the freetype library. ])],
    [LDFLAGS="$_ldflags"
     AC_MSG_WARN([not found - disabling freetype support])
     FEATURES=$SAVE_FEATURES])

  #TBB: 2.0.26: newer gd versions require ft2build.h
  AC_CHECK_HEADERS(ft2build.h,,
      [AC_MSG_RESULT([no])
      FEATURES=$SAVE_FEATURES
      ac_cv_lib_freetype_FT_Init_FreeType=no
      CFLAGS="$_cppflags"])

else
  ac_cv_lib_freetype_FT_Init_FreeType=no
fi

dnl check for libfontconfig by default
AC_ARG_WITH(fontconfig,dnl
[  --with-fontconfig=DIR         where to find the fontconfig library],
  [if test -d "$withval"; then
     LDFLAGS="$LDFLAGS -L$withval/lib"
     CFLAGS="$CFLAGS -I$withval/include"
   fi],
  withval=yes)

if test "$withval" != no; then
  AC_CHECK_LIB(fontconfig,FcInit,
    [LIBS="-lfontconfig $LIBS" 
     FEATURES="GD_FONTCONFIG $FEATURES"
     AC_DEFINE(HAVE_LIBFONTCONFIG, 1, [ Define if you have the fontconfig library. ])])
else
  ac_cv_lib_fontconfig_FcInit=no
fi

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

dnl check for libXpm by default
AC_ARG_WITH(xpm,dnl
[  --with-xpm=DIR          where to find the xpm library],
  [if test -d "$withval"; then
     LDFLAGS="$LDFLAGS -L$withval/lib"
     CFLAGS="$CFLAGS -I$withval/include"
   fi],
  withval=yes)

if test "$withval" != no; then
  AC_CHECK_LIB(Xpm,XpmReadFileToXpmImage,
    [LIBS="-lXpm $LIBS"
     FEATURES="GD_XPM $FEATURES"
     AC_DEFINE(HAVE_LIBXPM, 1, [ Define if you have the Xpm library. ])])
else
  ac_cv_lib_Xpm_XpmReadFileToXpmImage=no
fi



dnl check for libvpx by default
AC_ARG_WITH(vpx,dnl
[  --with-vpx=DIR         where to find the vpx library],
  [if test -d "$withval"; then
     LDFLAGS="$LDFLAGS -L$withval/lib"
     CFLAGS="$CFLAGS -I$withval/include"
   fi],
  withval=yes)

if test "$withval" != no; then
  AC_CHECK_LIB(vpx,vpx_codec_destroy,
    [LIBS="-lvpx $LIBS" 
     FEATURES="GD_VPX $FEATURES"
     AC_DEFINE(HAVE_LIBVPX, 1, [ Define if you have the vpx library. ])])
else
  ac_cv_lib_vpx_vpx_codec_destroy=no
fi


dnl check for libvpx by default
AC_ARG_WITH(tiff,dnl
[  --with-tiff=DIR         where to find the tiff library],
  [if test -d "$withval"; then
     LDFLAGS="$LDFLAGS -L$withval/lib"
     CFLAGS="$CFLAGS -I$withval/include"
   fi],
  withval=yes)

if test "$withval" != no; then
  AC_CHECK_LIB(tiff,TIFFClientOpen,
    [LIBS="-ltiff $LIBS" 
     FEATURES="GD_VPX $FEATURES"
     AC_DEFINE(HAVE_LIBTIFF, 1, [ Define if you have the vpx library. ])])
else
  ac_cv_lib_tiff_TIFFClientOpen=no
fi

gl_VISIBILITY()
CFLAGS="$CFLAGS $CFLAG_VISIBILITY"

AX_PTHREAD()

dnl report configuration
AC_MSG_RESULT([
** Configuration summary for $PACKAGE $VERSION:

   Support for PNG library:          $ac_cv_lib_png_png_create_read_struct
   Support for JPEG library:         $ac_cv_lib_jpeg_jpeg_set_defaults
   Support for VPX library:          $ac_cv_lib_vpx_vpx_codec_destroy
   Support for TIFF library:         $ac_cv_lib_tiff_TIFFClientOpen
   Support for Freetype 2.x library: $ac_cv_lib_freetype_FT_Init_FreeType
   Support for Fontconfig library:   $ac_cv_lib_fontconfig_FcInit
   Support for Xpm library:          $ac_cv_lib_Xpm_XpmReadFileToXpmImage
   Support for pthreads:             $ax_pthread_ok
])

AC_CONFIG_FILES([Makefile
                src/Makefile
                config/Makefile
                config/gdlib-config])

AC_OUTPUT
