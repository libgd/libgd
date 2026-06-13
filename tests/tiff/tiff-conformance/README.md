# TIFF Conformance Test Suite

154 TIFF files covering comprehensive feature combinations for decoder conformance and robustness testing.

| Folder | Files | Size | Purpose |
|--------|-------|------|---------|
| `valid/` | 145 | 12 MB | MUST decode correctly |
| `edge-cases/` | 5 | 64 KB | Uncommon but valid features (SubIFDs, multi-page, GeoTIFF) |
| `robustness/` | 4 | 24 KB | Malformed files — MUST reject gracefully |

## Feature Coverage

### Color Types

- **Grayscale (MinIsBlack)**: 8-bit, 16-bit, signed i8/i16
- **Grayscale + Alpha**: 8-bit
- **MinIsWhite**: 1-bit
- **RGB**: 8, 10, 12, 14, 16, 24, 32-bit (contiguous and planar)
- **CMYK (Separated)**: 8-bit, 16-bit (contiguous and planar; 3c and 4c variants)
- **Palette**: 1, 2, 4, 8, 16-bit
- **YCbCr**: 8-bit
- **LogLuv**: 16, 24, 32-bit

### Bit Depths

1, 2, 4, 6, 8, 10, 12, 14, 16, 24, 32, 64-bit integer and floating-point (f16, f32, f64)

### Compression

| Compression | Files |
|-------------|-------|
| Uncompressed | `32bpp-None.tiff`, `minisblack-*`, `rgb-3c-*`, many `flower-*` |
| LZW | `quad-lzw.tif`, `kodim*-lzw.tif`, `issue_69_lzw.tiff`, `lzw-single-strip.tiff` |
| Deflate (Zip) | `deflate-last-strip-extra-data.tiff` |
| PackBits | `issue_69_packbits.tiff`, `hpredict_packbits.tiff` |
| CCITT Group 3 | `g3test.tif`, `fax2d.tif`, `testfax3_bug_513.tiff` |
| CCITT Group 4 | `fax4.tiff`, `big_g4.tif`, `imagemagick_group4.tiff` |
| JPEG (new-style) | `quad-jpeg.tif`, `quad-tile.jpg.tiff`, `tiled-jpeg-*` |
| Old JPEG | `ojpeg_*.tiff` |
| Zstd | `int16_zstd.tif` |
| WebP | `webp_lossless_rgba_alpha_fully_opaque.tif`, `usda_naip_256_webp_z3.tif` |
| LogLuv | `logluv-3c-16b.tiff`, `off_luv24.tif`, `off_luv32.tif` |

### Organization

- **Stripped**: Most files
- **Tiled** (square): `tiled-rgb-u8.tif`, `cramps-tile.tif`, `quad-tile.tif`
- **Tiled** (rectangular): `tiled-rect-rgb-u8.tif`
- **Tiled** (oversized): `tiled-oversize-gray-i8.tif`
- **Planar** (separate planes): `planar-rgb-u8.tif`, `flower-rgb-planar-*`, `flower-separated-planar-*`

### Predictors

- **None**: Most files
- **Horizontal (predictor=2)**: `hpredict.tiff`, `hpredict_cmyk.tiff`, `test_float64_predictor2_*.tif`
- **Floating-point (predictor=3)**: `predictor-3-*.tif`, `white-fp16-pred3.tiff`, `random-fp16-pred3.tiff`

### Byte Order

- **Little-endian (II)**: Most files
- **Big-endian (MM)**: `BigTIFFMotorola.tif`, `test_float64_predictor2_be_lzw.tif`

### Format

- **Classic TIFF**: Most files
- **BigTIFF**: `BigTIFF.tif`, `BigTIFFMotorola.tif`, `BigTIFFLong.tif`

### Other Features

- **EXIF/GPS directories**: `custom_dir_EXIF_GPS.tiff`
- **XMP metadata**: `l1_xmp.tiff`
- **Extra samples**: `extra_bits_rgb_8b.tiff`, `extra_bits_gray_8b.tiff`
- **Half-precision float (f16)**: `white-fp16*.tiff`, `random-fp16*.tiff`, `single-black-fp16.tiff`
- **No RowsPerStrip**: `no_rows_per_strip.tiff`
- **12-bit cropped**: `12bit.cropped.tiff`, `12bit.cropped.rgb.tiff`

### Edge Cases

- **Multi-page (two IFDs)**: `test_two_ifds.tif`
- **SubIFDs**: `subsubifds.tif`, `tiff_with_subifd_chain.tif`
- **GeoTIFF**: `geo-5b.tif`, `usda_naip_256_webp_z3.tif`

### Robustness

- **IFD loops**: `test_ifd_loop_to_self.tif`, `test_ifd_loop_to_first.tif`, `test_ifd_loop_subifd.tif`
- **LZW decompression**: `sample-get-lzw-stuck.tiff`

## Sources

See [SOURCES.md](SOURCES.md) for per-file attribution and licensing.

## Licenses

| Source | License |
|--------|---------|
| image-tiff test images | MIT |
| libtiffpic | CC0 |
| libtiff test images | libtiff (permissive, see SOURCES.md) |
| image-rs test images | MIT |
| Kodak benchmark images | Unrestricted |
| USDA NAIP imagery | Public Domain |
| subsubifds.tif | CC0 |
