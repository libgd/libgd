|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

To build GD using CMake, all you need is (add the cmake options if you need any
or if the default libs and include paths are not the default) :

In a sub directory of the top level:
$ mkdir bld
$ cd bld
$ cmake -DBUILD_TEST=1 <options> ..
$ make

The available options are:
ENABLE_GD_FORMATS=1
ENABLE_PNG=1
ENABLE_LIQ=1
ENABLE_JPEG=1
ENABLE_TIFF=1
ENABLE_ICONV=1
ENABLE_FREETYPE=1
ENABLE_FONTCONFIG=1
ENABLE_XPM=1
ENABLE_WEBP=1
ENABLE_RAQM=1

You can optionally run our tests suite using:
$ ctest .

Or if you like to build in the source tree:
$ cmake -DBUILD_TEST=1 .
$ make
$ ctest .

Add "-DCMAKE_BUILD_TYPE=DEBUG" if you like to have a debug version.

To install, just run:
$ make install
Or to to install to a different path:
$ make install DESTDIR=/some/other/place

Typical usage with only PNG enabled:

This command generates VS7 project files and add some custom libraries and
includes directories (libpng in this case):

cmake  -DCMAKE_GENERATOR="Visual Studio 7 .NET 2003"  -DENABLE_PNG=1 -DCMAKE_LIBRARY_PATH=c:\gd_build\gd_deps\lib -DCMAKE_INCLUDE_PATH=c:\gd_build\gd_deps\include c:\libgd_src

A typical CMake call to build using the MinGW tools:

cmake -G"MinGW Makefiles" -DCMAKE_LIBRARY_PATH=c:\mingw\lib -DCMAKE_INCLUDE_PATH=c:\MinGW\include -DENABLE_JPEG=On -DBUILD_TEST=On -DCMAKE_RELEASE_TYPE=DEBUG ..\libgd_head

Given that your MinGW install is under c:\mingw and that you install all libraries using the /usr prefix
