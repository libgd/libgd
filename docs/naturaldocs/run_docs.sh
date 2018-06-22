#!/bin/sh

set -e

usage() {
	echo "Usage: run_docs.sh [--nonfatal]"
	echo "Generate the manual for gd!"
	if [ $# -ne 0 ] ; then
		printf 'ERROR: %b\n' "$*"
		exit 1
	else
		exit 0
	fi
}

nd() {
	# Figure out the name of this tool.
	if which naturaldocs 2>/dev/null ; then
		return
	elif which NaturalDocs 2>/dev/null ; then
		return
	else
		return 1
	fi
}

cd "$(dirname "$0")"

if [ $# -eq 1 ] ; then
	case $1 in
	--nonfatal)
		if ! nd >/dev/null ; then
			echo "skipping doc generation -- please install naturaldoc!"
			exit 0
		fi
		;;
	-h|--help)
		usage
		;;
	*)
		usage "unknown option: $1"
		;;
	esac
elif [ $# -ne 0 ] ; then
	usage "unknown options: $*"
fi

# Version number
VERSION=`(cd ../../; perl config/getver.pl)`

# Clear away old docs and ensure the doc dir. is present.
rm -rf html
mkdir html

# Create a lightly-processed copy of the source to use as input.  This
# file skips all non-C code in src/ and removes the BGD_DECLARE()
# macro from definitions so they don't show up in the docs.
rm -rf tmp
mkdir tmp
perl nobgd.pl ../../src/ tmp/

# Add the external docs.
echo "Title: License" | cat - ../../COPYING > tmp/license.txt
sed -e "s/@VERSION@/$VERSION/g" preamble.txt > tmp/preamble.txt
# ^^^ hack to get the version number in the docs.

# Run naturaldocs to create the manual.
$(nd) --rebuild --rebuild-output --documented-only \
    -i tmp/ \
    -img images/ \
    -o html html  \
    --project project/ \
    -s Default libgd

# And cleanup the temp files.
rm -rf Data tmp
