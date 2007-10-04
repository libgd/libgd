# cmake/modules/TestForHighBitCharacters.cmake
#
# Copyright (C) 2006  Alan W. Irwin
#
# This file is part of PLplot.
#
# PLplot is free software; you can redistribute it and/or modify
# it under the terms of the GNU Library General Public License as published
# by the Free Software Foundation; version 2 of the License.
#
# PLplot is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU Library General Public License for more details.
#
# You should have received a copy of the GNU Library General Public License
# along with the file PLplot; if not, write to the Free Software
# Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301  USA

# Check if ctype.h macros work on characters with the high bit set.
if(NOT DEFINED CMAKE_HIGH_BIT_CHARACTERS)
   message(STATUS 
      "Check for whether ctype.h macros work on characters with the\n"
   "   high bit set."
   )
  try_compile(CMAKE_HIGH_BIT_CHARACTERS  
    ${CMAKE_BINARY_DIR}
    ${CMAKE_SOURCE_DIR}/CMakeModules/TestForHighBitCharacters.c
    OUTPUT_VARIABLE OUTPUT)
  if(CMAKE_HIGH_BIT_CHARACTERS)
    message(STATUS "High-bit characters - work")
    set(HIGH_BIT_CHARACTERS 1 CACHE INTERNAL
      "Do ctype.h macros work on high-bit characters")
      file(APPEND ${CMAKE_BINARY_DIR}/CMakeFiles/CMakeOutput.log
      "Determining if ctype.h macros work on high-bit characters passed with "
      "the following output:\n${OUTPUT}\n\n")
  else(CMAKE_HIGH_BIT_CHARACTERS)
    message(STATUS "High-bit characters - don't work")
    set(HIGH_BIT_CHARACTERS 0 CACHE INTERNAL
      "Do ctype.h macros work on high-bit characters")
      file(APPEND ${CMAKE_BINARY_DIR}/CMakeFiles/CMakeError.log
      "Determining if ctype.h macros work on high-bit characters failed with "
      "the following output:\n${OUTPUT}\n\n")
  endif(CMAKE_HIGH_BIT_CHARACTERS)
endif(NOT DEFINED CMAKE_HIGH_BIT_CHARACTERS)
