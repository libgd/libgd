#!/bin/sh --
# $Id$
# Small shell script to build gd from CVS

# allow importing from the environment, e.g.
# "AUTOCONF=autoconf259 ... ./bootstrap.sh"
ACLOCAL=${ACLOCAL:-aclocal}
AUTOCONF=${AUTOCONF:-autoconf}
AUTOHEADER=${AUTOHEADER:-autoheader}
AUTOMAKE=${AUTOMAKE:-automake}

# might handle this differently
AUTOMAKE_FLAGS="--add-missing --copy"

#
CLEANFILES="Makefile Makefile.in aclocal.m4 autom4te.cache config.h config.log \
config.status configure libtool config/Makefile config/Makefile.in \
config/gdlib-config tests/Makefile tests/Makefile.in"

#
rm -rf ${CLEANFILES}

#
if ${ACLOCAL} \
   && ${AUTOHEADER} \
   && ${AUTOMAKE} ${AUTOMAKE_FLAGS} \
   && ${AUTOCONF} && [ -f configure ]
then
  echo Now run configure and make
else
  echo Failed
  exit 1
fi

exit 0
