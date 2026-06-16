# - Find the native JXL includes and library
#
# This module defines
#  JXL_INCLUDE_DIR, where to find jxl/decode.h, etc.
#  JXL_LIBRARIES, the libraries to link against to use JXL.
#  JXL_FOUND, If false, do not try to use JXL.
# also defined, but not for general use are
#  JXL_LIBRARY, where to find the JXL library.
#  JXL_THREADS_LIBRARY, where to find the optional JXL threads library.
#  JXL_CMS_LIBRARY, where to find the JXL CMS library.

find_package(PkgConfig QUIET)
if (PKG_CONFIG_FOUND)
  pkg_check_modules(JXL_PKG QUIET libjxl)
  pkg_check_modules(JXL_CMS_PKG QUIET libjxl_cms)
  pkg_check_modules(JXL_THREADS_PKG QUIET libjxl_threads)
endif ()

find_path(JXL_INCLUDE_DIR
  NAMES jxl/decode.h
  HINTS ${JXL_PKG_INCLUDE_DIRS} ${JXL_CMS_PKG_INCLUDE_DIRS}
  PATH_SUFFIXES include
)
find_library(JXL_LIBRARY
  NAMES jxl libjxl
  HINTS ${JXL_PKG_LIBRARY_DIRS}
  PATH_SUFFIXES lib
)
find_library(JXL_CMS_LIBRARY
  NAMES jxl_cms libjxl_cms
  HINTS ${JXL_CMS_PKG_LIBRARY_DIRS} ${JXL_PKG_LIBRARY_DIRS}
  PATH_SUFFIXES lib
)
find_library(JXL_THREADS_LIBRARY
  NAMES jxl_threads libjxl_threads
  HINTS ${JXL_THREADS_PKG_LIBRARY_DIRS} ${JXL_PKG_LIBRARY_DIRS}
  PATH_SUFFIXES lib
)

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(JXL
  REQUIRED_VARS JXL_LIBRARY JXL_CMS_LIBRARY JXL_INCLUDE_DIR
)

if (JXL_FOUND)
  set(JXL_LIBRARIES ${JXL_LIBRARY} ${JXL_CMS_LIBRARY})
  if (JXL_THREADS_LIBRARY)
    list(APPEND JXL_LIBRARIES ${JXL_THREADS_LIBRARY})
  endif ()
  set(JXL_INCLUDE_DIRS ${JXL_INCLUDE_DIR})
endif ()

mark_as_advanced(JXL_INCLUDE_DIR JXL_LIBRARY JXL_THREADS_LIBRARY JXL_CMS_LIBRARY)
