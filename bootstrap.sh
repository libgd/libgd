#!/bin/sh --
# $Id$
# Small shell script to build gd from source

# Generate the manual (unless naturaldocs isn't installed).  Source
# dists should include the docs so that end users don't need to
# install naturaldocs.  At the same time, we tolerate it being missing
# so that random hackers don't need it just to build the code.
if which naturaldocs > /dev/null; then
    echo "Generation user docs:"
    (cd docs/naturaldocs; bash run_docs.sh)
else
    echo "Can't find naturaldocs; not generating user manual."
fi


# allow importing from the environment, e.g.
# "AUTOCONF=autoconf259 ... ./bootstrap.sh"
if echo $OSTYPE | grep -q '^darwin' ; then 
	echo Having trouble on OS X? Try brew install autoconf libtool automake gettext apple-gcc42 pkg-config cmake
	LIBTOOLIZE=${LIBTOOLIZE:-glibtoolize}
fi
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
