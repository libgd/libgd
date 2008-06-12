# Process this file with autoconf to produce a configure script.
# Configure template for gd library

#
AC_PREREQ(2.54)

#HEY! Change BOTH the version number and the GDLIB_REVISION setting!
AC_INIT([GD], [2.1.0], [http://bugs.libgd.org])
AC_CONFIG_SRCDIR([gd.c])
AC_CONFIG_AUX_DIR(config)
AC_CANONICAL_TARGET([])

GDLIB_MAJOR=2
GDLIB_MINOR=1
GDLIB_REVISION=0
GDLIBNAME=gd
#Expanded by tests later in this file. TBB 2.0.26
#2.0.28: GIF is standard now. Doesn't depend on anything else,
#so we always build it.
FEATURES="GD_GIF GD_GIFANIM GD_OPENPOLYGON"
AC_SUBST(FEATURES)

AC_SUBST(GDLIB_MAJOR)
AC_SUBST(GDLIB_MINOR)
AC_SUBST(GDLIB_REVISION)
AC_SUBST(GDLIBNAME)

AM_INIT_AUTOMAKE()
AC_CONFIG_HEADERS([config.h:config.hin])

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
#AC_PROG_RANLIB

dnl may be required for freetype and Xpm
AC_PATH_X

if test -n "$x_includes" && test "x$x_includes" != xNONE ; then
  CPPFLAGS="$CPPFLAGS -I$x_includes"
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

##### http://autoconf-archive.cryp.to/acx_pthread.html
#
# SYNOPSIS
#
#   ACX_PTHREAD([ACTION-IF-FOUND[, ACTION-IF-NOT-FOUND]])
#
# DESCRIPTION
#
#   This macro figures out how to build C programs using POSIX threads.
#   It sets the PTHREAD_LIBS output variable to the threads library and
#   linker flags, and the PTHREAD_CFLAGS output variable to any special
#   C compiler flags that are needed. (The user can also force certain
#   compiler flags/libs to be tested by setting these environment
#   variables.)
#
#   Also sets PTHREAD_CC to any special C compiler that is needed for
#   multi-threaded programs (defaults to the value of CC otherwise).
#   (This is necessary on AIX to use the special cc_r compiler alias.)
#
#   NOTE: You are assumed to not only compile your program with these
#   flags, but also link it with them as well. e.g. you should link
#   with $PTHREAD_CC $CFLAGS $PTHREAD_CFLAGS $LDFLAGS ... $PTHREAD_LIBS
#   $LIBS
#
#   If you are only building threads programs, you may wish to use
#   these variables in your default LIBS, CFLAGS, and CC:
#
#          LIBS="$PTHREAD_LIBS $LIBS"
#          CFLAGS="$CFLAGS $PTHREAD_CFLAGS"
#          CC="$PTHREAD_CC"
#
#   In addition, if the PTHREAD_CREATE_JOINABLE thread-attribute
#   constant has a nonstandard name, defines PTHREAD_CREATE_JOINABLE to
#   that name (e.g. PTHREAD_CREATE_UNDETACHED on AIX).
#
#   ACTION-IF-FOUND is a list of shell commands to run if a threads
#   library is found, and ACTION-IF-NOT-FOUND is a list of commands to
#   run it if it is not found. If ACTION-IF-FOUND is not specified, the
#   default action will define HAVE_PTHREAD.
#
#   Please let the authors know if this macro fails on any platform, or
#   if you have any other suggestions or comments. This macro was based
#   on work by SGJ on autoconf scripts for FFTW (http://www.fftw.org/)
#   (with help from M. Frigo), as well as ac_pthread and hb_pthread
#   macros posted by Alejandro Forero Cuervo to the autoconf macro
#   repository. We are also grateful for the helpful feedback of
#   numerous users.
#
# LAST MODIFICATION
#
#   2007-07-29
#
# COPYLEFT
#
#   Copyright (c) 2007 Steven G. Johnson <stevenj@alum.mit.edu>
#
#   This program is free software: you can redistribute it and/or
#   modify it under the terms of the GNU General Public License as
#   published by the Free Software Foundation, either version 3 of the
#   License, or (at your option) any later version.
#
#   This program is distributed in the hope that it will be useful, but
#   WITHOUT ANY WARRANTY; without even the implied warranty of
#   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
#   General Public License for more details.
#
#   You should have received a copy of the GNU General Public License
#   along with this program. If not, see
#   <http://www.gnu.org/licenses/>.
#
#   As a special exception, the respective Autoconf Macro's copyright
#   owner gives unlimited permission to copy, distribute and modify the
#   configure scripts that are the output of Autoconf when processing
#   the Macro. You need not follow the terms of the GNU General Public
#   License when using or distributing such scripts, even though
#   portions of the text of the Macro appear in them. The GNU General
#   Public License (GPL) does govern all other use of the material that
#   constitutes the Autoconf Macro.
#
#   This special exception to the GPL applies to versions of the
#   Autoconf Macro released by the Autoconf Macro Archive. When you
#   make and distribute a modified version of the Autoconf Macro, you
#   may extend this special exception to the GPL to apply to your
#   modified version as well.

AC_DEFUN([ACX_PTHREAD], [
AC_REQUIRE([AC_CANONICAL_HOST])
AC_LANG_SAVE
AC_LANG_C
acx_pthread_ok=no

# We used to check for pthread.h first, but this fails if pthread.h
# requires special compiler flags (e.g. on True64 or Sequent).
# It gets checked for in the link test anyway.

# First of all, check if the user has set any of the PTHREAD_LIBS,
# etcetera environment variables, and if threads linking works using
# them:
if test x"$PTHREAD_LIBS$PTHREAD_CFLAGS" != x; then
        save_CFLAGS="$CFLAGS"
        CFLAGS="$CFLAGS $PTHREAD_CFLAGS"
        save_LIBS="$LIBS"
        LIBS="$PTHREAD_LIBS $LIBS"
        AC_MSG_CHECKING([for pthread_join in LIBS=$PTHREAD_LIBS with CFLAGS=$PTHREAD_CFLAGS])
        AC_TRY_LINK_FUNC(pthread_join, acx_pthread_ok=yes)
        AC_MSG_RESULT($acx_pthread_ok)
        if test x"$acx_pthread_ok" = xno; then
                PTHREAD_LIBS=""
                PTHREAD_CFLAGS=""
        fi
        LIBS="$save_LIBS"
        CFLAGS="$save_CFLAGS"
fi

# We must check for the threads library under a number of different
# names; the ordering is very important because some systems
# (e.g. DEC) have both -lpthread and -lpthreads, where one of the
# libraries is broken (non-POSIX).

# Create a list of thread flags to try.  Items starting with a "-" are
# C compiler flags, and other items are library names, except for "none"
# which indicates that we try without any flags at all, and "pthread-config"
# which is a program returning the flags for the Pth emulation library.

acx_pthread_flags="pthreads none -Kthread -kthread lthread -pthread -pthreads -mthreads pthread --thread-safe -mt pthread-config"

# The ordering *is* (sometimes) important.  Some notes on the
# individual items follow:

# pthreads: AIX (must check this before -lpthread)
# none: in case threads are in libc; should be tried before -Kthread and
#       other compiler flags to prevent continual compiler warnings
# -Kthread: Sequent (threads in libc, but -Kthread needed for pthread.h)
# -kthread: FreeBSD kernel threads (preferred to -pthread since SMP-able)
# lthread: LinuxThreads port on FreeBSD (also preferred to -pthread)
# -pthread: Linux/gcc (kernel threads), BSD/gcc (userland threads)
# -pthreads: Solaris/gcc
# -mthreads: Mingw32/gcc, Lynx/gcc
# -mt: Sun Workshop C (may only link SunOS threads [-lthread], but it
#      doesn't hurt to check since this sometimes defines pthreads too;
#      also defines -D_REENTRANT)
#      ... -mt is also the pthreads flag for HP/aCC
# pthread: Linux, etcetera
# --thread-safe: KAI C++
# pthread-config: use pthread-config program (for GNU Pth library)

case "${host_cpu}-${host_os}" in
        *solaris*)

        # On Solaris (at least, for some versions), libc contains stubbed
        # (non-functional) versions of the pthreads routines, so link-based
        # tests will erroneously succeed.  (We need to link with -pthreads/-mt/
        # -lpthread.)  (The stubs are missing pthread_cleanup_push, or rather
        # a function called by this macro, so we could check for that, but
        # who knows whether they'll stub that too in a future libc.)  So,
        # we'll just look for -pthreads and -lpthread first:

        acx_pthread_flags="-pthreads pthread -mt -pthread $acx_pthread_flags"
        ;;
esac

if test x"$acx_pthread_ok" = xno; then
for flag in $acx_pthread_flags; do

        case $flag in
                none)
                AC_MSG_CHECKING([whether pthreads work without any flags])
                ;;

                -*)
                AC_MSG_CHECKING([whether pthreads work with $flag])
                PTHREAD_CFLAGS="$flag"
                ;;

		pthread-config)
		AC_CHECK_PROG(acx_pthread_config, pthread-config, yes, no)
		if test x"$acx_pthread_config" = xno; then continue; fi
		PTHREAD_CFLAGS="`pthread-config --cflags`"
		PTHREAD_LIBS="`pthread-config --ldflags` `pthread-config --libs`"
		;;

                *)
                AC_MSG_CHECKING([for the pthreads library -l$flag])
                PTHREAD_LIBS="-l$flag"
                ;;
        esac

        save_LIBS="$LIBS"
        save_CFLAGS="$CFLAGS"
        LIBS="$PTHREAD_LIBS $LIBS"
        CFLAGS="$CFLAGS $PTHREAD_CFLAGS"

        # Check for various functions.  We must include pthread.h,
        # since some functions may be macros.  (On the Sequent, we
        # need a special flag -Kthread to make this header compile.)
        # We check for pthread_join because it is in -lpthread on IRIX
        # while pthread_create is in libc.  We check for pthread_attr_init
        # due to DEC craziness with -lpthreads.  We check for
        # pthread_cleanup_push because it is one of the few pthread
        # functions on Solaris that doesn't have a non-functional libc stub.
        # We try pthread_create on general principles.
        AC_TRY_LINK([#include <pthread.h>],
                    [pthread_t th; pthread_join(th, 0);
                     pthread_attr_init(0); pthread_cleanup_push(0, 0);
                     pthread_create(0,0,0,0); pthread_cleanup_pop(0); ],
                    [acx_pthread_ok=yes])

        LIBS="$save_LIBS"
        CFLAGS="$save_CFLAGS"

        AC_MSG_RESULT($acx_pthread_ok)
        if test "x$acx_pthread_ok" = xyes; then
                break;
        fi

        PTHREAD_LIBS=""
        PTHREAD_CFLAGS=""
done
fi

# Various other checks:
if test "x$acx_pthread_ok" = xyes; then
        save_LIBS="$LIBS"
        LIBS="$PTHREAD_LIBS $LIBS"
        save_CFLAGS="$CFLAGS"
        CFLAGS="$CFLAGS $PTHREAD_CFLAGS"

        # Detect AIX lossage: JOINABLE attribute is called UNDETACHED.
	AC_MSG_CHECKING([for joinable pthread attribute])
	attr_name=unknown
	for attr in PTHREAD_CREATE_JOINABLE PTHREAD_CREATE_UNDETACHED; do
	    AC_TRY_LINK([#include <pthread.h>], [int attr=$attr; return attr;],
                        [attr_name=$attr; break])
	done
        AC_MSG_RESULT($attr_name)
        if test "$attr_name" != PTHREAD_CREATE_JOINABLE; then
            AC_DEFINE_UNQUOTED(PTHREAD_CREATE_JOINABLE, $attr_name,
                               [Define to necessary symbol if this constant
                                uses a non-standard name on your system.])
        fi

        AC_MSG_CHECKING([if more special flags are required for pthreads])
        flag=no
        case "${host_cpu}-${host_os}" in
            *-aix* | *-freebsd* | *-darwin*) flag="-D_THREAD_SAFE";;
            *solaris* | *-osf* | *-hpux*) flag="-D_REENTRANT";;
        esac
        AC_MSG_RESULT(${flag})
        if test "x$flag" != xno; then
            PTHREAD_CFLAGS="$flag $PTHREAD_CFLAGS"
        fi

        LIBS="$save_LIBS"
        CFLAGS="$save_CFLAGS"

        # More AIX lossage: must compile with xlc_r or cc_r
	if test x"$GCC" != xyes; then
          AC_CHECK_PROGS(PTHREAD_CC, xlc_r cc_r, ${CC})
        else
          PTHREAD_CC=$CC
	fi
