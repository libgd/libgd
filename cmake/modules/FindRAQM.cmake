# - Find the native RAQM includes and library
#

# This module defines
#  RAQM_INCLUDE_DIR, where to find raqm.h, etc.
#  RAQM_LIBRARIES, the libraries to link against to use RAQM.
#  RAQM_FOUND, If false, do not try to use RAQM.
# Also defined, but not for general use are
#  RAQM_LIBRARY, where to find the RAQM library.

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

SET(RAQM_FOUND "NO")

FIND_PATH(RAQM_INCLUDE_DIR raqm.h
  /usr/local/include
  /usr/include
)

SET(RAQM_NAMES ${RAQM_NAMES} raqm)
FIND_LIBRARY(RAQM_LIBRARY
  NAMES ${RAQM_NAMES}
  PATH /usr/local/lib /usr/lib
)

IF (RAQM_LIBRARY AND RAQM_INCLUDE_DIR)
	SET(RAQM_FOUND "YES")
	SET(HAVE_LIBRAQM 1)
	SET(RAQM_LIBRARIES ${RAQM_LIBRARY})
ENDIF (RAQM_LIBRARY AND RAQM_INCLUDE_DIR)

IF (RAQM_FOUND)
	IF (NOT RAQM_FIND_QUIETLY)
		MESSAGE(STATUS "Find RAQM: ${RAQM_LIBRARY}")
	ENDIF (NOT RAQM_FIND_QUIETLY)
ELSE (RAQM_FOUDN)
	IF (RAQM_FIND_REQUIRED)
		MESSAGE(FATAL_ERROR "Could not find RAQM library")
	ENDIF (RAQM_FIND_REQUIRED)
ENDIF (RAQM_FOUND)

MARK_AS_ADVANCED(RAQM_INCLUDE_DIR RAQM_LIBRARY)
