# Find libimagequant includes and library (and download+build if needed)
# http://pngquant.org/lib
#
# This module defines
#  LIQ_INCLUDE_DIR, where to find libimagequant.h
#  LIQ_LIBRARIES, the libraries to link against to use libimagequant.
#  LIQ_FOUND, if false, do not try to use libimagequant.

SET(LIQ_FOUND "NO")

FIND_PATH(LIQ_INCLUDE_DIR libimagequant.h
  /usr/local/include
  /usr/include
)

FIND_LIBRARY(LIQ_LIBRARY
  NAMES libimagequant imagequant
  PATHS /usr/lib64 /usr/lib /usr/local/lib
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
ENDIF (LIQ_FOUND)

MARK_AS_ADVANCED(LIQ_INCLUDE_DIR LIQ_LIBRARIES LIQ_BUILD)
