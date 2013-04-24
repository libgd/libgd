# - Find the native PNG includes and library
#

# This module defines
#  VPX_INCLUDE_DIR, where to find png.h, etc.
#  VPX_LIBRARIES, the libraries to link against to use PNG.
#  VPX_DEFINITIONS - You should ADD_DEFINITONS(${PNG_DEFINITIONS}) before compiling code that includes png library files.
#  VPX_FOUND, If false, do not try to use PNG.
# also defined, but not for general use are
#  VPX_LIBRARY, where to find the PNG library.
# None of the above will be defined unles zlib can be found.
# PNG depends on Zlib
#
# Copyright (c) 2002 Kitware, Inc., Insight Consortium.  All rights reserved.
# See Copyright.txt or http://www.cmake.org/HTML/Copyright.html for details.


INCLUDE(FindZLIB)

SET(VPX_FOUND "NO")
SET(VPX_LIBRARY "")

FIND_PATH(VPX_INCLUDE_DIR vp8cx.h
/usr/local/include/vpx
/usr/include/vpx
)

SET(VPX_NAMES ${VPX_NAMES} "vpxmt" "libvpx")
FIND_LIBRARY(VPX_LIBRARY
 NAMES ${VPX_NAMES}
 PATHS "${PROJECT_SOURCE_DIR}/../deps/lib" /usr/lib64 /usr/lib /usr/local/lib 
)

IF (VPX_LIBRARY AND VPX_INCLUDE_DIR)
	SET(VPX_INCLUDE_DIR ${VPX_INCLUDE_DIR})
	SET(VPX_LIBRARIES ${VPX_LIBRARY})
	SET(VPX_FOUND "YES")

ENDIF (VPX_LIBRARY AND VPX_INCLUDE_DIR)

IF (VPX_FOUND)
  IF (NOT VPX_FIND_QUIETLY)
    MESSAGE(STATUS "Found VPX: ${VPX_LIBRARY}")
  ENDIF (NOT VPX_FIND_QUIETLY)
ELSE (VPX_FOUND)
  IF (VPX_FIND_REQUIRED)
    MESSAGE(FATAL_ERROR "Could not find VPX library")
  ENDIF (VPX_FIND_REQUIRED)
ENDIF (VPX_FOUND)

MARK_AS_ADVANCED(VPX_INCLUDE_DIR VPX_LIBRARY )
SET(VPX_LIBRARIES ${VPX_LIBRARY})
