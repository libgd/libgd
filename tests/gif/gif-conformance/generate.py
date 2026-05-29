#!/usr/bin/env python3
"""
Generate GIF conformance test corpus.

Produces valid, invalid, and edge-case GIF files for decoder testing.
Uses only Python stdlib — no external dependencies.

GIF89a binary format reference:
  Header (6B): "GIF89a"
  Logical Screen Descriptor (7B): width(2LE), height(2LE), packed(1), bg_color(1), aspect(1)
  Global Color Table (optional): 3 * 2^(size+1) bytes
  Blocks: Extension blocks, Image Descriptor blocks
  Trailer: 0x3B

LZW compression uses variable-width codes starting at max(2, color_depth) + 1 bits.
"""

import struct
import os
from pathlib import Path

SCRIPT_DIR = Path(__file__).parent
VALID_DIR = SCRIPT_DIR / "valid"
INVALID_DIR = SCRIPT_DIR / "invalid"
EDGE_DIR = SCRIPT_DIR / "edge-cases"


# ---------------------------------------------------------------------------
# Low-level GIF building blocks
# ---------------------------------------------------------------------------

def le16(v: int) -> bytes:
    """Little-endian 16-bit unsigned."""
    return struct.pack("<H", v & 0xFFFF)


def gif_header(version: str = "89a") -> bytes:
    return f"GIF{version}".encode("ascii")


def logical_screen_descriptor(
    width: int, height: int,
    gct_flag: bool = True, color_res: int = 7, sort_flag: bool = False,
    gct_size_field: int = 0, bg_color: int = 0, aspect: int = 0,
) -> bytes:
    packed = 0
    if gct_flag:
        packed |= 0x80
    packed |= (color_res & 7) << 4
    if sort_flag:
        packed |= 0x08
    packed |= gct_size_field & 7
    return le16(width) + le16(height) + bytes([packed, bg_color, aspect])


def color_table(colors: list[tuple[int, int, int]], pad_to: int | None = None) -> bytes:
    """Build a color table. Pad with black to next power of 2 if needed."""
    n = len(colors)
    if pad_to is not None:
        target = pad_to
    else:
        target = 2
        while target < n:
            target *= 2
    data = bytearray()
    for r, g, b in colors:
        data.extend([r & 0xFF, g & 0xFF, b & 0xFF])
    while len(data) < target * 3:
        data.extend([0, 0, 0])
    return bytes(data)


def gct_size_field(num_colors: int) -> int:
    """Return the GCT size field value (0-7) for the given number of colors."""
    n = 2
    field = 0
    while n < num_colors:
        n *= 2
        field += 1
    return min(field, 7)


def color_table_size(field: int) -> int:
    """Number of entries from the size field."""
    return 2 ** (field + 1)


def graphics_control_extension(
    disposal: int = 0, user_input: bool = False,
    transparent: bool = False, delay_cs: int = 0,
    transparent_index: int = 0,
) -> bytes:
    packed = ((disposal & 7) << 2)
    if user_input:
        packed |= 0x02
    if transparent:
        packed |= 0x01
    return (
        b"\x21\xF9"           # Extension introducer + GCE label
        + b"\x04"              # Block size = 4
        + bytes([packed])
        + le16(delay_cs)
        + bytes([transparent_index])
        + b"\x00"              # Block terminator
    )


def netscape_loop_extension(loop_count: int) -> bytes:
    return (
        b"\x21\xFF"            # Extension introducer + Application Extension label
        + b"\x0B"              # Block size = 11
        + b"NETSCAPE2.0"       # Application identifier + auth code
        + b"\x03"              # Sub-block size = 3
        + b"\x01"              # Sub-block ID = 1 (loop)
        + le16(loop_count)
        + b"\x00"              # Block terminator
    )


def comment_extension(text: str) -> bytes:
    data = text.encode("ascii", errors="replace")
    out = bytearray(b"\x21\xFE")  # Extension introducer + Comment label
    # Write in sub-blocks of up to 255 bytes
    i = 0
    while i < len(data):
        chunk = data[i:i + 255]
        out.append(len(chunk))
        out.extend(chunk)
        i += 255
    out.append(0x00)  # Block terminator
    return bytes(out)


