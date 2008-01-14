|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
$Id$
Tobuild GD using CMake, all you need is (add the cmake options if you need any
or if the default libs and include paths are not the default) :

In a sub directory:
$ mkdir bld
$ cd bld
$ cmake -DBUILD_TEST=1 <options>

The available options are:
ENABLE_PNG=1
ENABLE_JPEG=1
ENABLE_TIFF=1
ENABLE_FREETYPE=1
ENABLE_FONTCONFIG=1
ENABLE_XPM=1

You can optionnally run our tests suite using:
$ ctest .

Or if you like to build in the source tree:
$ cmake -DBUILD_TEST=1 .
$ ctest .

Add "-DCMAKE_BUILD_TYPE=DEBUG" if you like to have a debug version.


