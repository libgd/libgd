# Keep in sync with AC_CHECK_HEADERS in configure.ac.

check_include_files(dirent.h HAVE_DIRENT_H)
check_include_files(errno.h HAVE_ERRNO_H)
check_include_files(inttypes.h HAVE_INTTYPES_H)
check_include_files(limits.h HAVE_LIMITS_H)
check_include_files(stddef.h HAVE_STDDEF_H)
check_include_files(stdint.h HAVE_STDINT_H)
check_include_files(stdlib.h HAVE_STDLIB_H)
check_include_files(string.h HAVE_STRING_H)
check_include_files(strings.h HAVE_STRINGS_H)
check_include_files(unistd.h HAVE_UNISTD_H)

check_include_files(sys/stat.h HAVE_SYS_STAT_H)
check_include_files(sys/types.h HAVE_SYS_TYPES_H)

set(HAVE_LIBM 1)
