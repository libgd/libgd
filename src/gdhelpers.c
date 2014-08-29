#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "gd.h"
#include "gdhelpers.h"
#include <stdlib.h>
#include <string.h>

#include <sys/types.h>
#include <ctype.h>

/* TBB: gd_strtok_r is not portable; provide an implementation */

#define SEP_TEST (separators[*((unsigned char *) s)])

char *
gd_strtok_r (char *s, char *sep, char **state)
{
	char separators[256];
	char *result = 0;
	memset (separators, 0, sizeof (separators));
	while (*sep) {
		separators[*((unsigned char *) sep)] = 1;
		sep++;
	}
	if (!s) {
		/* Pick up where we left off */
		s = *state;
	}
	/* 1. EOS */
	if (!(*s)) {
		*state = s;
		return 0;
	}
	/* 2. Leading separators, if any */
	if (SEP_TEST) {
		do {
			s++;
		} while (SEP_TEST);
		/* 2a. EOS after separators only */
		if (!(*s)) {
			*state = s;
			return 0;
		}
	}
	/* 3. A token */
	result = s;
	do {
		/* 3a. Token at end of string */
		if (!(*s)) {
			*state = s;
			return result;
		}
		s++;
	} while (!SEP_TEST);
	/* 4. Terminate token and skip trailing separators */
	*s = '\0';
	do {
		s++;
	} while (SEP_TEST);
	/* 5. Return token */
	*state = s;
	return result;
}

void * gdCalloc (size_t nmemb, size_t size)
{
	return calloc (nmemb, size);
}

void *
gdMalloc (size_t size)
{
	return malloc (size);
}

void *
gdRealloc (void *ptr, size_t size)
{
	return realloc (ptr, size);
}

void *
gdReallocEx (void *ptr, size_t size)
{
	void *newPtr = gdRealloc (ptr, size);
	if (!newPtr && ptr)
		gdFree(ptr);
	return newPtr;
}

BGD_DECLARE(void) gdFree (void *ptr)
{
	free (ptr);
}


#ifdef LENGTH_LIMIT
# define STRXCASECMP_FUNCTION strncasecmp
# define STRXCASECMP_DECLARE_N , size_t n
# define LENGTH_LIMIT_EXPR(Expr) Expr
#else
# define STRXCASECMP_FUNCTION strcasecmp
# define STRXCASECMP_DECLARE_N /* empty */
# define LENGTH_LIMIT_EXPR(Expr) 0
#endif


#define TOLOWER(Ch) (isupper (Ch) ? tolower (Ch) : (Ch))

/* Compare {{no more than N characters of }}strings S1 and S2,
   ignoring case, returning less than, equal to or
   greater than zero if S1 is lexicographically less
   than, equal to or greater than S2.  */

int
STRXCASECMP_FUNCTION (const char *s1, const char *s2 STRXCASECMP_DECLARE_N)
{
  register const unsigned char *p1 = (const unsigned char *) s1;
  register const unsigned char *p2 = (const unsigned char *) s2;
  unsigned char c1, c2;

  if (p1 == p2 || LENGTH_LIMIT_EXPR (n == 0))
    return 0;

  do
    {
      c1 = TOLOWER (*p1);
      c2 = TOLOWER (*p2);

      if (LENGTH_LIMIT_EXPR (--n == 0) || c1 == '\0')
	break;

      ++p1;
      ++p2;
    }
  while (c1 == c2);

  return c1 - c2;
}