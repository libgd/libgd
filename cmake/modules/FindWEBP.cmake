# - Find the native WEBP includes and library
#

# This module defines
#  WEBP_INCLUDE_DIR, where to find decode.h, etc.
#  WEBP_LIBRARIES, the libraries to link against to use WEBP.
#  WEBP_FOUND, If false, do not try to use WEBP.
# also defined, but not for general use are
#  WEBP_LIBRARY, where to find the WEBP library.
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

include(FindZLIB)

find_path(WEBP_INCLUDE_DIR decode.h
/usr/local/include/webp
/usr/include/webp
)

set(WEBP_NAMES ${WEBP_NAMES} webp)
find_library(WEBP_LIBRARY
  NAMES ${WEBP_NAMES}
  PATHS "${PROJECT_SOURCE_DIR}/../deps/lib" /usr/lib64 /usr/lib /usr/local/lib
  )

if (WEBP_LIBRARY AND WEBP_INCLUDE_DIR)
  set(WEBP_INCLUDE_DIR ${WEBP_INCLUDE_DIR})
  set(WEBP_LIBRARIES ${WEBP_LIBRARY})
  set(WEBP_FOUND "YES")

endif (WEBP_LIBRARY AND WEBP_INCLUDE_DIR)

if (WEBP_FOUND)
  if (NOT WEBP_FIND_QUIETLY)
    message(STATUS "Found WEBP: ${WEBP_LIBRARY}")
  endif (NOT WEBP_FIND_QUIETLY)
else (WEBP_FOUND)
  if (WEBP_FIND_REQUIRED)
    message(FATAL_ERROR "Could not find WEBP library")
  endif (WEBP_FIND_REQUIRED)
endif (WEBP_FOUND)

mark_as_advanced(WEBP_INCLUDE_DIR WEBP_LIBRARY )
set(WEBP_LIBRARIES ${WEBP_LIBRARY})
