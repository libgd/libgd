#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "gd.h"
#include "gdhelpers.h"
#include <stdlib.h>
#include <string.h>

#include <sys/types.h>
#include <ctype.h>

static struct gdAllocHelpers_s {
  gdCallocMethod gdMemoryCallocMethod;
  gdMallocMethod gdMemoryMallocMethod;
  gdReallocMethod gdMemoryReallocMethod;
  gdFreeMethod gdMemoryFreeMethod;
} gdAllocHelpers = {
  calloc,
  malloc,
  realloc,
  free,
};

/* TBB: gd_strtok_r is not portable; provide an implementation */

#define SEP_TEST (separators[*((unsigned char *) s)])

char *
gd_strtok_r(char *s, const char *sep, char **state)
{
	char separators[256];
	char *result = 0;
	memset (separators, 0, sizeof (separators));
	while (*sep) {
		separators[*((const unsigned char *) sep)] = 1;
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

BGD_DECLARE(void *)
gdCalloc (size_t nmemb, size_t size)
{
	return gdAllocHelpers.gdMemoryCallocMethod (nmemb, size);
}

BGD_DECLARE(void *)
gdMalloc (size_t size)
{
	return gdAllocHelpers.gdMemoryMallocMethod (size);
}

BGD_DECLARE(void *)
gdRealloc (void *ptr, size_t size)
{
	return gdAllocHelpers.gdMemoryReallocMethod (ptr, size);
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
	gdAllocHelpers.gdMemoryFreeMethod (ptr);
}

/*
  Function: gdSetMemoryCallocMethod

    <gdSetMemoryCallocMethod> sets the memory allocation (initialized with
    zeroes) method used by GD.

    This function is not thread-safe. Should be called once and before any
    operation with the library.

  Parameters:

    calloc_method - Function pointer to the memory allocation method. If it is
                    NULL, no operation is performed.

  Returns:

    Nothing.

  See also:
    - <gdClearMemoryCallocMethod>
*/
BGD_DECLARE(void) gdSetMemoryCallocMethod(gdCallocMethod calloc_method)
{
	if (calloc_method != NULL)
		gdAllocHelpers.gdMemoryCallocMethod = calloc_method;
}

/*
  Function: gdSetMemoryMallocMethod

    <gdSetMemoryMallocMethod> sets the memory allocation method used by GD.

    This function is not thread-safe. Should be called once and before any
    operation with the library.

  Parameters:

    malloc_method - Function pointer to the memory allocation method. If it is
                    NULL, no operation is performed.

  Returns:

    Nothing.

  See also:
    - <gdClearMemoryMallocMethod>

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
BGD_DECLARE(void) gdSetMemoryMallocMethod(gdMallocMethod malloc_method)
{
	if (malloc_method != NULL)
		gdAllocHelpers.gdMemoryMallocMethod = malloc_method;
}

/*
  Function: gdSetMemoryReallocMethod

    <gdSetMemoryReallocMethod> sets the memory allocation method used by GD.

    This function is not thread-safe. Should be called once and before any
    operation with the library.

  Parameters:

    realloc_method - Function pointer to the memory reallocation method. If it
                     is NULL, no operation is performed.

  Returns:

    Nothing.

  See also:
    - <gdClearMemoryReallocMethod>
*/
BGD_DECLARE(void) gdSetMemoryReallocMethod(gdReallocMethod realloc_method)
{
	if (realloc_method != NULL)
		gdAllocHelpers.gdMemoryReallocMethod = realloc_method;
}

/*
  Function: gdSetMemoryFreeMethod

    <gdSetMemoryFreeMethod> sets the memory release method used by GD.

    This function is not thread-safe. Should be called once and before any
    operation with the library.

  Parameters:

    free_method - Function pointer to the memory release method. If it is NULL,
                  no operation is performed.

  Returns:

    Nothing.
*/
BGD_DECLARE(void) gdSetMemoryFreeMethod(gdFreeMethod free_method)
{
	if (free_method != NULL)
		gdAllocHelpers.gdMemoryFreeMethod = free_method;
}

/*
  Function: gdClearMemoryCallocMethod

    <gdClearMemoryCallocMethod> clears the memory allocation (initialized with
    zeroes) method used by GD.

    This function is not thread-safe and it should not be called during
    operations.

  Returns:

    Nothing.

  See also:
    - <gdSetMemoryCallocMethod>
*/
BGD_DECLARE(void) gdClearMemoryCallocMethod()
{
	gdAllocHelpers.gdMemoryCallocMethod = calloc;
}

/*
  Function: gdClearMemoryMallocMethod

    <gdClearMemoryMallocMethod> clears the memory allocation method used by GD.

    This function is not thread-safe and it should not be called during
    operations.

  Returns:

    Nothing.

  See also:
    - <gdSetMemoryMallocMethod>

  Example:
    (start code)

    gdSetMemoryMallocMethod(customMalloc);

    // ... do the desired operations ...

    // ... clear the custom malloc so it uses the default one ...
    gdClearMemoryMallocMethod();

    (end code)
*/
BGD_DECLARE(void) gdClearMemoryMallocMethod()
{
	gdAllocHelpers.gdMemoryMallocMethod = malloc;
}

/*
  Function: gdClearMemoryReallocMethod

    <gdClearMemoryReallocMethod> clears the memory allocation method used by
    GD.

    This function is not thread-safe and it should not be called during
    operations.

  Returns:

    Nothing.

  See also:
    - <gdSetMemoryReallocMethod>
*/
BGD_DECLARE(void) gdClearMemoryReallocMethod()
{
	gdAllocHelpers.gdMemoryReallocMethod = realloc;
}

/*
  Function: gdClearMemoryFreeMethod

    <gdSetMemClearFreeMethod> clears the memory release method used by GD.

    This function is not thread-safe and it should not be called during
    operations.

  Returns:

    Nothing.

  See also:
    - <gdSetMemoryFreeMethod>
*/
BGD_DECLARE(void) gdClearMemoryFreeMethod()
{
	gdAllocHelpers.gdMemoryFreeMethod = free;
}
