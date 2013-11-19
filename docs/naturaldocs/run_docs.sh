#!/bin/bash

set -e

# Version number
VERSION=`(cd ../../; perl config/getver.pl)`

# Clear away old docs and ensure the doc dir. is present.
[ -d html ] && rm -rf html
mkdir html

# Create a lightly-processed copy of the source to use as input.  This
# file skips all non-C code in src/ and removes the BGD_DECLARE()
# macro from definitions so they don't show up in the docs.
[ -d tmp ] && rm -rf tmp
mkdir tmp
perl nobgd.pl ../../src/ tmp/

# Add the external docs.
cp license.txt tmp/
sed -e "s/@VERSION@/$VERSION/g" preamble.txt > tmp/preamble.txt
# ^^^ hack to get the version number in the docs.

# Run naturaldocs to create the manual.
naturaldocs --rebuild --rebuild-output --documented-only \
    -i tmp/ \
    -o html html  \
    --project project/

# And cleanup the temp files.
rm -rf Data tmp


