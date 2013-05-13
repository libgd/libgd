# - Find the native VPX includes and library
#

# This module defines
#  VPX_INCLUDE_DIR, where to find png.h, etc.
#  VPX_LIBRARIES, the libraries to link against to use VPX.
#  VPX_DEFINITIONS - You should ADD_DEFINITONS(${VPX_DEFINITIONS}) before compiling code that includes png library files.
#  VPX_FOUND, If false, do not try to use VPX.
# also defined, but not for general use are
#  VPX_LIBRARY, where to find the VPX library.
#
# Copyright (c) 2002 Kitware, Inc., Insight Consortium.  All rights reserved.
#
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions are
# met:
#
#  * Redistributions of source code must retain the above copyright notice,
#    this list of conditions and the following disclaimer.
# 
#  * Redistributions in binary form must reproduce the above copyright notice,
#    this list of conditions and the following disclaimer in the documentation
#    and/or other materials provided with the distribution.
# 
#  * The names of Kitware, Inc., the Insight Consortium, or the names of
#    any consortium members, or of any contributors, may not be used to
#    endorse or promote products derived from this software without
#    specific prior written permission.
# 
# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDER AND CONTRIBUTORS ``AS IS''
# AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
# IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
# ARE DISCLAIMED. IN NO EVENT SHALL THE AUTHORS OR CONTRIBUTORS BE LIABLE FOR
# ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
# DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
# SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
# CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
# OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
# OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

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
