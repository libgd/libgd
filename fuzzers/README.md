# Fuzzing targets

Contains fuzzing targets for the libFuzzer API. These are run continuously by
Google's [oss-fuzz](https://github.com/google/oss-fuzz) initiative.

Test changes to these files by running, from the root of a local oss-fuzz
checkout,

```
python infra/helper.py build_fuzzers libgd /path/to/local/libgd/checkout
```
