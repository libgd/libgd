#!/bin/bash
# set $1 to 1 to force an x86 build
# set $2 to 1 to rebuild and statically link any deps
# ex: _build 0 1 would build an x64 static (on x64 machines)

# Env vars used by script:
# BUILD_TEST=1
# ENABLE_PNG=1
# ENABLE_JPEG=1
# ENABLE_FREETYPE=1
# ENABLE_ICONV=0
# ENABLE_TIFF=1

_build()
{
  echo "*building* (x86=${1-0}; static=${2-0})"
  
  pack="*.a *.so*"
  cargs="-fPIC"
  cmargs=()
  [ ${1-0} -gt 0 ] && cargs="$cargs -m32" || cmargs+=(-D64BIT=1)
  [ ${1-0} -gt 0 ] && [ ${2-0} -gt 0 ] && [ $ENABLE_ICONV == 1 ] && cargs="$cargs -DHAVE_ICONV_T_DEF=1"
  cmargs+=(-DCMAKE_C_FLAGS="$cargs" -DCMAKE_CXX_FLAGS="$cargs" -DCMAKE_PREFIX_PATH=$(pwd)/deps)
  
  if [ ${2-0} -gt 0 ]; then
    mkdir deps
    
    git clone https://github.com/imazen/zlib
    cd zlib
    cmake -G "Unix Makefiles" "${cmargs[@]}"
    make zlib_static
    cp libz.a ../deps
    cp *.h ../deps
    cd ..
    
    if [ $ENABLE_PNG == 1 ]; then
      git clone https://github.com/imazen/libpng
      cd libpng
      cmake -G "Unix Makefiles" "${cmargs[@]}"
      make png_static
      cp libpng.a ../deps
      cp *.h ../deps
      cd ..
    fi

    if [ $ENABLE_JPEG == 1 ]; then
      sudo apt-get install nasm
      git clone https://github.com/imazen/libjpeg-turbo
      cd libjpeg-turbo
      cmake -G "Unix Makefiles" "${cmargs[@]}"
      make
      cp libjpeg.a ../deps
      cp *.h ../deps
      cd ..
    fi

    if [ $ENABLE_FREETYPE == 1 ]; then
      git clone https://github.com/imazen/freetype
      cd freetype
      cmake -G "Unix Makefiles" "${cmargs[@]}"
      make freetype_static
      cp libfreetype.a ../deps
      cp -r include/* ../deps
      cd ..
    fi

    if [ $ENABLE_TIFF == 1 ]; then
      git clone https://github.com/imazen/libtiff
      cd libtiff
      cmake -G "Unix Makefiles" "${cmargs[@]}"
      make tiff_static
      cp libtiff/libtiff.a ../deps
      cp libtiff/*.h ../deps
      cp headers/*.h ../deps
      cd ..
    fi

    if [ $ENABLE_ICONV == 1 ]; then
      git clone https://github.com/imazen/libiconv
      cd libiconv/source
      CFLAGS="-fPIC"
      [ ${1-0} -gt 0 ] && CFLAGS="$CFLAGS -m32"
      export CFLAGS
      sh ./configure --enable-static
      make
      cp lib/.libs/libiconv.a ../../deps
      cp include/iconv.h ../../deps
      cd ../..
      CFLAGS=
      export CFLAGS
    fi
    
    pack="$pack ../deps/*.a"
  fi
  
  cmake -G "Unix Makefiles" "${cmargs[@]}" -DBUILD_TEST=$BUILD_TEST -DENABLE_PNG=$ENABLE_PNG -DENABLE_JPEG=$ENABLE_JPEG -DENABLE_FREETYPE=$ENABLE_FREETYPE -DENABLE_ICONV=$ENABLE_ICONV -DENABLE_TIFF=$ENABLE_TIFF -Wno-dev .
  make
  ctest .
  cd Bin
  objdump -f *.so | grep ^architecture
  ldd *.so
  find . -maxdepth 1 -type l -exec rm -f {} \;
  rm -f libgdTest.a
  tar -zcf ../binaries.tar.gz $pack
  cd ..
}


_clean()
{
  echo "*cleaning*"
  git clean -ffde /out > /dev/null
  git reset --hard > /dev/null
  rm CMakeCache.txt
  rm -rf CMakeFiles
  rm -rf deps
}

mkdir out

_build
mv binaries.tar.gz out/libgd-x64.tar.gz
_clean

_build 0 1
mv binaries.tar.gz out/libgd-static-x64.tar.gz
_clean

sudo apt-get -y update > /dev/null
sudo apt-get -y install gcc-multilib > /dev/null
sudo apt-get -y install g++-multilib > /dev/null
sudo apt-get -y install libjpeg-dev:i386 > /dev/null
sudo apt-get -y install libtiff-dev:i386 > /dev/null
sudo apt-get -y install libfreetype6-dev:i386 > /dev/null
sudo ldconfig -n /lib/i386-linux-gnu/
for f in $(find /lib/i386-linux-gnu/*.so.*); do sudo ln -s -f $f ${f%%.*}.so; done > /dev/null
sudo cp /usr/include/x86_64-linux-gnu/jconfig.h /usr/include/jconfig.h

_build 1
mv binaries.tar.gz out/libgd-x86.tar.gz
_clean

_build 1 1
mv binaries.tar.gz out/libgd-static-x86.tar.gz
_clean