def plain_text_extension(
    left: int = 0, top: int = 0, width: int = 8, height: int = 8,
    cell_w: int = 8, cell_h: int = 8, fg: int = 1, bg: int = 0,
    text: str = "Hi",
) -> bytes:
    data = text.encode("ascii")
    out = bytearray(b"\x21\x01")  # Extension introducer + Plain Text label
    out.append(12)  # Block size for the fixed header
    out.extend(le16(left))
    out.extend(le16(top))
    out.extend(le16(width))
    out.extend(le16(height))
    out.append(cell_w)
    out.append(cell_h)
    out.append(fg)
    out.append(bg)
    # Text sub-blocks
    i = 0
    while i < len(data):
        chunk = data[i:i + 255]
        out.append(len(chunk))
        out.extend(chunk)
        i += 255
    out.append(0x00)  # Block terminator
    return bytes(out)


def image_descriptor(
    left: int = 0, top: int = 0, width: int = 0, height: int = 0,
    lct_flag: bool = False, interlace: bool = False,
    sort_flag: bool = False, lct_size_field: int = 0,
) -> bytes:
    packed = 0
    if lct_flag:
        packed |= 0x80
    if interlace:
        packed |= 0x40
    if sort_flag:
        packed |= 0x20
    packed |= lct_size_field & 7
    return b"\x2C" + le16(left) + le16(top) + le16(width) + le16(height) + bytes([packed])


def trailer() -> bytes:
    return b"\x3B"


# ---------------------------------------------------------------------------
# LZW compressor for GIF
# ---------------------------------------------------------------------------

def lzw_compress(pixels: list[int], min_code_size: int) -> bytes:
    """
    GIF-style LZW compression.

    min_code_size: number of bits for raw pixel values (min 2 for GIF).
    Returns the compressed byte stream (sub-blocked) suitable for GIF image data.
    """
    if min_code_size < 2:
        min_code_size = 2

    clear_code = 1 << min_code_size
    eoi_code = clear_code + 1

    # Initialize code table
    def init_table():
        table = {}
        for i in range(clear_code):
            table[(i,)] = i
        return table, clear_code + 2, min_code_size + 1

    code_table, next_code, code_size = init_table()
    max_code = (1 << code_size) - 1

    # Collect output codes
    codes = [clear_code]  # Start with clear code

    if not pixels:
        codes.append(eoi_code)
    else:
        buffer = (pixels[0],)
        for pixel in pixels[1:]:
            candidate = buffer + (pixel,)
            if candidate in code_table:
                buffer = candidate
            else:
                codes.append(code_table[buffer])
                if next_code <= 4095:
                    code_table[candidate] = next_code
                    next_code += 1
                    if next_code > max_code + 1 and code_size < 12:
                        code_size += 1
                        max_code = (1 << code_size) - 1
                elif next_code > 4095:
                    # Table full — emit clear code and reset
                    codes.append(clear_code)
                    code_table, next_code, code_size = init_table()
                    max_code = (1 << code_size) - 1
                buffer = (pixel,)
        codes.append(code_table[buffer])
        codes.append(eoi_code)

    # Pack codes into a bit stream
    bit_buffer = 0
    bits_in_buffer = 0
    raw_bytes = bytearray()

    # We need to track code_size per code for proper bit packing.
    # Re-run the code size logic during packing.
    pack_code_table_size = clear_code + 2
    pack_code_size = min_code_size + 1
    pack_max_code = (1 << pack_code_size) - 1

    for code in codes:
        bit_buffer |= code << bits_in_buffer
        bits_in_buffer += pack_code_size

        while bits_in_buffer >= 8:
            raw_bytes.append(bit_buffer & 0xFF)
            bit_buffer >>= 8
            bits_in_buffer -= 8

        if code == clear_code:
            pack_code_table_size = clear_code + 2
            pack_code_size = min_code_size + 1
            pack_max_code = (1 << pack_code_size) - 1
        elif code != eoi_code:
            pack_code_table_size += 1
            if pack_code_table_size > pack_max_code + 1 and pack_code_size < 12:
                pack_code_size += 1
                pack_max_code = (1 << pack_code_size) - 1
            elif pack_code_table_size > 4095:
                pass  # Will be followed by clear code

    if bits_in_buffer > 0:
        raw_bytes.append(bit_buffer & 0xFF)

    # Sub-block the output
    out = bytearray()
    out.append(min_code_size)  # LZW minimum code size byte
    i = 0
    while i < len(raw_bytes):
        chunk = raw_bytes[i:i + 255]
        out.append(len(chunk))
        out.extend(chunk)
        i += 255
    out.append(0x00)  # Block terminator
    return bytes(out)


