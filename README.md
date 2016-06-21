# GD Graphics (Draw) Library

[![Build Status](https://travis-ci.org/libgd/libgd.svg?branch=master)](https://travis-ci.org/libgd/libgd)
[![Build Status](https://scan.coverity.com/projects/3810/badge.svg)](https://scan.coverity.com/projects/libgd)

GD is an open source code library for the dynamic creation of images by
programmers.

GD is written in C, and "wrappers" are available for Perl, PHP and other
languages. GD can read and write many different image formats. GD is commonly
used to generate charts, graphics, thumbnails, and most anything else, on the
fly.

The most common applications of GD involve website development, although it
can be used with any standalone application!

The library was originally developed by Thomas Boutell and is now maintained
by many contributors (see the [CONTRIBUTORS](CONTRIBUTORS) file) under the
umbrella of PHP.net.

## Downloads/etc...

Please visit our [homepage](http://www.libgd.org/) for more details.

## Supported Image Formats

GD has builtin support for:

* [BMP](https://en.wikipedia.org/wiki/BMP_file_format)
* [GIF](https://en.wikipedia.org/wiki/GIF)
* [TGA](https://en.wikipedia.org/wiki/Truevision_TGA)
* [WBMP](https://en.wikipedia.org/wiki/Wireless_Application_Protocol_Bitmap_Format)

It also has optional support for more formats via external libraries:

* [JPEG](https://en.wikipedia.org/wiki/JPEG) via [IJG/libjpeg](http://www.ijg.org/) or [libjpeg-turbo](http://libjpeg-turbo.virtualgl.org/)
  * Does not include [JPEG 2000](https://en.wikipedia.org/wiki/JPEG_2000)
* [PNG](https://en.wikipedia.org/wiki/Portable_Network_Graphics) via [libpng](http://www.libpng.org/)
* [TIFF](https://en.wikipedia.org/wiki/Tagged_Image_File_Format) via [libtiff](http://www.remotesensing.org/libtiff/)
* [WebP](https://en.wikipedia.org/wiki/WebP) via [libwebp](https://developers.google.com/speed/webp/)
* [XPM](https://en.wikipedia.org/wiki/X_PixMap) via [libXpm](http://xorg.freedesktop.org/)
