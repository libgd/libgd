# MINGW_AC_WIN32_NATIVE_HOST
# --------------------------
# Check if the runtime platform is a native Win32 host.
#
AC_DEFUN([MINGW_AC_WIN32_NATIVE_HOST],
[AC_CACHE_CHECK([whether we are building for a Win32 host], [mingw_cv_win32_host],
 AC_COMPILE_IFELSE([AC_LANG_SOURCE([[
#ifdef _WIN32
 choke me
#endif]])], [mingw_cv_win32_host=no], [mingw_cv_win32_host=yes]))dnl
])# MINGW_AC_WIN32_NATIVE_HOST
