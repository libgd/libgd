# MSYS Fallback Makefile

This is a simple, straightforward Makefile for building LibGD with
MinGW on MSYS (or possibly Cygwin).  It is here for anyone who doesn't
want to deal with autotools or CMake on Windows or who can't get
either of them working on their particular setup.

Note that this Makefile only builds the library and test cases; the
standalone utilities are ignored.  If you need those, you'll need to
use one of the other build systems (or add them yourself.)

## To build LibGD:

1. Install MinGW and MSYS (see <http://www.mingw.org/wiki/Getting_Started>).
   You will need to install zip and unzip as well as the 32-bit C compiler
   and related tools.

2. Install LibJpeg, LibPng, LibTiff, Zlib and FreeType from
   <http://gnuwin32.sourceforge.net> and install them all in
   `c:/tools/GnuWin32/`.  (You can get these libraries from other
   places and/or install them in different locations, but you'll need to
   edit the Makefile accordingly if you do.)

3. Skip ahead to the next step.  If that doesn't work, edit the
   Makefile to fix what went wrong.  Things to try include:
    * Ensure SRC contains an up-to-date list of source files.  (Take a
      look at libgd_la_SOURCES src/Makefile.am for a reference.)
    * Ensure that the paths and #defines for unsupported libraries
      are commented out.  They're nicely grouped into clusters to
      simplify that.

4. `cd` to this directory and type:

       make
       make check

    If both commands succeed, you're done.  (Note that some of the
    testcases will fail, just because they test features not enabled
    here.  This is fine.  As long as most tests pass, you're probably
    okay.)

5.  Type

       make dist

    This will create libgd-win.zip, a zip file containing the new
    library and its dependants.  Note that this step may pick up extra
    DLLs; it sweeps up all of the DLLs in the directories pointed to
    by *_DLLDIR Makefile variables.

    Before type "make dist", please make sure you have installed
    zip command in msys. Or you can try:
    * If you use msys, you can install msys-zip(lib, docs, lic)
    in MinGW Installation Manager/all package.
    * If you use msys2, you can install msys/zip by pacman and running:

       pacman -S msys/zip

    Alternately, you can copy the lib (in src/) to wherever it needs
    to go.


## Stuff that Doesn't Work

* LibXpm: It's probably nothing serious, just more trouble than it's
  worth.
* LibFontconfig: I couldn't find Windows binaries for it anywhere
  reputable.
