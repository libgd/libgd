#!/usr/bin/env python3
# -*- coding: utf-8 -*-

"""Helper for checking whitespace.

These operations aren't easy to do in pure shell.
"""

from __future__ import print_function

import sys


def check(path):
    with open(path, 'rb') as fp:
        # Skip 0 byte files.
        first_byte = fp.read(1)
        if not first_byte:
            return 0

        # Check for leading blank lines.
        if b'\n' == first_byte:
            print('%s: trim leading blank lines' % (path,), file=sys.stderr)
            return 1

        # Check for missing trailing new line.
        fp.seek(-1, 2)
        if b'\n' != fp.read(1):
            print('%s: missing trailing new line' % (path,), file=sys.stderr)
            return 1
        elif fp.tell() == 1:
            print('%s: dummy file' % (path,), file=sys.stderr)
            return 1

        # Check for trailing blank lines.
        fp.seek(-2, 2)
        if b'\n\n' == fp.read(2):
            print('%s: trim trailing blank lines' % (path,), file=sys.stderr)
            return 1

    return 0


def main(argv):
    ret = 0
    for path in argv:
        ret |= check(path)
    return ret


if __name__ == '__main__':
    sys.exit(main(sys.argv[1:]))
