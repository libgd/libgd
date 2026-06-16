#!/bin/bash
# generate_test_images.sh — regenerate committed JXL test images
# Run manually from tests/jxl/ directory. Not run at build time.
set -euo pipefail

SCRIPT_DIR="$(cd "$(dirname "$0")" && pwd)"
CJXL="${SCRIPT_DIR}/../../specs/jpgxl/tools/cjxl"
SRC="${SCRIPT_DIR}/../../specs/jpgxl/test_images"
CONF="${SCRIPT_DIR}/../../specs/jpgxl/jxl-conformance"

# --- Generated from source PNGs ---
"$CJXL" "$SRC/jxl/flower/flower_small.rgb.png"   flower_small_rgb.jxl           -d 1.0 -e 7
"$CJXL" "$SRC/jxl/flower/flower_small.rgb.png"   flower_small_rgb_lossless.jxl  -d 0   -e 7
"$CJXL" "$SRC/jxl/flower/flower_small.rgba.png"  flower_small_rgba.jxl          -d 1.0 -e 7
"$CJXL" "$SRC/palette/358colors.png"              palette_358.jxl                -d 1.0 -e 7

# --- Copied from conformance / pre-existing ---
cp "$SRC/jxl/pq_gradient.jxl"                                 pq_gradient.jxl
cp "$CONF/edge-cases/cropped_traffic_light.jxl"                cropped_traffic_light.jxl
cp "$CONF/edge-cases/basic.jxl"                                basic.jxl
cp "$CONF/edge-cases/3x3_srgb_lossless.jxl"                    3x3_srgb_lossless.jxl
cp "$CONF/edge-cases/3x3_srgb_lossy.jxl"                       3x3_srgb_lossy.jxl
cp "$CONF/features/alpha_distance_1_0.jxl"                     alpha_distance_1_0.jxl
cp "$CONF/features/hdr_pq_test.jxl"                            hdr_pq_test.jxl
cp "$CONF/features/animation_distance_0_0.jxl"                 animation_distance_0_0.jxl
cp "$CONF/features/animation_distance_1_0.jxl"                 animation_distance_1_0.jxl

echo "Done. All test images regenerated."
