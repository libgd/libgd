#!/bin/bash -eu

./bootstrap.sh

# Limit the size of buffer allocations to avoid bogus OOM issues
# https://github.com/libgd/libgd/issues/422
sed -i'' -e 's/INT_MAX/100000/' "$SRC/libgd/src/gd_security.c"

./configure --prefix="$WORK" --disable-shared
make -j$(nproc) install

cd $SRC/libgd/fuzzers

# Assemble a corpus
curl -Lo afl_testcases.tgz http://lcamtuf.coredump.cx/afl/demo/afl_testcases.tgz
mkdir -p afl_testcases
for format in bmp gif; do
    tar xf afl_testcases.tgz -C afl_testcases $format
    mkdir -p corpus/$format
    find afl_testcases -type f -name '*.'$format -exec mv -n {} corpus/$format/ \;
done

for target in Bmp Gif Tga Gd Gd2 WBMP; do
    lowercase=$(echo $target | tr "[:upper:]" "[:lower:]")
    $CXX $CXXFLAGS -std=c++11 -I"$WORK/include" -L"$WORK/lib" \
      -DFUZZ_GD_FORMAT=$target \
      parser_target.cc -o $OUT/${lowercase}_target \
      -lFuzzingEngine -Wl,-Bstatic -lgd -lz -Wl,-Bdynamic

    # Add test cases to the fuzzing corpus
    mkdir -p "corpus/$lowercase"
    find "$SRC/libgd/tests" -type f -name '*.'$lowercase -exec cp {} corpus/$lowercase/ \;
    if [ -n "$(ls -A corpus/$lowercase)" ]; then
        zip -rj "$OUT/${lowercase}_target_seed_corpus.zip" corpus/$lowercase
    fi

    # Mask stderr to reduce logging burden
    printf "[libfuzzer]\nclose_fd_mask = 2\n" > $OUT/${lowercase}_target.options
done