def image_data(pixels: list[int], min_code_size: int | None = None, color_depth: int = 8) -> bytes:
    """Compress pixel indices and return GIF image data block."""
    if min_code_size is None:
        min_code_size = max(2, color_depth)
    return lzw_compress(pixels, min_code_size)


def interlace_pixels(pixels: list[int], width: int, height: int) -> list[int]:
    """Re-order pixel rows into GIF interlace order."""
    rows = [pixels[y * width:(y + 1) * width] for y in range(height)]
    result = []
    # Pass 1: rows 0, 8, 16, ...
    for y in range(0, height, 8):
        result.extend(rows[y])
    # Pass 2: rows 4, 12, 20, ...
    for y in range(4, height, 8):
        result.extend(rows[y])
    # Pass 3: rows 2, 6, 10, ...
    for y in range(2, height, 4):
        result.extend(rows[y])
    # Pass 4: rows 1, 3, 5, ...
    for y in range(1, height, 2):
        result.extend(rows[y])
    return result


# ---------------------------------------------------------------------------
# High-level helpers
# ---------------------------------------------------------------------------

def make_gif(
    width: int, height: int,
    frames: list[dict],
    gct: list[tuple[int, int, int]] | None = None,
    bg_color: int = 0,
    loop_count: int | None = None,
    version: str = "89a",
    extensions: list[bytes] | None = None,
    add_trailer: bool = True,
) -> bytes:
    """
    Build a complete GIF file.

    frames: list of dicts with keys:
        pixels: list[int] — color indices
        delay_cs: int — delay in centiseconds (optional, default 0)
        disposal: int — disposal method (optional, default 0)
        transparent: bool (optional)
        transparent_index: int (optional)
        left, top, width, height: int (optional, default to canvas)
        lct: list of (r,g,b) (optional local color table)
        interlace: bool (optional)
    """
    out = bytearray()

    # Header
    out.extend(gif_header(version))

    # Determine GCT
    if gct is not None:
        gct_field = gct_size_field(len(gct))
        num_gct = color_table_size(gct_field)
        color_depth = gct_field + 1
        out.extend(logical_screen_descriptor(
            width, height, gct_flag=True, color_res=gct_field,
            gct_size_field=gct_field, bg_color=bg_color,
        ))
        out.extend(color_table(gct, pad_to=num_gct))
    else:
        # No GCT — frames must have LCTs
        out.extend(logical_screen_descriptor(
            width, height, gct_flag=False, color_res=7,
            gct_size_field=0, bg_color=bg_color,
        ))
        color_depth = 8  # Will be overridden per frame

    # Pre-image extensions
    if extensions:
        for ext in extensions:
            out.extend(ext)

    # Netscape loop extension (before first frame)
    if loop_count is not None:
        out.extend(netscape_loop_extension(loop_count))

    # Frames
    for frame in frames:
        fw = frame.get("width", width)
        fh = frame.get("height", height)
        fl = frame.get("left", 0)
        ft = frame.get("top", 0)
        disposal = frame.get("disposal", 0)
        delay_cs = frame.get("delay_cs", 0)
        transparent = frame.get("transparent", False)
        transparent_index = frame.get("transparent_index", 0)
        interlace = frame.get("interlace", False)
        lct = frame.get("lct", None)
        pixels_data = frame["pixels"]

        # GCE if needed
        has_gce = (disposal != 0 or delay_cs != 0 or transparent or version == "89a")
        if has_gce and version == "89a":
            out.extend(graphics_control_extension(
                disposal=disposal, delay_cs=delay_cs,
                transparent=transparent, transparent_index=transparent_index,
            ))

        # Image Descriptor
        if lct is not None:
            lct_field = gct_size_field(len(lct))
            num_lct = color_table_size(lct_field)
            frame_depth = lct_field + 1
            out.extend(image_descriptor(
                left=fl, top=ft, width=fw, height=fh,
                lct_flag=True, interlace=interlace, lct_size_field=lct_field,
            ))
            out.extend(color_table(lct, pad_to=num_lct))
        else:
            frame_depth = color_depth
            out.extend(image_descriptor(
                left=fl, top=ft, width=fw, height=fh,
                lct_flag=False, interlace=interlace,
            ))

        # Pixel data
        if interlace:
            pixels_data = interlace_pixels(pixels_data, fw, fh)

        min_cs = max(2, frame_depth)
        out.extend(image_data(pixels_data, min_code_size=min_cs, color_depth=frame_depth))

    if add_trailer:
        out.extend(trailer())

    return bytes(out)


