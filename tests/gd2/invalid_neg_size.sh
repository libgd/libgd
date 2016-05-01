#!/bin/sh -x
base=${0%.sh}
input="${base}.gd2"
exec ./gd2/gd2_read_corrupt "${input}"