else
        PTHREAD_CC="$CC"
fi

AC_SUBST(PTHREAD_LIBS)
AC_SUBST(PTHREAD_CFLAGS)
AC_SUBST(PTHREAD_CC)

# Finally, execute ACTION-IF-FOUND/ACTION-IF-NOT-FOUND:
if test x"$acx_pthread_ok" = xyes; then
        ifelse([$1],,AC_DEFINE(HAVE_PTHREAD,1,[Define if you have POSIX threads libraries and header files.]),[$1])
        :
else
        acx_pthread_ok=no
        $2
fi
AC_LANG_RESTORE
])dnl ACX_PTHREAD

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
    libpng_CPPFLAGS=`$LIBPNG12_CONFIG --cflags`
    # should be --ldopts, but it's currently broken
    libpng_LDFLAGS=`$LIBPNG12_CONFIG --ldflags`
    libpng_LDFLAGS=`echo " $libpng_LDFLAGS" | sed 's/ -l[[^ ]][[^ ]]*//g'`
  elif test -n "$LIBPNG_CONFIG"; then
    libpng_CPPFLAGS=`$LIBPNG_CONFIG --cflags`
    # should be --ldopts, but it's currently broken
    libpng_LDFLAGS=`$LIBPNG_CONFIG --ldflags`
    libpng_LDFLAGS=`echo " $libpng_LDFLAGS" | sed 's/ -l[[^ ]][[^ ]]*//g'`
  fi

  AC_CHECK_HEADERS(png.h,,
    [AC_CHECK_HEADERS(libpng/png.h,
       [CPPFLAGS="$CPPFLAGS $libpng_CPPFLAGS/libpng"],
       [CPPFLAGS="$_cppflags"])])

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

  _cppflags="$CPPFLAGS"
  _ldflags="$LDFLAGS"
  CPPFLAGS="$libft_INCLUDES $CPPFLAGS"
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
      CPPFLAGS="$_cppflags"])

