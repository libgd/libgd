# libgd codec fuzz harnesses (Tier 1)

libFuzzer harnesses for the codec **decode** paths in libgd. One translation
unit per codec, one `LLVMFuzzerTestOneInput` each. Dumb by design, call the
decoder, free the result, return 0. ASan + UBSan surface the bugs.

This is **not** a replacement for OSS-Fuzz. It is a faster, on-PR feedback
loop on top of OSS-Fuzz's continuous fuzzing.

## Scope

Decode-only (`gdImageCreateFrom*Ptr`) for these codecs:

| Harness                 | Decode surface                                                    |
| ----------------------- | ----------------------------------------------------------------- |
| `fuzz_jxl_decode`        | `gdImageCreateFromJxlPtr`                                          |
| `fuzz_uhdr_decode`       | `gdUhdrImageCreateFromPtr(... GD_UHDR_FORMAT_JPEG, &err)` + `gdUhdrImageDestroy` |
| `fuzz_tiff_decode`       | `gdImageCreateFromTiffPtr`                                         |
| `fuzz_bmp_decode`        | `gdImageCreateFromBmpPtr`                                          |
| `fuzz_png_decode`        | `gdImageCreateFromPngPtr`                                          |
| `fuzz_webp_decode`       | `gdImageCreateFromWebpPtr`                                        |
| `fuzz_gif_decode`        | `gdImageCreateFromGifPtr`                                         |
| `fuzz_jpeg_decode`       | `gdImageCreateFromJpegPtr`                                        |

Encode/roundtrip and drawing-API fuzzing are out of scope for Tier 1 —
future specs cover Tier 2 (roundtrip) and Tier 3 (drawing/transform).

## Building locally

Requires clang (`ENABLE_FUZZERS` is clang-gated; gcc/MSVC builds ignore it).

```sh
CC=clang CXX=clang++ cmake \
  -DENABLE_FUZZERS=1 \
  -DENABLE_GD_FORMATS=1 \
  -DENABLE_PNG=1 -DENABLE_JPEG=1 -DENABLE_JXL=1 \
  -DENABLE_ULTRAHDR=1 -DENABLE_WEBP=1 -DENABLE_TIFF=1 \
  -DBUILD_TESTS=1 -DBUILD_EXAMPLES=0 -DBUILD_DOCS=0 -DBUILD_PROGRAMS=0 -DBUILD_CPACK=0 \
  -B build -DCMAKE_BUILD_TYPE=RelWithDebInfo

# Build one harness (cheap):
cmake --build build --target fuzz_png_decode --parallel 4

# Or build all enabled ones at once:
cmake --build build --target \
  fuzz_jxl_decode fuzz_uhdr_decode fuzz_tiff_decode fuzz_bmp_decode \
  fuzz_png_decode fuzz_webp_decode fuzz_gif_decode fuzz_jpeg_decode --parallel 4
```

Binaries land in `build/Bin/`.

## Running locally

Each harness takes one or more corpus directories as arguments. **Always
write to a throwaway output dir, never to `tests/fuzzing/corpus/<codec>/`**:
libFuzzer writes any newly-discovered interesting inputs into the FIRST
corpus dir on the command line, and `-artifact_prefix` only controls where
crash/leak/oom/timeout files land, **not** the regular NEW coverage inputs.
Pointing the bare fuzzer at the committed corpus will grow/mutate it.

The pattern: pass an empty `/tmp` dir as the first corpus arg (output), and
the committed corpus as the second arg (input, read-only):

```sh
# Short CI-style run (120s, default RSS limit):
mkdir -p /tmp/fuzz-out/png /tmp/fuzz-corpus/png
./build/Bin/fuzz_png_decode \
  -max_total_time=120 -rss_limit_mb=2048 \
  -artifact_prefix=/tmp/fuzz-out/png/ \
  /tmp/fuzz-corpus/png \
  tests/fuzzing/corpus/png

# Long run while investigating a crash (30min, bigger RSS budget):
mkdir -p /tmp/fuzz-out/png /tmp/fuzz-corpus/png
./build/Bin/fuzz_png_decode \
  -max_total_time=3600 -rss_limit_mb=4096 \
  -artifact_prefix=/tmp/fuzz-out/png/ \
  /tmp/fuzz-corpus/png \
  tests/fuzzing/corpus/png
```

