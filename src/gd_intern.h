#ifndef GD_INTERN_H
#define GD_INTERN_H

#if HAVE_LIMITS_H
#include <limits.h>
#endif

#ifndef MIN
#define MIN(a,b) ((a)<(b)?(a):(b))
#endif
#define MIN3(a,b,c) ((a)<(b)?(MIN(a,c)):(MIN(b,c)))
#ifndef MAX
#define MAX(a,b) ((a)<(b)?(b):(a))
#endif
#define MAX3(a,b,c) ((a)<(b)?(MAX(b,c)):(MAX(a,c)))

#ifndef MAXPATHLEN
# ifdef PATH_MAX
#  define MAXPATHLEN PATH_MAX
# elif defined(MAX_PATH)
#  define MAXPATHLEN MAX_PATH
# else
#  if defined(__GNU__)
#   define MAXPATHLEN 4096
#  else
#   define MAXPATHLEN 256    /* Should be safe for any weird systems that do not define it */
#  endif
# endif
#endif

#ifdef HAVE_STDINT_H
# include <stdint.h>
#else
# if defined(HAVE_INTTYPES_H)
#  include <inttypes.h>
# else
#  include "msinttypes/inttypes.h"
# endif
#endif

#endif

