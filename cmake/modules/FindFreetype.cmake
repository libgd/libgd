# - Locate FreeType library
# This module defines
#  FREETYPE_LIBRARIES, the library to link against
#  FREETYPE_FOUND, if false, do not try to link to FREETYPE
#  FREETYPE_INCLUDE_DIRS, where to find headers.
#  FREETYPE_VERSION_STRING, the version of freetype found (since CMake 2.8.8)
#  This is the concatenation of the paths:
#  FREETYPE_INCLUDE_DIR_ft2build
#  FREETYPE_INCLUDE_DIR_freetype2
#
# $FREETYPE_DIR is an environment variable that would
# correspond to the ./configure --prefix=$FREETYPE_DIR
# used in building FREETYPE.

#=============================================================================
# Copyright 2007-2009 Kitware, Inc.
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

# Created by Eric Wing.
# Modifications by Alexander Neundorf.
# This file has been renamed to "FindFreetype.cmake" instead of the correct
# "FindFreeType.cmake" in order to be compatible with the one from KDE4, Alex.

# Ugh, FreeType seems to use some #include trickery which
# makes this harder than it should be. It looks like they
# put ft2build.h in a common/easier-to-find location which
# then contains a #include to a more specific header in a
# more specific location (#include <freetype/config/ftheader.h>).
# Then from there, they need to set a bunch of #define's
# so you can do something like:
# #include FT_FREETYPE_H
# Unfortunately, using CMake's mechanisms like include_directories()
# wants explicit full paths and this trickery doesn't work too well.
# I'm going to attempt to cut out the middleman and hope
# everything still works.
find_path(FREETYPE_INCLUDE_DIR_ft2build ft2build.h
  HINTS
    ENV FREETYPE_DIR
  PATHS
    /usr/local/X11R6
    /usr/local/X11
    /usr/freeware
  PATH_SUFFIXES include/freetype2 include
)

find_path(FREETYPE_INCLUDE_DIR_freetype2 config/ftheader.h
  HINTS
    ENV FREETYPE_DIR
  PATHS
    /usr/local/X11R6
    /usr/local/X11
    /usr/freeware
  PATH_SUFFIXES include/freetype2 include
)

find_library(FREETYPE_LIBRARY
  NAMES freetype libfreetype freetype219 freetype_a
  HINTS
    ENV FREETYPE_DIR
  PATH_SUFFIXES lib
  PATHS
  /usr/local/X11R6
  /usr/local/X11
  /usr/freeware
)

# set the user variables
if(FREETYPE_INCLUDE_DIR_ft2build AND FREETYPE_INCLUDE_DIR_freetype2)
  set(FREETYPE_INCLUDE_DIRS "${FREETYPE_INCLUDE_DIR_ft2build};${FREETYPE_INCLUDE_DIR_freetype2}")
endif()
set(FREETYPE_LIBRARIES "${FREETYPE_LIBRARY}")

if(FREETYPE_INCLUDE_DIR_freetype2 AND EXISTS "${FREETYPE_INCLUDE_DIR_freetype2}/freetype/freetype.h")
    file(STRINGS "${FREETYPE_INCLUDE_DIR_freetype2}/freetype/freetype.h" freetype_version_str
         REGEX "^#[\t ]*define[\t ]+FREETYPE_(MAJOR|MINOR|PATCH)[\t ]+[0-9]+$")

    unset(FREETYPE_VERSION_STRING)
    foreach(VPART MAJOR MINOR PATCH)
        foreach(VLINE ${freetype_version_str})
            if(VLINE MATCHES "^#[\t ]*define[\t ]+FREETYPE_${VPART}")
                string(REGEX REPLACE "^#[\t ]*define[\t ]+FREETYPE_${VPART}[\t ]+([0-9]+)$" "\\1"
                       FREETYPE_VERSION_PART "${VLINE}")
                if(FREETYPE_VERSION_STRING)
                    set(FREETYPE_VERSION_STRING "${FREETYPE_VERSION_STRING}.${FREETYPE_VERSION_PART}")
                else()
                    set(FREETYPE_VERSION_STRING "${FREETYPE_VERSION_PART}")
                endif()
                unset(FREETYPE_VERSION_PART)
            endif()
        endforeach()
    endforeach()
endif()


# handle the QUIETLY and REQUIRED arguments and set FREETYPE_FOUND to TRUE if
# all listed variables are TRUE
include(${CMAKE_CURRENT_LIST_DIR}/FindPackageHandleStandardArgs.cmake)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(Freetype
                                  REQUIRED_VARS FREETYPE_LIBRARY FREETYPE_INCLUDE_DIRS
                                  VERSION_VAR FREETYPE_VERSION_STRING)

mark_as_advanced(FREETYPE_LIBRARY FREETYPE_INCLUDE_DIR_freetype2 FREETYPE_INCLUDE_DIR_ft2build)
