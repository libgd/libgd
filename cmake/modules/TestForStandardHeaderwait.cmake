# cmake/modules/CheckHEADER_SYS_WAIT.cmake
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

#
# - Check for sys/wait.h that is POSIX.1 compatible following autotools
#   AC_HEADER_SYS_WAIT

include(CheckCSourceCompiles)

set(_CHECK_HEADER_SYS_WAIT_SOURCE_CODE "
#include <sys/types.h>
#include <sys/wait.h>
#ifndef WEXITSTATUS
# define WEXITSTATUS(stat_val) ((unsigned)(stat_val) >> 8)
#endif
#ifndef WIFEXITED
# define WIFEXITED(stat_val) (((stat_val) & 255) == 0)
#endif
int
main ()
{
  int s;
  wait (&s);
  s = WIFEXITED (s) ? WEXITSTATUS (s) : 1;
  ;
  return 0;
}
")
check_c_source_compiles(
"${_CHECK_HEADER_SYS_WAIT_SOURCE_CODE}"
HAVE_SYS_WAIT_H)
