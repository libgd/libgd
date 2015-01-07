#!/bin/bash

# THe Ultimate Make Bash Script
# Used to wrap build scripts for easy dep
# handling and multiplatform support


# Basic usage on *nix:
# export tbs_arch=x86
# ./thumbs.sh make


# On Win (msvc 2013):
# C:\Program Files (x86)\Microsoft Visual Studio 12.0\VC\vcvarsall x86_amd64
# SET tbs_tools=msvc12
# thumbs make

# On Win (mingw32):
# SET path=C:\mingw32\bin;%path%
# SET tbs_tools=mingw
# SET tbs_arch=x86
# thumbs make


# Global settings are stored in env vars
# Should be inherited

[ $tbs_conf ]           || export tbs_conf=Release
[ $tbs_arch ]           || export tbs_arch=x64
[ $tbs_tools ]          || export tbs_tools=gnu
[ $tbs_static_runtime ] || export tbs_static_runtime=0

[ $tbs_gd_png ]         || export tbs_gd_png=1
[ $tbs_gd_jpeg ]        || export tbs_gd_jpeg=1
[ $tbs_gd_freetype ]    || export tbs_gd_freetype=1
[ $tbs_gd_tiff ]        || export tbs_gd_tiff=1


# tbsd_* contains dep related settings
# tbsd_[name]_* contains settings specific to the dep
# name should match the repo name

# deps contains a map of what should be built/used
# keep the keys in sync ... no assoc arrays on msys :/
# targ contains a target for each dep (default=empty str)
# post is executed after each thumbs dep build
# ^ used for copying/renaming any libs you need - uses eval

zname=zlib.lib
jname=jpeg.lib
pname=png.lib
tname=tiff.lib
fname=freetype.lib

if [ $tbs_tools = gnu -o $tbs_tools = mingw ]
then
  zname=libz.a
  jname=libjpeg.a
  pname=libpng.a
  tname=libtiff.a
  fname=libfreetype.a
fi

deps=()
targ=()
post=()

[ $tbsd_zlib_repo ]          || export tbsd_zlib_repo="https://github.com/imazen/zlib"
[ $tbsd_libpng_repo ]        || export tbsd_libpng_repo="https://github.com/imazen/libpng"
[ $tbsd_libjpeg_turbo_repo ] || export tbsd_libjpeg_turbo_repo="https://github.com/imazen/libjpeg-turbo libjpeg_turbo"
[ $tbsd_libtiff_repo ]       || export tbsd_libtiff_repo="https://github.com/imazen/libtiff"
[ $tbsd_freetype_repo ]      || export tbsd_freetype_repo="https://github.com/imazen/freetype"

if [[ "$OSTYPE" == "darwin"* ]]; then cp="rsync"
else cp="cp"
fi

deps+=(zlib); targ+=(zlibstatic)
post+=("$cp -u \$(./thumbs.sh list_slib) ../../deps/$zname")

if [ $tbs_gd_png -gt 0 ]; then
  deps+=(libpng); targ+=(png16_static)
  post+=("$cp -u \$(./scripts/thumbs.sh list_slib) ../../deps/$pname")
fi

if [ $tbs_gd_jpeg -gt 0 ]; then
  deps+=(libjpeg_turbo); targ+=(jpeg_static)
  post+=("for lib in \$(./thumbs.sh list_slib); do [ -f \$lib ] && $cp -u \$lib ../../deps/$jname; done")
fi

if [ $tbs_gd_tiff -gt 0 ]; then
  ttarg="libtiff/tiff_static"
  [ $tbs_tools = gnu -o $tbs_tools = mingw ] && ttarg=tiff_static
  deps+=(libtiff); targ+=($ttarg)
  post+=("$cp -u \$(./thumbs.sh list_slib) ../../deps/$tname")
fi

if [ $tbs_gd_freetype -gt 0 ]; then
  deps+=(freetype); targ+=("freetype_static")
  post+=("$cp -u \$(./thumbs.sh list_slib) ../../deps/$fname")
fi



# -----------
# dep processor

process_deps()
{
  mkdir build_deps
  mkdir deps
  cd build_deps

  for key in "${!deps[@]}"
  do
    dep=${deps[$key]}
    i_dep_repo="tbsd_${dep}_repo"
    i_dep_incdir="tbsd_${dep}_incdir"
    i_dep_libdir="tbsd_${dep}_libdir"
    i_dep_built="tbsd_${dep}_built"
    
    [ ${!i_dep_built} ] || export "${i_dep_built}=0"
    
    if [ ${!i_dep_built} -eq 0 ]
    then
      git clone ${!i_dep_repo} --depth 1
      cd $dep || exit 1
      
      thumbs="./thumbs.sh"
      [ ! -f $thumbs ] && thumbs=$(find . -name thumbs.sh -maxdepth 2)
      
      $thumbs make ${targ[$key]} || exit 1
      
      # copy any includes and do poststep
      $cp -u -r $($thumbs list_inc) ../../deps
      eval ${post[$key]}
      
      # look in both local and parent dep dirs
      export "${i_dep_incdir}=../../deps;deps"
      export "${i_dep_libdir}=../../deps;deps"
      export "${i_dep_built}=1"
      
      cd ..
    fi
  done
  
  export "${i_dep_incdir}=../../deps/freetype;deps/freetype"
  
  cd ..
}

