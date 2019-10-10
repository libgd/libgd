/*	$NetBSD: getline.c,v 1.0 2019/10/09 16:00:22 christos Exp $	*/

/*
 * Copyright (c) 1987, 1993, 1994
 *	The Regents of the University of California.  All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name of the University nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE REGENTS AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

#include <stdio.h>
#include "gd.h"
#include "gdhelpers.h"

/*
 * getline from stdio
 */
BGD_DECLARE(size_t) getline(char **lineptr, size_t *n, FILE *stream)
{
	size_t count = 0;
	int c;
	char *newLineptr;

	if (!*lineptr) {
		*n = (*n > 0) ? *n : BUFSIZ;
		*lineptr = (char*)gdCalloc(*n, sizeof(char));
	}

	if (!*lineptr) {
		return -1;
	}

	while ((c = fgetc(stream)) != EOF) {
		count++;

		if (*n <= count) {
			*n = *n * 2 > count ? *n * 2 : count * 2;
			newLineptr = (char*)gdReallocEx(*lineptr, *n);
			if (!newLineptr) {
				return -1;
			}

			*lineptr = newLineptr;
		}

		*(*lineptr + count - 1) = c;
		*(*lineptr + count) = '\0';

		if (c == '\n') {
			return count;
		}
	}

	return -1;

}
