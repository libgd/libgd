# GD Graphics (Draw) Library
[![Build Status](https://scan.coverity.com/projects/3810/badge.svg)](https://scan.coverity.com/projects/libgd)
[![Chat](https://badges.gitter.im/libgd/libgd.svg)](https://gitter.im/libgd/libgd)
[![codecov.io](https://codecov.io/github/libgd/libgd/coverage.svg?branch=master)](https://codecov.io/github/libgd/libgd/)

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

If you like to contribute, report bugs, see [how to contribute document](CONTRIBUTING.md)

For security related issues, please contact us at security@libgd.org

Support available in [![Chat](https://badges.gitter.im/libgd/libgd.svg)](https://gitter.im/libgd/libgd) or using issues.

We also have a mailing list. To subscribe to any mailing list, send an email to gd-devel-subscribe@lists.php.net. Then emails can be sent to gd-devel@lists.php.net.

## Downloads/etc...

Please visit our [homepage](https://www.libgd.org/) for more details.

## Supported Image Formats

GD has support for:

* [WebP](https://en.wikipedia.org/wiki/WebP) via [libwebp](https://developers.google.com/speed/webp/)
* [JPEG](https://en.wikipedia.org/wiki/JPEG) via [IJG/libjpeg](http://www.ijg.org/) or [libjpeg-turbo](http://libjpeg-turbo.virtualgl.org/)
  * Does not include [JPEG 2000](https://en.wikipedia.org/wiki/JPEG_2000)
* [PNG](https://en.wikipedia.org/wiki/Portable_Network_Graphics) via [libpng](http://www.libpng.org/)
* [AVIF](https://en.wikipedia.org/wiki/AV1#AV1_Image_File_Format_(AVIF)) via [libavif](https://github.com/AOMediaCodec/libavif)
  * This includes [AVIF](https://en.wikipedia.org/wiki/AV1#AV1_Image_File_Format_%28AVIF%29) read support if your system's `libheif` has AV1 decoding.
* [HEIF](https://en.wikipedia.org/wiki/High_Efficiency_Image_File_Format) via [libheif](https://github.com/strukturag/libheif/)
* [TIFF](https://en.wikipedia.org/wiki/Tagged_Image_File_Format) via [libtiff](http://www.libtiff.org/)
* [BMP](https://en.wikipedia.org/wiki/BMP_file_format) (builtin)
* [GIF](https://en.wikipedia.org/wiki/GIF) (builtin)
* [TGA](https://en.wikipedia.org/wiki/Truevision_TGA) (builtin)
* [WBMP](https://en.wikipedia.org/wiki/Wireless_Application_Protocol_Bitmap_Format) (builtin)
* [XPM](https://en.wikipedia.org/wiki/X_PixMap) via [libXpm](http://xorg.freedesktop.org/)

Besides that, GD depends on some external libraries, which are all optional
and disabled by default:

* [FreeType](https://freetype.org) for rendering fonts
* [Fontconfig](https://fontconfig.org) for configuring and customizing font access
* [libraqm](https://github.com/HOST-Oman/libraqm) for complex text layout
* [libimagequant](https://pngquant.org/lib) for conversion of RGBA images to 8-bit indexed-color images
  * **NOTE** libimagequant is dual-licensed: GPLv3 and commercial license


## Platforms supported

CI means whether we have an automatic CI for this platform. If someone has CI for these platforms or any other platforms not listed here and would like to add them to our automatic CI, please get in touch with us, it will much appreciated!

| Platform  | Support | CI |
| ------------- | ------------- |----|
| Linux x64  | &#10003;  | &#10003; |
| Linux x86  | &#10003;  | &#10003; |
| Linux ARM64  | &#10003; | &#10003; |
| Windows x86  | &#10003;  | &#10003; |
| Windows x64  | &#10003;  | &#10003; |
| Windows arm64  | &#10003;  | x |
| MacOS x64  | &#10003;  | &#10003; |
| MacOS M1  | &#10003;  | x |
| S390  | &#10003;  | x |

It is also known to work on almost all variations of *BSD, Solaris, etc. We don't have CI nor environment to test them. However many progamming languages binding do test libgd on these platforms.

## Compilers

It should compile with all C99 and C++ compliant compilers, either using CMake or the configure script.

We do have CI using:
- GCC
- CLang
- Visual Studio
- Xcode
- MingW
