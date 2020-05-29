# Change Log
All notable changes to this project will be documented in this file.

The format is based on [Keep a Changelog](http://keepachangelog.com/)
and this project adheres to [Semantic Versioning](http://semver.org/).

## [2.3.0] - 2020-03-22

### Security

- Potential double-free in gdImage*Ptr(). (CVE-2019-6978)
- gdImageColorMatch() out of bounds write on heap. (CVE-2019-6977)
- Uninitialized read in gdImageCreateFromXbm(). (CVE-2019-11038)
- Double-free in gdImageBmp. (CVE-2018-1000222)
- Potential NULL pointer dereference in gdImageClone(). (CVE-2018-14553)
- Potential infinite loop in gdImageCreateFromGifCtx(). (CVE-2018-5711)

### Fixed

* Fix #597: add codecov support

- Fix #596: gdTransformAffineCopy run error

- Fix #589: Install dependencies move to .travis.yml

- Fix #586: gdTransformAffineCopy() segfaults on palette images
- Fix #585: gdTransformAffineCopy() changes interpolation method
- Fix #584: gdImageSetInterpolationMethod(im, GD_DEFAULT) inconsistent
- Fix #583: gdTransformAffineCopy() may use unitialized values
- Fix #533: Remove cmake modules
- Fix #539: Add RAQM support for cmake
- Fix #499: gdImageGifAnimAddPtr: heap corruption with 2 identical images
- Fix #486: gdImageCropAuto(…, GD_CROP_SIDES) crops left but not right
- Fix #485: auto cropping has insufficient precision
- Fix #479: Provide a suitable malloc function to liq
- Fix #474: libtiff link returns 404 HTTP code
- Fix #450: Failed to open 1 bit per pixel bitmap
- Fix #440: new_width & new_height exception handling
- Fix #432: gdImageCrop neglecting transparency
- Fix #420: Potential infinite loop in gdImageCreateFromGifCtx
- Fix #411: gd_gd.c format documentation appears to be incorrect
- Fix #369: Fix new_a init error in gdImageConvolution()
- Fix #351: gdImageFilledArc() doesn't properly draw pies
- Fix #338: Fatal and normal libjpeg/libpng errors not distinguishable
- Fix #169: Update var type to hold bigger w&h for ellipse
- Fix #164: update doc files install directory in CMakeLists.txt
- Correct some test depend errors
- Update cmake min version to 3.7
- Delete libimagequant source code download action in CMakeLists.txt
- Improve msys support
- Fix some logic error in CMakeLists.txt
- Remove the following macro: HAVE_STDLIB_H, HAVE_STRING_H, HAVE_STDDEF_H, HAVE_LIMITS_H, HAVE_ERRNO_H, AC_C_CONST

### Added

- test cases for following API: gdImageCopyResized(), gdImageWebpEx(), gdImageCreateFromGd2PartPtr(),  gdImageCloneMatch(), gdImageColorClosestHWB(), gdImageColorMatch(), gdImageStringUp(), gdImageStringUp16(), gdImageString(), gdImageString16(), gdImageCopyMergeGray(), gdImageCopyMerge()

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