def solid_pixels(index: int, width: int, height: int) -> list[int]:
    return [index] * (width * height)


def write(path: Path, data: bytes):
    path.parent.mkdir(parents=True, exist_ok=True)
    path.write_bytes(data)
    print(f"  {path.name} ({len(data)} bytes)")


# ---------------------------------------------------------------------------
# Valid files
# ---------------------------------------------------------------------------

def generate_valid():
    print("Generating valid files...")

    # --- Static images ---

    # static_4x4_red.gif
    gct = [(255, 0, 0), (0, 0, 0)]
    write(VALID_DIR / "static_4x4_red.gif", make_gif(
        4, 4, [{"pixels": solid_pixels(0, 4, 4)}], gct=gct,
    ))

    # static_8x8_palette.gif — 8 colors in a pattern
    colors8 = [
        (255, 0, 0), (0, 255, 0), (0, 0, 255), (255, 255, 0),
        (255, 0, 255), (0, 255, 255), (128, 128, 128), (255, 255, 255),
    ]
    pattern = []
    for y in range(8):
        for x in range(8):
            pattern.append((x + y) % 8)
    write(VALID_DIR / "static_8x8_palette.gif", make_gif(
        8, 8, [{"pixels": pattern}], gct=colors8,
    ))

    # static_256colors.gif — all 256 entries used in a 16x16 image
    colors256 = []
    for i in range(256):
        r = (i * 7) & 0xFF
        g = (i * 13 + 50) & 0xFF
        b = (i * 23 + 100) & 0xFF
        colors256.append((r, g, b))
    pixels256 = list(range(256))
    write(VALID_DIR / "static_256colors.gif", make_gif(
        16, 16, [{"pixels": pixels256}], gct=colors256,
    ))

    # static_interlaced.gif — 8x8 interlaced
    colors4 = [(0, 0, 0), (255, 0, 0), (0, 255, 0), (0, 0, 255)]
    interlace_pattern = []
    for y in range(8):
        for x in range(8):
            interlace_pattern.append(y % 4)
    write(VALID_DIR / "static_interlaced.gif", make_gif(
        8, 8, [{"pixels": interlace_pattern, "interlace": True}], gct=colors4,
    ))

    # --- Animation basics ---

    # anim_2frame.gif — red/blue alternating, 500ms
    rb_gct = [(255, 0, 0), (0, 0, 255)]
    write(VALID_DIR / "anim_2frame.gif", make_gif(
        8, 8, [
            {"pixels": solid_pixels(0, 8, 8), "delay_cs": 50},
            {"pixels": solid_pixels(1, 8, 8), "delay_cs": 50},
        ], gct=rb_gct, loop_count=0,
    ))

    # anim_3frame_rgb.gif
    rgb_gct = [(255, 0, 0), (0, 255, 0), (0, 0, 255)]
    write(VALID_DIR / "anim_3frame_rgb.gif", make_gif(
        8, 8, [
            {"pixels": solid_pixels(0, 8, 8), "delay_cs": 50},
            {"pixels": solid_pixels(1, 8, 8), "delay_cs": 50},
            {"pixels": solid_pixels(2, 8, 8), "delay_cs": 50},
        ], gct=rgb_gct, loop_count=0,
    ))

    # anim_10frame.gif — fade from black to white
    grays = [(i * 255 // 9, i * 255 // 9, i * 255 // 9) for i in range(10)]
    # Pad to 16 entries (next power of 2)
    while len(grays) < 16:
        grays.append((0, 0, 0))
    frames_10 = []
    for i in range(10):
        frames_10.append({"pixels": solid_pixels(i, 8, 8), "delay_cs": 20})
    write(VALID_DIR / "anim_10frame.gif", make_gif(
        8, 8, frames_10, gct=grays, loop_count=0,
    ))

    # --- Disposal methods ---

    # Common setup: 8x8, 2 frames, first covers left half, second covers right
    disp_gct = [(255, 255, 255), (255, 0, 0), (0, 0, 255)]

    def disposal_frames(disposal_method):
        return [
            {
                "pixels": solid_pixels(1, 4, 8),
                "left": 0, "top": 0, "width": 4, "height": 8,
                "delay_cs": 50, "disposal": disposal_method,
            },
            {
                "pixels": solid_pixels(2, 4, 8),
                "left": 4, "top": 0, "width": 4, "height": 8,
                "delay_cs": 50, "disposal": disposal_method,
            },
        ]

    write(VALID_DIR / "dispose_none.gif", make_gif(
        8, 8, disposal_frames(1), gct=disp_gct, loop_count=0, bg_color=0,
    ))
    write(VALID_DIR / "dispose_background.gif", make_gif(
        8, 8, disposal_frames(2), gct=disp_gct, loop_count=0, bg_color=0,
    ))
    write(VALID_DIR / "dispose_previous.gif", make_gif(
        8, 8, disposal_frames(3), gct=disp_gct, loop_count=0, bg_color=0,
    ))
    write(VALID_DIR / "dispose_unspecified.gif", make_gif(
        8, 8, disposal_frames(0), gct=disp_gct, loop_count=0, bg_color=0,
    ))

    # --- Transparency ---

    # transparent_bg.gif — index 0 is transparent, bg_color=0
    trans_gct = [(0, 0, 0), (255, 0, 0), (0, 255, 0)]
    checkerboard = []
    for y in range(8):
        for x in range(8):
            checkerboard.append(0 if (x + y) % 2 == 0 else 1)
    write(VALID_DIR / "transparent_bg.gif", make_gif(
        8, 8, [{
            "pixels": checkerboard,
            "transparent": True, "transparent_index": 0,
        }], gct=trans_gct, bg_color=0,
    ))

    # transparent_frame.gif — frame has transparent pixels via GCE
    write(VALID_DIR / "transparent_frame.gif", make_gif(
        8, 8, [
            {"pixels": solid_pixels(1, 8, 8), "delay_cs": 50},
            {
                "pixels": checkerboard, "delay_cs": 50,
                "transparent": True, "transparent_index": 0,
            },
        ], gct=trans_gct, loop_count=0,
    ))

    # --- Timing ---

    timing_gct = [(255, 0, 0), (0, 0, 255)]

    write(VALID_DIR / "delay_0.gif", make_gif(
        4, 4, [
            {"pixels": solid_pixels(0, 4, 4), "delay_cs": 0},
            {"pixels": solid_pixels(1, 4, 4), "delay_cs": 0},
        ], gct=timing_gct, loop_count=0,
    ))

    write(VALID_DIR / "delay_10ms.gif", make_gif(
        4, 4, [
            {"pixels": solid_pixels(0, 4, 4), "delay_cs": 1},
            {"pixels": solid_pixels(1, 4, 4), "delay_cs": 1},
        ], gct=timing_gct, loop_count=0,
    ))

    write(VALID_DIR / "delay_1s.gif", make_gif(
        4, 4, [
            {"pixels": solid_pixels(0, 4, 4), "delay_cs": 100},
            {"pixels": solid_pixels(1, 4, 4), "delay_cs": 100},
        ], gct=timing_gct, loop_count=0,
    ))

    # variable_delay.gif — 4 frames with different delays
    var_gct = [(255, 0, 0), (0, 255, 0), (0, 0, 255), (255, 255, 0)]
    write(VALID_DIR / "variable_delay.gif", make_gif(
        4, 4, [
            {"pixels": solid_pixels(0, 4, 4), "delay_cs": 10},
            {"pixels": solid_pixels(1, 4, 4), "delay_cs": 50},
            {"pixels": solid_pixels(2, 4, 4), "delay_cs": 100},
            {"pixels": solid_pixels(3, 4, 4), "delay_cs": 200},
        ], gct=var_gct, loop_count=0,
    ))

    # --- Looping ---

    loop_gct = [(255, 0, 0), (0, 0, 255)]

    write(VALID_DIR / "loop_infinite.gif", make_gif(
        4, 4, [
            {"pixels": solid_pixels(0, 4, 4), "delay_cs": 50},
            {"pixels": solid_pixels(1, 4, 4), "delay_cs": 50},
        ], gct=loop_gct, loop_count=0,
    ))

    write(VALID_DIR / "loop_once.gif", make_gif(
        4, 4, [
            {"pixels": solid_pixels(0, 4, 4), "delay_cs": 50},
            {"pixels": solid_pixels(1, 4, 4), "delay_cs": 50},
        ], gct=loop_gct, loop_count=1,
    ))

    write(VALID_DIR / "loop_3.gif", make_gif(
        4, 4, [
            {"pixels": solid_pixels(0, 4, 4), "delay_cs": 50},
            {"pixels": solid_pixels(1, 4, 4), "delay_cs": 50},
        ], gct=loop_gct, loop_count=3,
    ))

    # no_loop_ext.gif — no Netscape extension at all
    write(VALID_DIR / "no_loop_ext.gif", make_gif(
        4, 4, [
            {"pixels": solid_pixels(0, 4, 4), "delay_cs": 50},
            {"pixels": solid_pixels(1, 4, 4), "delay_cs": 50},
        ], gct=loop_gct, loop_count=None,
    ))

    # --- Color tables ---

    # global_ct_only.gif — two frames, both use GCT
    gctonly_colors = [(255, 0, 0), (0, 255, 0), (0, 0, 255), (0, 0, 0)]
    write(VALID_DIR / "global_ct_only.gif", make_gif(
        4, 4, [
            {"pixels": solid_pixels(0, 4, 4), "delay_cs": 50},
            {"pixels": solid_pixels(1, 4, 4), "delay_cs": 50},
        ], gct=gctonly_colors, loop_count=0,
    ))

    # local_ct.gif — frame overrides GCT with local color table
    local_colors = [(0, 0, 0), (255, 128, 0)]  # orange instead of red
    write(VALID_DIR / "local_ct.gif", make_gif(
        4, 4, [
            {"pixels": solid_pixels(0, 4, 4), "delay_cs": 50},
            {
                "pixels": solid_pixels(1, 4, 4), "delay_cs": 50,
                "lct": local_colors,
            },
        ], gct=[(255, 0, 0), (0, 0, 255)], loop_count=0,
    ))

    # mixed_ct.gif — 3 frames: GCT, LCT, GCT
    write(VALID_DIR / "mixed_ct.gif", make_gif(
        4, 4, [
            {"pixels": solid_pixels(0, 4, 4), "delay_cs": 50},
            {
                "pixels": solid_pixels(1, 4, 4), "delay_cs": 50,
                "lct": [(0, 128, 255), (255, 128, 0)],
            },
            {"pixels": solid_pixels(1, 4, 4), "delay_cs": 50},
        ], gct=[(255, 0, 0), (0, 0, 255)], loop_count=0,
    ))

    # --- Canvas/frame geometry ---

    # small_frame_big_canvas.gif — 16x16 canvas, 4x4 frame at offset (6,6)
    geo_gct = [(200, 200, 200), (255, 0, 0)]
    write(VALID_DIR / "small_frame_big_canvas.gif", make_gif(
        16, 16, [{
            "pixels": solid_pixels(1, 4, 4),
            "left": 6, "top": 6, "width": 4, "height": 4,
        }], gct=geo_gct, bg_color=0,
    ))

    # overlapping_frames.gif — two frames at different offsets
    write(VALID_DIR / "overlapping_frames.gif", make_gif(
        16, 16, [
            {
                "pixels": solid_pixels(1, 8, 8),
                "left": 0, "top": 0, "width": 8, "height": 8,
                "delay_cs": 50,
            },
            {
                "pixels": solid_pixels(2, 8, 8),
                "left": 4, "top": 4, "width": 8, "height": 8,
                "delay_cs": 50,
            },
        ], gct=[(200, 200, 200), (255, 0, 0), (0, 0, 255)], loop_count=0,
    ))

    # --- Minimum viable ---

    # 1x1.gif
    write(VALID_DIR / "1x1.gif", make_gif(
        1, 1, [{"pixels": [0]}], gct=[(0, 0, 0), (255, 255, 255)],
    ))

    # 2color.gif — minimum palette
    write(VALID_DIR / "2color.gif", make_gif(
        4, 4, [{"pixels": [i % 2 for i in range(16)]}],
        gct=[(0, 0, 0), (255, 255, 255)],
    ))


# ---------------------------------------------------------------------------
# Invalid files
# ---------------------------------------------------------------------------

def generate_invalid():
    print("Generating invalid files...")

    # bad_magic.gif — "GIF90a"
    data = bytearray(make_gif(
        4, 4, [{"pixels": solid_pixels(0, 4, 4)}],
        gct=[(255, 0, 0), (0, 0, 0)],
    ))
    data[0:6] = b"GIF90a"
    write(INVALID_DIR / "bad_magic.gif", bytes(data))

    # truncated_header.gif — only 4 bytes
    write(INVALID_DIR / "truncated_header.gif", b"GIF8")

    # truncated_lzw.gif — cut LZW data short
    full = make_gif(
        8, 8, [{"pixels": solid_pixels(0, 8, 8)}],
        gct=[(255, 0, 0), (0, 0, 0)],
    )
    # Cut off last 10 bytes (includes trailer and end of LZW data)
    write(INVALID_DIR / "truncated_lzw.gif", full[:-10])

    # empty.gif — 0 bytes
    write(INVALID_DIR / "empty.gif", b"")

    # no_trailer.gif — valid but missing 0x3B
    no_trail = make_gif(
        4, 4, [{"pixels": solid_pixels(0, 4, 4)}],
        gct=[(255, 0, 0), (0, 0, 0)], add_trailer=False,
    )
    write(INVALID_DIR / "no_trailer.gif", no_trail)

    # bad_lzw_code.gif — corrupt the LZW stream
    corrupt = bytearray(make_gif(
        4, 4, [{"pixels": solid_pixels(0, 4, 4)}],
        gct=[(255, 0, 0), (0, 0, 0)],
    ))
    # Find the LZW data (after image descriptor) and corrupt it
    # Image descriptor starts with 0x2C. After GCT + GCE it should be findable.
    idx = corrupt.index(0x2C)  # Image descriptor separator
    # Skip image descriptor (10 bytes) + min code size (1 byte) + sub-block size (1 byte)
    lzw_start = idx + 10 + 1 + 1
    if lzw_start + 3 < len(corrupt):
        corrupt[lzw_start] = 0xFF
        corrupt[lzw_start + 1] = 0xFF
        corrupt[lzw_start + 2] = 0xFF
    write(INVALID_DIR / "bad_lzw_code.gif", bytes(corrupt))

    # zero_dimensions.gif — width=0
    data = bytearray()
    data.extend(gif_header("89a"))
    data.extend(logical_screen_descriptor(
        0, 0, gct_flag=True, color_res=0, gct_size_field=0, bg_color=0,
    ))
    data.extend(color_table([(0, 0, 0), (255, 255, 255)], pad_to=2))
    data.extend(image_descriptor(left=0, top=0, width=0, height=0))
    # Minimal LZW: clear + EOI
    data.append(2)  # min code size
    data.append(2)  # sub-block size
    data.extend([0x04, 0x01])  # clear code + EOI packed
    data.append(0)  # block terminator
    data.extend(trailer())
    write(INVALID_DIR / "zero_dimensions.gif", bytes(data))


# ---------------------------------------------------------------------------
# Edge cases
# ---------------------------------------------------------------------------

def generate_edge_cases():
    print("Generating edge-case files...")

    # gif87a.gif — GIF87a format (no extensions)
    # GIF87a has no GCE, no application extensions
    data = bytearray()
    data.extend(gif_header("87a"))
    gct = [(255, 0, 0), (0, 0, 0)]
    gct_f = gct_size_field(len(gct))
    data.extend(logical_screen_descriptor(
        4, 4, gct_flag=True, color_res=gct_f, gct_size_field=gct_f,
    ))
    data.extend(color_table(gct, pad_to=color_table_size(gct_f)))
    data.extend(image_descriptor(left=0, top=0, width=4, height=4))
    pixels = solid_pixels(0, 4, 4)
    data.extend(image_data(pixels, min_code_size=2, color_depth=gct_f + 1))
    data.extend(trailer())
    write(EDGE_DIR / "gif87a.gif", bytes(data))

    # comment_ext.gif — has a comment extension
    gct_ce = [(0, 0, 255), (255, 255, 255)]
    gif_data = bytearray()
    gif_data.extend(gif_header("89a"))
    gct_f = gct_size_field(len(gct_ce))
    gif_data.extend(logical_screen_descriptor(
        4, 4, gct_flag=True, color_res=gct_f, gct_size_field=gct_f,
    ))
    gif_data.extend(color_table(gct_ce, pad_to=color_table_size(gct_f)))
    gif_data.extend(comment_extension("This is a GIF conformance test file."))
    gif_data.extend(graphics_control_extension())
    gif_data.extend(image_descriptor(left=0, top=0, width=4, height=4))
    gif_data.extend(image_data(solid_pixels(0, 4, 4), min_code_size=2))
    gif_data.extend(trailer())
    write(EDGE_DIR / "comment_ext.gif", bytes(gif_data))

    # plain_text_ext.gif — has a plain text extension (rarely supported)
    gif_data = bytearray()
    gif_data.extend(gif_header("89a"))
    gct_pt = [(0, 0, 0), (255, 255, 255)]
    gct_f = gct_size_field(len(gct_pt))
    gif_data.extend(logical_screen_descriptor(
        64, 16, gct_flag=True, color_res=gct_f, gct_size_field=gct_f,
    ))
    gif_data.extend(color_table(gct_pt, pad_to=color_table_size(gct_f)))
    # First, an actual image frame (some decoders require at least one image)
    gif_data.extend(graphics_control_extension())
    gif_data.extend(image_descriptor(left=0, top=0, width=64, height=16))
    gif_data.extend(image_data(solid_pixels(0, 64, 16), min_code_size=2))
    # Then the plain text extension
    gif_data.extend(plain_text_extension(
        left=0, top=0, width=64, height=16,
        cell_w=8, cell_h=8, fg=1, bg=0, text="Hello GIF!",
    ))
    gif_data.extend(trailer())
    write(EDGE_DIR / "plain_text_ext.gif", bytes(gif_data))

    # large_palette_small_image.gif — 256 colors but only 2 used
    colors_big = [(i, i, i) for i in range(256)]
    pixels_small = [0, 255] * 8  # 4x4 checkerboard
    write(EDGE_DIR / "large_palette_small_image.gif", make_gif(
        4, 4, [{"pixels": pixels_small}], gct=colors_big,
    ))


# ---------------------------------------------------------------------------
# Main
# ---------------------------------------------------------------------------

def main():
    for d in [VALID_DIR, INVALID_DIR, EDGE_DIR]:
        d.mkdir(parents=True, exist_ok=True)

    generate_valid()
    generate_invalid()
    generate_edge_cases()

    # Summary
    valid_count = len(list(VALID_DIR.glob("*.gif")))
    invalid_count = len(list(INVALID_DIR.glob("*.gif")))
    edge_count = len(list(EDGE_DIR.glob("*.gif")))
    print(f"\nGenerated {valid_count} valid, {invalid_count} invalid, {edge_count} edge-case files.")
    print(f"Total: {valid_count + invalid_count + edge_count} files")


if __name__ == "__main__":
    main()
