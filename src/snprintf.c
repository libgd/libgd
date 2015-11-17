/* Provide a snprintf on Windows for older Visual Studio builds.
 * VS2013 and older do not support C99 snprintf(). The subsitute _snprintf()
 * does not correctly NUL-terminate buffers in case of overflow.
 * This implementation emulates the ISO C99 snprintf() for VS2013 and older.
 */

#if defined(_MSC_VER) && _MSC_VER < 1900

#include <stdio.h>
#include <stdarg.h>

int snprintf(char* buf, size_t len, const char* fmt, ...)
{
  int n;
  va_list ap;
  va_start(ap, fmt);

  n = _vscprintf(fmt, ap);
  vsnprintf_s(buf, len, _TRUNCATE, fmt, ap);

  va_end(ap);
  return n;
}

#endif
