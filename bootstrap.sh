#!/bin/sh --
# $Id$
# Small shell script to build gd from source

# allow importing from the environment, e.g.
# "AUTOCONF=autoconf259 ... ./bootstrap.sh"
ACLOCAL=${ACLOCAL:-aclocal}
AUTOCONF=${AUTOCONF:-autoconf}
AUTOHEADER=${AUTOHEADER:-autoheader}
AUTOMAKE=${AUTOMAKE:-automake}
LIBTOOLIZE=${LIBTOOLIZE:-libtoolize}

# might handle this differently
AUTOMAKE_FLAGS="--add-missing --copy"

#
CLEANFILES="Makefile.in aclocal.m4 autom4te.cache configure libtool config/Makefile.in \
tests/Makefile.in src/Makefile.in"

#
rm -rf ${CLEANFILES}

#
if ${ACLOCAL} -I m4 \
   && ${LIBTOOLIZE} --automake --copy --force \
   && ${ACLOCAL} -I m4 \
   && ${AUTOHEADER} \
   && ${AUTOMAKE} ${AUTOMAKE_FLAGS} \
   && ${AUTOCONF} --force && [ -f configure ]
then
  echo Now run configure and make
else
  echo Failed
  exit 1
fi

exit 0
