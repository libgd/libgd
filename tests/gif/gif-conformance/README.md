# GIF Conformance Test Suite

Test corpus for GIF decoder validation. All files generated from scratch using
only Python stdlib — no external tools or libraries.

**License:** Generated test data, public domain / CC0.

## GIF Format Overview

GIF (Graphics Interchange Format) uses LZW-compressed indexed color images with
up to 256 palette entries per frame. Two versions exist:

- **GIF87a** — Original format. Images only, no extensions.
- **GIF89a** — Adds extension blocks: Graphics Control (animation/transparency),
  Application (Netscape looping), Comment, Plain Text.

Binary structure:
```
Header (6B)           "GIF89a" or "GIF87a"
Logical Screen Desc   width(2LE) height(2LE) packed(1) bg_color(1) aspect(1)
Global Color Table    3 * 2^(N+1) bytes (optional)
[Extension blocks]    GCE, Application, Comment, Plain Text
[Image blocks]        Image Descriptor + optional LCT + LZW data
Trailer (1B)          0x3B
```

LZW compression uses variable-width codes starting at `max(2, bit_depth) + 1`
bits, with clear code = `2^min_code_size` and EOI = clear + 1. Maximum code
width is 12 bits (4096 table entries).

**LZW patent note:** The Unisys LZW patent (US 4,558,302) expired in 2003 (US)
and 2004 (worldwide). LZW is now freely usable.

## Regenerating

```bash
python3 generate.py
```

Requires Python 3.10+ (for `X | Y` type union syntax). No external dependencies.

## File Inventory

### valid/ (28 files)

#### Static Images

| File | Size | Description |
|------|------|-------------|
| `static_4x4_red.gif` | 4x4 | Single frame, solid red, 2-color palette |
| `static_8x8_palette.gif` | 8x8 | 8-color diagonal stripe pattern |
| `static_256colors.gif` | 16x16 | All 256 palette entries used, one per pixel |
| `static_interlaced.gif` | 8x8 | Single interlaced frame, 4-color horizontal bands |

#### Animation

| File | Size | Description |
|------|------|-------------|
| `anim_2frame.gif` | 8x8 | 2 frames alternating red/blue, 500ms delay each |
| `anim_3frame_rgb.gif` | 8x8 | 3 frames cycling red/green/blue |
| `anim_10frame.gif` | 8x8 | 10 frames fading black to white |

#### Disposal Methods

All use 8x8 canvas with two 4x8 sub-frames (left half, right half).

| File | Disposal | Expected Behavior |
|------|----------|-------------------|
| `dispose_unspecified.gif` | 0 | Implementation-defined; typically same as 1 |
| `dispose_none.gif` | 1 | Do not dispose; frame persists under next frame |
| `dispose_background.gif` | 2 | Restore covered area to background color |
| `dispose_previous.gif` | 3 | Restore to canvas state before frame was drawn |

#### Transparency

| File | Size | Description |
|------|------|-------------|
| `transparent_bg.gif` | 8x8 | Checkerboard with index 0 transparent via GCE |
| `transparent_frame.gif` | 8x8 | First frame solid red, second has transparent pixels |

#### Timing

| File | Delay | Description |
|------|-------|-------------|
| `delay_0.gif` | 0cs | Zero delay — display as fast as possible |
| `delay_10ms.gif` | 1cs | Minimum non-zero delay (10ms) |
| `delay_1s.gif` | 100cs | One second delay |
| `variable_delay.gif` | mixed | 4 frames: 100ms, 500ms, 1000ms, 2000ms |

#### Looping (Netscape Application Extension)

| File | Loop Count | Expected Behavior |
|------|------------|-------------------|
| `loop_infinite.gif` | 0 | Loop forever |
| `loop_once.gif` | 1 | Play animation twice (1 repeat after initial) |
| `loop_3.gif` | 3 | Play animation 4 times (3 repeats after initial) |
| `no_loop_ext.gif` | (none) | No Netscape extension; play once per GIF87a behavior |

#### Color Tables

| File | Description |
|------|-------------|
| `global_ct_only.gif` | Two frames, both use global color table |
| `local_ct.gif` | Second frame overrides GCT with local color table (orange) |
| `mixed_ct.gif` | Three frames: GCT, LCT, GCT |

#### Canvas/Frame Geometry

| File | Canvas | Description |
|------|--------|-------------|
| `small_frame_big_canvas.gif` | 16x16 | Single 4x4 frame at offset (6,6) |
| `overlapping_frames.gif` | 16x16 | Two 8x8 frames at (0,0) and (4,4), overlapping |

#### Minimum Viable

| File | Description |
|------|-------------|
| `1x1.gif` | Smallest possible GIF: 1x1 pixel, 43 bytes |
| `2color.gif` | Minimum 2-color palette, checkerboard pattern |

### invalid/ (7 files)

| File | Defect | Expected Decoder Behavior |
|------|--------|---------------------------|
| `bad_magic.gif` | Header is "GIF90a" | Reject: unrecognized version |
| `truncated_header.gif` | Only 4 bytes ("GIF8") | Reject: incomplete header |
| `truncated_lzw.gif` | LZW stream cut short | Reject or partial decode with error |
| `empty.gif` | 0 bytes | Reject: no data |
| `no_trailer.gif` | Missing 0x3B trailer | Accept gracefully or reject |
| `bad_lzw_code.gif` | Corrupted LZW code bytes | Reject or partial decode with error |
| `zero_dimensions.gif` | Width=0, Height=0 | Reject: zero-size image |

### edge-cases/ (4 files)

| File | Description | Notes |
|------|-------------|-------|
| `gif87a.gif` | GIF87a format, no extensions | Decoders must accept both 87a and 89a |
| `comment_ext.gif` | Contains comment extension block | Comment text: "This is a GIF conformance test file." |
| `plain_text_ext.gif` | Contains plain text extension | Text: "Hello GIF!" — rarely rendered by modern decoders |
| `large_palette_small_image.gif` | 256-entry palette, only indices 0 and 255 used | Tests palette handling with sparse usage |