else
  ac_cv_lib_freetype_FT_Init_FreeType=no
fi

dnl check for libfontconfig by default
AC_ARG_WITH(fontconfig,dnl
[  --with-fontconfig=DIR         where to find the fontconfig library],
  [if test -d "$withval"; then
     LDFLAGS="$LDFLAGS -L$withval/lib"
     CPPFLAGS="$CPPFLAGS -I$withval/include"
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
     CPPFLAGS="$CPPFLAGS -I$withval/include"
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
     CPPFLAGS="$CPPFLAGS -I$withval/include"
   fi],
  withval=yes)

if test "$withval" != no; then
  AC_CHECK_LIB(Xpm,XpmReadFileToXpmImage,
    [LIBS="-lXpm -lX11 $LIBS"
     FEATURES="GD_XPM $FEATURES"
     AC_DEFINE(HAVE_LIBXPM, 1, [ Define if you have the Xpm library. ])],,
    [-lX11])
else
  ac_cv_lib_Xpm_XpmReadFileToXpmImage=no
fi

ACX_PTHREAD()

dnl report configuration
AC_MSG_RESULT([
** Configuration summary for $PACKAGE $VERSION:

   Support for PNG library:          $ac_cv_lib_png_png_create_read_struct
   Support for JPEG library:         $ac_cv_lib_jpeg_jpeg_set_defaults
   Support for Freetype 2.x library: $ac_cv_lib_freetype_FT_Init_FreeType
   Support for Fontconfig library:   $ac_cv_lib_fontconfig_FcInit
   Support for Xpm library:          $ac_cv_lib_Xpm_XpmReadFileToXpmImage
   Support for pthreads:             $acx_pthread_ok
])

AC_CONFIG_FILES([Makefile config/Makefile config/gdlib-config test/Makefile])

AC_OUTPUT
