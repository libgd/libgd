#!/bin/bash -e

if [[ -z "${top_srcdir}" ]] ; then
	cd "${0%/*}/../.."
	top_srcdir="${PWD}"
fi
TEST_DIR="${top_srcdir}/tests/source"

cd "${top_srcdir}"

ret=0

# Force a basic environment regardless of how we're configured.
export GIT_PAGER=cat
export LC_ALL=C

# List all the non-binary files we know about in the tree.
read -r -d'\n' -a files < <(
	git ls-tree -r --name-only HEAD | \
		grep -Ev '\.(avif|bin|bmp|gd|gd2|gif|heic|jpg|jpeg|png|pic|sgi|tga|tiff|ttf|xbm|xpm)$'
) || :

banner() {
	echo
	echo "### $*"
}

do_grep() {
	# Use -a so UTF-8 files don't get flagged as binary & skipped.
	if git grep -aHnE "$@" "${files[@]}" ; then
		ret=1
	fi
}

# Run some tests.

banner "Check for trailing whitespace."
do_grep '[[:space:]]+$'

banner "Check for Windows line endings."
do_grep $'\r$'

banner "Checking trailing lines."
"${TEST_DIR}"/whitespace.py "${files[@]}"

exit ${ret}