- `/tmp/fuzz-out/png/` receives crash/leak/oom/timeout files (if any).
- `/tmp/fuzz-corpus/png/` receives new coverage-discovered inputs (grow this freely).
- `tests/fuzzing/corpus/png/` is only ever read, never written.

## Reproducing a CI crash

When the CI workflow finds a crash, it uploads the crash artifact for that
codec as `fuzz-crash-<codec>` (containing `crash-*`, `leak-*`, `timeout-*`,
`oom-*` files). Download and unzip, then run the harness against the single
crash file:

```sh
./build/Bin/fuzz_png_decode path/to/crash-<sha>
```

The harness will run one input and exit (no `-artifact_prefix` needed since
no fuzzing/mutation happens for a single-file input, it just runs it once).
If it's an ASan crash, the stack trace will be printed. If it's a leak,
you'll see the `LEAK:` summary.

## Seed corpus

Each codec has a committed corpus directory under `tests/fuzzing/corpus/<codec>/`
(27 seeds total, ~256KB). The corpus is **self-contained**, it does not
reference `tests/images/` or `codecs-corpus/` at run time. The harnesses
read directly from this committed directory. Seed selection was done by
copying the smallest committed test fixtures per codec into `/tmp`
candidates, then running `-merge=1` to keep only files that earned unique
coverage edges.

### Contributing new seeds

The committed corpus should only grow after a deliberate `-merge` that has
been verified to earn new coverage. The workflow:

1. Put candidate seeds in a throwaway dir (NOT in `tests/fuzzing/corpus/`):

```sh
mkdir -p /tmp/<codec>-candidates
cp my-new-seed.<ext> /tmp/<codec>-candidates/
# you can also add several candidates at once
```

2. Run `-merge=1` with `OUTDIR` = a fresh temp dir; the committed corpus
   `tests/fuzzing/corpus/<codec>/` is passed as an **input** (read-only here,
   because merge only writes to `OUTDIR`):

```sh
mkdir -p /tmp/<codec>-merged
./build/Bin/fuzz_<codec>_decode -merge=1 \
  /tmp/<codec>-merged \
  tests/fuzzing/corpus/<codec> \
  /tmp/<codec>-candidates
```

3. Inspect what merge kept. If only the committed seeds survived (i.e. your
   candidates added zero new coverage), discard `/tmp/<codec>-merged` and
   stop, nothing to commit.

```sh
ls -la /tmp/<codec>-merged/
```

4. If your candidates earned new coverage, copy the new files from
   `/tmp/<codec>-merged/` into the committed corpus dir, preserving
   descriptive filenames (libFuzzer names merged outputs by content sha1;
   rename to something human-readable like `interlaced_progressive.png`):

```sh
for f in /tmp/<codec>-merged/*; do
  [ -f "$f" ] && cp "$f" tests/fuzzing/corpus/<codec>/<descriptive-name>.<ext>
done
```

5. Verify the committed corpus still parses cleanly by listing it and
   running one short fuzz run against it (read-only, with
   `-artifact_prefix=/tmp/`).

`-merge=1` is the only sanctioned way to mutate the committed corpus. Do
**not** run the bare fuzzer against `tests/fuzzing/corpus/<codec>/` to grow
it, that path also writes to the corpus and is what polluted the corpus
during an earlier verification run.

## CI

`.github/workflows/fuzz-codecs.yml` runs on every PR to `main` and via
`workflow_dispatch`. It's a matrix over the 8 codecs with `fail-fast: false`
(one codec's crash doesn't hide signal from the others). Per-PR budget is
120s per codec; longer budgets are a future scheduled (nightly/weekly) job,
out of scope for Tier 1.

## Known gotcha: bitmap font tables + UBSan

The spec warned about UBSan alignment noise from bitmap font data arrays
(`concept.md` lines 170–179). A smoke pass over all 8 harnesses with
`UBSAN_OPTIONS=halt_on_error=0:print_stacktrace=1` produced **zero** UBSan
reports, the decode paths don't touch the font tables. No suppression file
is currently needed.

If a future Tier 3 harness (drawing-API fuzzing) hits the font tables and
fires benign alignment noise, suppress it narrowly per the concept:
`__attribute__((no_sanitize("alignment")))` on the specific function, or a
`suppressions/ubsan.supp` file pointed at by `UBSAN_OPTIONS`. **Don't**
disable UBSan globally for one table.
