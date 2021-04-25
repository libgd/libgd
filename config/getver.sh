#!/bin/sh

# Simple script to extract the version number parts from src/gd.h.  If
# called with the middle word of the version macro, it prints the
# value of that macro.  If called with no argument, it outputs a
# human-readable version string.  This must be run in the project
# root.  It is used by configure.ac and docs/naturaldocs/run_docs.sh.

TOPDIR="${0%/*}/.."
HEADER="${TOPDIR}/src/gd.h"
SENTINEL="/*version605b5d1778*/"

getpart() {
	awk -v field="GD_${1}_VERSION" -v sentinel="${SENTINEL}" '
		$1 == "#define" && $2 == field && $NF == sentinel {
			gsub(/^"/, "", $3)
			gsub(/"$/, "", $3)
			print $3
		}
	' "${HEADER}"
}

case $# in
0)
	printf '%s.%s.%s%s\n' \
		"$(getpart MAJOR)" \
		"$(getpart MINOR)" \
		"$(getpart RELEASE)" \
		"$(getpart EXTRA)"
	;;
1)
	case $1 in
	MAJOR|MINOR|RELEASE|EXTRA)
		part=$(getpart "$1")
		if [ -n "${part}" ]; then
			printf '%s' "${part}"
		fi
		;;
	*)
		exit 1
		;;
	esac
	;;
*)
	echo "$0: error: script takes at most 1 arg"
	exit 1
	;;
esac
