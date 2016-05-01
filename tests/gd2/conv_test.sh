#!/bin/sh -x
base=${0%.sh}
input="${base}.gd2"
exp="${base}_exp.png"
exec ./gd2/gd2_read "${input}" "${exp}"
