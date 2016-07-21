#!/bin/sh
# CMake doesn't provide the equiv of "distclean" which makes it impossible to
# properly clean up after it when you build in-tree.  This script emulates it.
# It assumes it's run in the dir you want to clean.

usage() {
	cat <<-EOF
Usage: $0 [--automake]

Clean all the cmake generated output files.

Options:
  --automake  Do not clean files autotools also creates
EOF
	exit ${1:-0}
}

AUTOMAKE="false"
while [ $# -ne 0 ]; do
	case $1 in
	-h|--help)  usage ;;
	--automake) AUTOMAKE="true" ;;
	*) usage 1 ;;
	esac
	shift
done

set -x

find . -maxdepth 3 \
	'(' \
		-name CMakeCache.txt -o \
		-name CMakeFiles -o \
		-name CTestTestfile.cmake -o \
		-name cmake_install.cmake \
	')' \
	-exec rm -rf {} +
rm -rf \
	Bin Testing \
	CPackConfig.cmake CPackSourceConfig.cmake \
	DartConfiguration.tcl

if [ "${AUTOMAKE}" = "false" ]; then
	find . -maxdepth 3 '!' -wholename './windows/*' -a -name Makefile -exec rm -rf {} +
	rm -f src/config.h
fi
