# Experimental Vector2D API

GD 2.4 provides an always-built, experimental vector drawing API in
`gd_vector2d.h`. Its API and ABI may change before it is declared stable.

Create a context with `gdContextCreateForImage()`. The destination must be a
truecolor `gdImage`; palette destinations are rejected. A context borrows the
destination image, snapshots its pixels when created, and writes results back
on `gdContextFlushImage()` or `gdContextDestroy()`. The image must outlive the
context. Mutating it while the context exists is unsupported and those changes
may be overwritten.

Image patterns are created with `gdPathPatternCreateForImage()`, or installed
directly with `gdContextSetSourceImage()`. Palette and truecolor sources are
accepted. Pattern pixels are copied into an independent premultiplied snapshot,
so the source image can subsequently be changed or destroyed.

The implementation supports paths, transforms, fills, strokes and dashes,
clipping, linear and radial gradients, image patterns, opacity, and the 29
Porter-Duff and blend operators. The internal render buffer and surface types
are not public GD 2.4 API.

## Corrected legacy compositing

The experimental corrected legacy compositor is disabled by default. Enable
it with either:

- CMake: `-DENABLE_CORRECTED_LEGACY_COMPOSITING=ON`
- Autotools: `--enable-corrected-legacy-compositing`

When enabled, `gdAlphaBlend()`, `gdLayerOverlay()`, `gdLayerMultiply()`, and
pixel effects routed through them use the normalized Vector2D compositor.
This makes their output build-dependent. Copy and resampling paths retain their
historical behavior. With the option disabled, existing GD APIs retain the GD
2.4 compositing implementation and golden output.

## Building the tests

Enable tests with `-DBUILD_TEST=ON`. A shared-only build produces the public
API test at `build/Bin/test_vector2d_image_context`. The structural, raster,
compositor, and gradient tests exercise private implementation details and are
built when the static library is enabled:

```sh
cmake -S . -B build -DBUILD_TEST=ON -DBUILD_STATIC_LIBS=ON
cmake --build build
ctest --test-dir build -R vector2d --output-on-failure
```
