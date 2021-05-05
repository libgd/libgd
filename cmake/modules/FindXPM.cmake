# - Find the native XPM includes and library
#

# This module defines
#  XPM_INCLUDE_DIR, where to find xpm.h, etc.
#  XPM_LIBRARIES, the libraries to link against to use XPM.
#  XPM_DEFINITIONS - You should ADD_DEFINITONS(${XPM_DEFINITIONS}) before compiling code that includes xpm library files.
#  XPM_FOUND, If false, do not try to use XPM.
# also defined, but not for general use are
#  XPM_LIBRARY, where to find the XPM library.

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
