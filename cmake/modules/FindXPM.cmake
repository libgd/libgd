# - Find the native PNG includes and library
#

# This module defines
#  XPM_INCLUDE_DIR, where to find png.h, etc.
#  XPM_LIBRARIES, the libraries to link against to use PNG.
#  XPM_DEFINITIONS - You should ADD_DEFINITONS(${PNG_DEFINITIONS}) before compiling code that includes png library files.
#  XPM_FOUND, If false, do not try to use PNG.
# also defined, but not for general use are
#  XPM_LIBRARY, where to find the PNG library.
# None of the above will be defined unles zlib can be found.
# PNG depends on Zlib
#
# Copyright (c) 2002 Kitware, Inc., Insight Consortium.  All rights reserved.
# See Copyright.txt or http://www.cmake.org/HTML/Copyright.html for details.


INCLUDE(FindZLIB)

SET(XPM_FOUND "NO")

FIND_PATH(XPM_XPM_INCLUDE_DIR xpm.h
/usr/local/include/X11
/usr/include/X11
)

SET(XPM_NAMES ${XPM_NAMES} Xpm libXpm)
FIND_LIBRARY(XPM_LIBRARY
 NAMES ${XPM_NAMES}
 PATHS /usr/lib64 /usr/lib /usr/local/lib
)

IF (XPM_LIBRARY AND XPM_XPM_INCLUDE_DIR)
	SET(XPM_INCLUDE_DIR ${XPM_XPM_INCLUDE_DIR})
	SET(XPM_LIBRARIES ${XPM_LIBRARY})
	SET(XPM_FOUND "YES")

	IF (CYGWIN)
	  IF(BUILD_SHARED_LIBS)
		  # No need to define XPM_USE_DLL here, because it's default for Cygwin.
	  ELSE(BUILD_SHARED_LIBS)
		 SET (XPM_DEFINITIONS -DXPM_STATIC)
	  ENDIF(BUILD_SHARED_LIBS)
	ENDIF (CYGWIN)
ENDIF (XPM_LIBRARY AND XPM_XPM_INCLUDE_DIR)

IF (XPM_FOUND)
  IF (NOT XPM_FIND_QUIETLY)
    MESSAGE(STATUS "Found XPM: ${XPM_LIBRARY}")
  ENDIF (NOT XPM_FIND_QUIETLY)
ELSE (XPM_FOUND)
  IF (XPM_FIND_REQUIRED)
    MESSAGE(FATAL_ERROR "Could not find XPM library")
  ENDIF (XPM_FIND_REQUIRED)
ENDIF (XPM_FOUND)

MARK_AS_ADVANCED(XPM_XPM_INCLUDE_DIR XPM_LIBRARY )