# -----------
# constructs dep dirs for cmake

postproc_deps()
{
  cm_inc=
  cm_lib=
  
  for dep in "${deps[@]}"
  do
    i_dep_incdir="tbsd_${dep}_incdir"
    i_dep_libdir="tbsd_${dep}_libdir"
    
    cm_inc="${!i_dep_incdir};$cm_inc"
    cm_lib="${!i_dep_libdir};$cm_lib"
  done
  
  cm_args+=(-DCMAKE_LIBRARY_PATH=$cm_lib)
  cm_args+=(-DCMAKE_INCLUDE_PATH=$cm_inc)
}

# -----------

if [ $# -lt 1 ]
then
  echo ""
  echo " Usage : ./thumbs [command]"
  echo ""
  echo " Commands:"
  echo "   make      - builds everything"
  echo "   check     - runs tests"
  echo "   clean     - removes build files"
  echo "   list      - echo paths to any interesting files"
  echo "               space separated; relative"
  echo "   list_bin  - echo binary paths"
  echo "   list_inc  - echo lib include files"
  echo "   list_slib - echo static lib path"
  echo "   list_dlib - echo dynamic lib path"
  echo ""
  exit
fi

# -----------

upper()
{
  echo $1 | tr [:lower:] [:upper:]
}

# Local settings

l_inc="./src/gd.h ./src/gd_io.h ./src/gdfx.h"
l_slib=
l_dlib=
l_bin=
list=

make=
c_flags=
cm_tools=
cm_args=(-DCMAKE_BUILD_TYPE=$tbs_conf)
cm_args+=(-DBUILD_TEST=1)
cm_args+=(-DENABLE_PNG=$tbs_gd_png)
cm_args+=(-DENABLE_JPEG=$tbs_gd_jpeg)
cm_args+=(-DENABLE_FREETYPE=$tbs_gd_freetype)
cm_args+=(-DENABLE_TIFF=$tbs_gd_tiff)

target=
[ $2 ] && target=$2

# -----------

case "$tbs_tools" in
msvc12)
  cm_tools="Visual Studio 12"
  [ "$target" = "" ] && mstrg="GD.sln" || mstrg="$target.vcxproj"
  make="msbuild.exe $mstrg //p:Configuration=$tbs_conf //v:m"
  
  l_slib="./build/Bin/$tbs_conf/libgd_static.lib"
  l_dlib="./build/Bin/$tbs_conf/libgd.lib"
  l_bin="./build/Bin/$tbs_conf/libgd.dll"
  list="$l_bin $l_slib $l_dlib $l_inc" ;;
gnu)
  cm_tools="Unix Makefiles"
  c_flags+=" -fPIC"
  make="make $target"
  l_slib="./build/Bin/libgd.a"
  l_dlib="./build/Bin/libgd.so"
  l_bin="$l_dlib"
  list="$l_slib $l_dlib $l_inc" ;;
mingw)
  cm_tools="MinGW Makefiles"
  make="mingw32-make $target"
  c_flags+=" -fkeep-inline-functions"
  
  # allow sh in path; some old cmake/mingw bug?
  cm_args+=(-DCMAKE_SH=)
  
  l_slib="./build/Bin/liblibgd.a"
  l_dlib="./build/Bin/liblibgd.dll.a"
  l_bin="./build/Bin/liblibgd.dll"
  list="$l_bin $l_slib $l_dlib $l_inc" ;;

*) echo "Tool config not found for $tbs_tools"
   exit 1 ;;
esac

# -----------

case "$tbs_arch" in
x64)
  [ $tbs_tools = msvc12 ] && cm_tools="$cm_tools Win64"
  [ $tbs_tools = gnu -o $tbs_tools = mingw ] && c_flags+=" -m64" ;;
x86)
  [ $tbs_tools = gnu -o $tbs_tools = mingw ] && c_flags+=" -m32" ;;

*) echo "Arch config not found for $tbs_arch"
   exit 1 ;;
esac

# -----------

if [ $tbs_static_runtime -gt 0 ]
then
  [ $tbs_tools = msvc12 ] && c_flags+=" /MT"
  [ $tbs_tools = gnu ] && cm_args+=(-DCMAKE_SHARED_LINKER_FLAGS="-static-libgcc -static-libstdc++")
  [ $tbs_tools = mingw ] && cm_args+=(-DCMAKE_SHARED_LINKER_FLAGS="-static")
fi

# -----------

case "$1" in
make)
  process_deps
  postproc_deps
  
  mkdir build
  cd build
  
  cm_args+=(-DCMAKE_C_FLAGS_$(upper $tbs_conf)="$c_flags")
  cm_args+=(-DCMAKE_CXX_FLAGS_$(upper $tbs_conf)="$c_flags")
  
  cmake -G "$cm_tools" "${cm_args[@]}" .. || exit 1
  $make || exit 1
  
  cd ..
  ;;
  
check)
  cd build
  ctest -C $tbs_conf . || exit 1
  cd ..
  ;;
  
clean)
  rm -rf deps
  rm -rf build_deps
  rm -rf build
  ;;

list) echo $list;;
list_bin) echo $l_bin;;
list_inc) echo $l_inc;;
list_slib) echo $l_slib;;
list_dlib) echo $l_dlib;;

*) echo "Unknown command $1"
   exit 1;;
esac
