# Change Log
All notable changes to this project will be documented in this file.

The format is based on [Keep a Changelog](http://keepachangelog.com/)
and this project adheres to [Semantic Versioning](http://semver.org/).

## [2.2.5] - 2017-08-30

### Security
- Double-free in gdImagePngPtr(). (CVE-2017-6362)
- Buffer over-read into uninitialized memory. (CVE-2017-7890)

### Fixed
- Fix #109: XBM reading fails with printed error
- Fix #338: Fatal and normal libjpeg/ibpng errors not distinguishable
- Fix #357: 2.2.4: Segfault in test suite
- Fix #386: gdImageGrayScale() may produce colors
- Fix #406: webpng -i removes the transparent color
- Fix Coverity #155475: Failure to restore alphaBlendingFlag
- Fix Coverity #155476: potential resource leak
- Fix several build issues and test failures
- Fix and reenable optimized support for reading 1 bps TIFFs

### Added
- The native MSVC buildchain now supports libtiff and most executables

## [2.2.4] - 2017-01-18

### Security
- gdImageCreate() doesn't check for oversized images and as such is
  prone to DoS vulnerabilities. (CVE-2016-9317)
- double-free in gdImageWebPtr() (CVE-2016-6912)
- potential unsigned underflow in gd_interpolation.c
- DOS vulnerability in gdImageCreateFromGd2Ctx()

### Fixed
- Fix #354: Signed Integer Overflow gd_io.c
- Fix #340: System frozen
- Fix OOB reads of the TGA decompression buffer
- Fix DOS vulnerability in gdImageCreateFromGd2Ctx()
- Fix potential unsigned underflow
- Fix double-free in gdImageWebPtr()
- Fix invalid read in gdImageCreateFromTiffPtr()
- Fix OOB reads of the TGA decompression buffer
- Fix #68: gif: buffer underflow reported by AddressSanitizer
- Avoid potentially dangerous signed to unsigned conversion
- Fix #304: test suite failure in gif/bug00006 [2.2.3]
- Fix #329: GD_BILINEAR_FIXED gdImageScale() can cause black border
- Fix #330: Integer overflow in gdImageScaleBilinearPalette()
- Fix 321: Null pointer dereferences in gdImageRotateInterpolated
- Fix whitespace and add missing comment block
- Fix #319: gdImageRotateInterpolated can have wrong background color
- Fix color quantization documentation
- Fix #309: gdImageGd2() writes wrong chunk sizes on boundaries
- Fix #307: GD_QUANT_NEUQUANT fails to unset trueColor flag
- Fix #300: gdImageClone() assigns res_y = res_x
- Fix #299: Regression regarding gdImageRectangle() with gdImageSetThickness()
- Replace GNU old-style field designators with C89 compatible initializers
- Fix #297: gdImageCrop() converts palette image to truecolor image
- Fix #290: TGA RLE decoding is broken
- Fix unnecessary non NULL checks
- Fix #289: Passing unrecognized formats to gdImageGd2 results in corrupted files
- Fix #280: gdImageWebpEx() `quantization` parameter is a misnomer
- Publish all gdImageCreateFromWebp*() functions and gdImageWebpCtx()
- Fix issue #276: Sometimes pixels are missing when storing images as BMPs
- Fix issue #275: gdImageBmpCtx() may segfault for non-seekable contexts
- Fix copy&paste error in gdImageScaleBicubicFixed()

### Added
- More documentation
- Documentation on GD and GD2 formats
- More tests
