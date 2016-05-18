# Find libimagequant includes and library (and download+build if needed)
# http://pngquant.org/lib
#
# This module defines
#  LIQ_INCLUDE_DIR, where to find libimagequant.h
#  LIQ_LIBRARIES, the libraries to link against to use libimagequant.
#  LIQ_FOUND, If false, do not try to use libimagequant.

SET(LIQ_FOUND "NO")

FIND_PATH(LIQ_INCLUDE_DIR libimagequant.h
"${PROJECT_SOURCE_DIR}/libimagequant"
"${PROJECT_SOURCE_DIR}/pngquant/lib"
/usr/local/include
/usr/include
)

FIND_LIBRARY(LIQ_LIBRARY
  NAMES libimagequant imagequant
  PATHS "${PROJECT_SOURCE_DIR}/libimagequant" "${PROJECT_SOURCE_DIR}/pngquant/lib" /usr/lib64 /usr/lib /usr/local/lib
)

IF (LIQ_LIBRARY AND LIQ_INCLUDE_DIR)
    SET(LIQ_FOUND "YES")
    SET(LIQ_LIBRARIES ${LIQ_LIBRARY})
		SET(HAVE_LIBIMAGEQUANT 1)
ENDIF (LIQ_LIBRARY AND LIQ_INCLUDE_DIR)

IF (LIQ_FOUND)
  IF (NOT LIQ_FIND_QUIETLY)
    MESSAGE(STATUS "Found LIQ: ${LIQ_LIBRARY} ${LIQ_INCLUDE_DIR}")
  ENDIF (NOT LIQ_FIND_QUIETLY)
ELSE (LIQ_FOUND)
  # if existing library not found, then download and build it
  IF (NOT WIN32 OR CYGWIN OR MINGW) # MSVC's C compiler is too old to compile libimagequant
    IF (CMAKE_VERSION VERSION_GREATER "2.8.1")
      MESSAGE(STATUS "LIQ will be built")
      INCLUDE(ExternalProject)
      EXTERNALPROJECT_ADD(
          libimagequant
          URL "http://pngquant.org/libimagequant-2.0.0-src.tar.bz2"
          SOURCE_DIR libimagequant
          BUILD_IN_SOURCE 1
          INSTALL_DIR libimagequant
          INSTALL_COMMAND true
          CONFIGURE_COMMAND true
          BUILD_COMMAND make static CFLAGSADD='-fPIC'
      )

      SET(LIQ_FOUND "SORTOF")
      SET(LIQ_BUILD "YES")
    SET(LIQ_LIBRARIES "${PROJECT_BINARY_DIR}/libimagequant/libimagequant.a")
    SET(LIQ_INCLUDE_DIR "${PROJECT_BINARY_DIR}/libimagequant/")
      SET(HAVE_LIBIMAGEQUANT 1)
    ENDIF(CMAKE_VERSION VERSION_GREATER "2.8.1")
  ENDIF(NOT WIN32 OR CYGWIN OR MINGW)
ENDIF (LIQ_FOUND)

MARK_AS_ADVANCED(LIQ_INCLUDE_DIR LIQ_LIBRARIES LIQ_BUILD)
