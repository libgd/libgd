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

static gdCallocf gdMemCallocf = calloc;
static gdMallocf gdMemMallocf = malloc;
static gdReallocf gdMemReallocf = realloc;
static gdFreef gdMemFreef = free;

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
	return gdMemCallocf (nmemb, size);
}

void *
gdMalloc (size_t size)
{
	return gdMemMallocf (size);
}

void *
gdRealloc (void *ptr, size_t size)
{
	return gdMemReallocf (ptr, size);
}

void *
gdReallocEx (void *ptr, size_t size)
{
	void *newPtr = gdRealloc (ptr, size);
	if (!newPtr && ptr)
		gdFree(ptr);
	return newPtr;
}

/*
  Function: gdFree

    Frees memory that has been allocated by libgd functions.

	Unless more specialized functions exists (for instance, <gdImageDestroy>),
	all memory that has been allocated by public libgd functions has to be
	freed by calling <gdFree>, and not by free(3), because libgd internally
	doesn't use alloc(3) and friends but rather its own allocation functions,
	which are, however, not publicly available.

  Parameters:

	ptr - Pointer to the memory space to free. If it is NULL, no operation is
		  performed.

  Returns:

	Nothing.
*/
BGD_DECLARE(void) gdFree (void *ptr)
{
	gdMemFreef (ptr);
}

/*
  Function gdSetMemoryCallocMethod

    <gdSetMemoryCallocMethod> sets the memory allocation (initialized with
    zeroes) method used by GD.

    This function is not thread-safe. Should be called once and before any
    operation with the library.

  Parameters:

    callocf - Function pointer to the memory allocation method. If it is NULL,
              no operation is performed.

  Returns:

    Nothing.
*/
BGD_DECLARE(void) gdSetMemoryCallocMethod(gdCallocf callocf)
{
	if (callocf != NULL)
		gdMemCallocf = callocf;
}

/*
  Function gdSetMemoryMallocMethod

    <gdSetMemoryMallocMethod> sets the memory allocation method used by GD.

    This function is not thread-safe. Should be called once and before any
    operation with the library.

  Parameters:

    mallocf - Function pointer to the memory allocation method. If it is NULL,
              no operation is performed.

  Returns:

    Nothing.

  Example:
    (start code)

	// ... set a custom malloc first before any operation ...
	gdSetMemoryMallocMethod(customMalloc);

    gdImagePtr im;
    FILE *in;
    in = fopen("mypng.png", "rb");
    im = gdImageCreateFromPng(in);
    fclose(in);
    gdImageDestroy(im);

    (end code)
*/
BGD_DECLARE(void) gdSetMemoryMallocMethod(gdMallocf mallocf)
{
	if (mallocf != NULL)
		gdMemMallocf = mallocf;
}

/*
  Function gdSetMemoryReallocMethod

    <gdSetMemoryReallocMethod> sets the memory allocation method used by GD.

    This function is not thread-safe. Should be called once and before any
    operation with the library.

  Parameters:

    reallocf - Function pointer to the memory reallocation method. If it is
               NULL, no operation is performed.

  Returns:

    Nothing.
*/
BGD_DECLARE(void) gdSetMemoryReallocMethod(gdReallocf reallocf)
{
	if (reallocf != NULL)
		gdMemReallocf = reallocf;
}

/*
  Function gdSetMemoryFreeMethod

    <gdSetMemoryFreeMethod> sets the memory release method used by GD.

    This function is not thread-safe. Should be called once and before any
    operation with the library.

  Parameters:

    freef - Function pointer to the memory release method. If it is NULL, no
            operation is performed.

  Returns:

    Nothing.
*/
BGD_DECLARE(void) gdSetMemoryFreeMethod(gdFreef freef)
{
	if (freef != NULL)
		gdMemFreef = freef;
}

/*
  Function gdSetMemoryAllocationMethods

    <gdSetMemoryAllocationMethods> sets the memory allocation methods used by
    GD.

    This function is not thread-safe. Should be called once and before any
    operation with the library.

  Parameters:

    callocf - Function pointer to the memory allocation method, initialized
              with zeroes. If it is NULL, the assignation is not performed.
    mallocf - Function pointer to the memory allocation method. If it is NULL,
              the assignation is not performed.
    reallocf - Function pointer to the memory reallocation method. If it is
               NULL, the assignation is not performed.
    freef - Function pointer to the memory release method. If it is NULL, the
            assignation is not performed.

  Returns:

    Nothing.
*/
BGD_DECLARE(void) gdSetMemoryAllocationMethods(gdCallocf callocf, gdMallocf mallocf, gdReallocf reallocf, gdFreef freef)
{
	if (callocf != NULL)
		gdMemCallocf = callocf;
	if (mallocf != NULL)
		gdMemMallocf = mallocf;
	if (reallocf != NULL)
		gdMemReallocf = reallocf;
	if (freef != NULL)
		gdMemFreef = freef;
}
