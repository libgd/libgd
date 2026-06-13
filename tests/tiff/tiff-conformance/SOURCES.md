# TIFF Conformance — Sources & Attribution

## image-tiff (tiff-rs) test images

- **Source**: https://github.com/image-rs/image-tiff
- **License**: MIT
- **Files in `valid/`**:
  `12bit.cropped.rgb.tiff`, `12bit.cropped.tiff`, `big_g4.tif`,
  `cmyk-3c-16b.tiff`, `cmyk-3c-32b-float.tiff`, `cmyk-3c-8b.tiff`, `cmyk-4c-8b.tiff`,
  `extra_bits_gray_8b.tiff`, `extra_bits_rgb_8b.tiff`,
  `fax4.tiff`,
  `gradient-1c-32b-float.tiff`, `gradient-1c-32b.tiff`, `gradient-1c-64b-float.tiff`, `gradient-1c-64b.tiff`,
  `gradient-3c-32b-float.tiff`, `gradient-3c-32b.tiff`, `gradient-3c-64b.tiff`,
  `imagemagick_group4.tiff`,
  `int16_rgb.tif`, `int16.tif`, `int16_zstd.tif`, `int8_rgb.tif`, `int8.tif`,
  `issue_69_lzw.tiff`, `issue_69_packbits.tiff`,
  `logluv-3c-16b.tiff`,
  `minisblack-1c-16b.tiff`, `minisblack-1c-8b.tiff`, `minisblack-1c-i16b.tiff`, `minisblack-1c-i8b.tiff`,
  `minisblack-2c-8b-alpha.tiff`, `miniswhite-1c-1b.tiff`,
  `no_rows_per_strip.tiff`,
  `palette-1c-1b.tiff`, `palette-1c-4b.tiff`, `palette-1c-8b.tiff`,
  `planar-rgb-u8.tif`,
  `predictor-3-gray-f32.tif`, `predictor-3-rgb-f32.tif`,
  `quad-lzw-compat.tiff`, `quad-tile.jpg.tiff`,
  `random-fp16-pred2.tiff`, `random-fp16-pred3.tiff`, `random-fp16.tiff`,
  `rgb-3c-16b.tiff`, `rgb-3c-8b.tiff`,
  `single-black-fp16.tiff`,
  `tiled-cmyk-i8.tif`, `tiled-gray-i1.tif`, `tiled-jpeg-rgb-u8.tif`, `tiled-jpeg-ycbcr.tif`,
  `tiled-oversize-gray-i8.tif`, `tiled-rect-rgb-u8.tif`, `tiled-rgb-u8.tif`,
  `white-fp16-pred2.tiff`, `white-fp16-pred3.tiff`, `white-fp16.tiff`
- **Files in `valid/` (BigTIFF)**:
  `BigTIFF.tif`, `BigTIFFLong.tif`, `BigTIFFMotorola.tif`
- **Files in `edge-cases/`**:
  `geo-5b.tif`, `usda_naip_256_webp_z3.tif` (USDA NAIP imagery, Public Domain)
- **Files in `robustness/`**:
  `sample-get-lzw-stuck.tiff`

## libtiffpic (libtiff sample pictures)

- **Source**: https://download.osgeo.org/libtiff/
- **License**: CC0
- **Files in `valid/`**:
  `caspian.tif`, `cramps.tif`, `cramps-tile.tif`,
  `dscf0013.tif`,
  `fax2d.tif`,
  `flower-minisblack-02.tif` through `flower-minisblack-32.tif` (10 files),
  `flower-palette-02.tif` through `flower-palette-16.tif` (4 files),
  `flower-rgb-contig-02.tif` through `flower-rgb-contig-32.tif` (9 files),
  `flower-rgb-planar-02.tif` through `flower-rgb-planar-32.tif` (9 files),
  `flower-separated-contig-08.tif`, `flower-separated-contig-16.tif`,
  `flower-separated-planar-08.tif`, `flower-separated-planar-16.tif`,
  `g3test.tif`,
  `jello.tif`,
  `jim___ah.tif`, `jim___cg.tif`, `jim___dg.tif`, `jim___gg.tif`,
  `ladoga.tif`,
  `off_l16.tif`, `off_luv24.tif`, `off_luv32.tif`,
  `oxford.tif`,
  `pc260001.tif`,
  `quad-jpeg.tif`, `quad-lzw.tif`, `quad-tile.tif`,
  `smallliz.tif`,
  `strike.tif`,
  `text.tif`,
  `ycbcr-cat.tif`,
  `zackthecat.tif`

## libtiff test images

- **Source**: https://gitlab.com/libtiff/libtiff (`test/images/`)
- **License**: libtiff license (permissive BSD-like)

  > Copyright (c) 1988-1997 Sam Leffler, Copyright (c) 1991-1997 Silicon Graphics, Inc.
  > Permission to use, copy, modify, distribute, and sell this software and its documentation
  > for any purpose is hereby granted without fee.

- **Files in `valid/`** (unique to libtiff, not already present from image-tiff):
  `32bpp-None-jpeg.tiff`, `32bpp-None.tiff`,
  `custom_dir_EXIF_GPS.tiff`,
  `deflate-last-strip-extra-data.tiff`,
  `lzw-single-strip.tiff`,
  `ojpeg_chewey_subsamp21_multi_strip.tiff`,
  `ojpeg_single_strip_no_rowsperstrip.tiff`,
  `ojpeg_zackthecat_subsamp22_single_strip.tiff`,
  `testfax3_bug_513.tiff`, `testfax3_bug54_1dnoEOL.tif`,
  `test_float64_predictor2_be_lzw.tif`, `test_float64_predictor2_le_lzw.tif`,
  `webp_lossless_rgba_alpha_fully_opaque.tif`
- **Files in `edge-cases/`**:
  `test_two_ifds.tif`, `tiff_with_subifd_chain.tif`
- **Files in `robustness/`**:
  `test_ifd_loop_subifd.tif`, `test_ifd_loop_to_first.tif`, `test_ifd_loop_to_self.tif`

## image-rs test images

- **Source**: https://github.com/image-rs/image (`tests/images/tiff/`)
- **License**: MIT
- **Files in `valid/`**:
  `hpredict.tiff`, `hpredict_cmyk.tiff`, `hpredict_packbits.tiff`,
  `l1.tiff`, `l1_xmp.tiff`,
  `mandrill.tiff`,
  `rgb32f_bw.tiff`, `rgb32f_color.tiff`

## Kodak benchmark images

- **Source**: Eastman Kodak Company
- **License**: Released for unrestricted use
- **Files**: `kodim02-lzw.tif`, `kodim07-lzw.tif`

## Other

- `Transparency-lzw.tif`: Public Domain (Wikimedia)
- `subsubifds.tif`: CC0 (via [tifftools](https://github.com/DigitalSlideArchive/tifftools))
- `usda_naip_256_webp_z3.tif`: USDA NAIP on AWS, Public Domain
