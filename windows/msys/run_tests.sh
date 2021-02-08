#!/bin/bash

set -e

# Parameters
DLLPATH_EXTRA=$1        # Path to supporting DLLs
CFLAGS_EXTRA=$2         # Extra C flags


LOG=run_tests.log

CFLAGS=( -g -Igdtest/ -I. -I../src/ -D_WIN32 -DHAVE_SYS_STAT_H )
if [[ -n "${CFLAGS_EXTRA}" ]]; then
	# shellcheck disable=SC2206
	CFLAGS+=( ${CFLAGS_EXTRA} )
fi
LDFLAGS=( -L../src -llibgd )
DLLPATH=../src:$DLLPATH_EXTRA

run_gcc() {
	local msg
	if msg=$(gcc "$@" 2>&1); then
		true
	else
		echo "COMMAND: gcc $*" >> "${LOG}"
		echo "${msg}" >> "${LOG}"
		false
	fi
}

# Switch to the working directory
export PATH=$PATH:$DLLPATH
cd ../../tests

# Initial setup
echo "Setting up..."
rm -f "${LOG}"
[ -f test_config.h ] || echo '#define GDTEST_TOP_DIR "."' > test_config.h
run_gcc -c "${CFLAGS[@]}" gdtest/gdtest.c


echo "Running tests:"
count=0
failures=0
compile_failures=0
for test in $(find . -name '*.c' | grep -vE '^./(fontconfig|gdtest|gdhelpers|xpm)'); do
	: $(( count += 1 ))

	exe=${test%.c}.exe
	if run_gcc -o "${exe}" "${CFLAGS[@]}" "${LDFLAGS[@]}" "${test}" gdtest.o; then
		true
	else
		echo "COMPILE_FAIL: $test"
		: $(( compile_failures += 1 ))
		continue
	fi

	echo "Running $exe:" >> "${LOG}"
	if $exe >> "${LOG}" 2>&1; then
		echo "PASS: $test"
	else
		: $(( failures += 1 ))
		echo "FAIL: $test"
	fi
	echo >> "${LOG}"
done

echo "$failures failures and $compile_failures compile failures out of $count tests."
echo "Error messages in ${LOG}"
