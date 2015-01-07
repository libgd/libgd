#!/bin/bash

mkdir out
sudo apt-get update -qq > /dev/null
sudo apt-get -y install nasm > /dev/null

./thumbs.sh make || exit 1
./thumbs.sh check || exit 1
objdump -f build/Bin/*.so | grep ^architecture
ldd build/Bin/*.so
tar -zcf out/libgd-x64.tar.gz --transform 's/.*\///' $(./thumbs.sh list)
./thumbs.sh clean

sudo apt-get -y install gcc-multilib > /dev/null
sudo apt-get -y install g++-multilib > /dev/null

export tbs_arch=x86
./thumbs.sh make || exit 1
./thumbs.sh check || exit 1
objdump -f build/Bin/*.so | grep ^architecture
ldd build/Bin/*.so
tar -zcf out/libgd-x86.tar.gz --transform 's/.*\///' $(./thumbs.sh list)
./thumbs.sh clean
