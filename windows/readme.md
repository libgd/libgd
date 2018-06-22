# Building on Windows with Visual Studio 2015

* Get the required dependencies from
  http://windows.php.net/downloads/php-sdk/deps/vc14/ and
  http://windows.php.net/downloads/pecl/deps/, respectively. Choose the x86 or
  x64 packages depending on your needs.

  * freetype
  * libiconv
  * libjpeg
  * libpng
  * libtiff
  * libwebp
  * libxpm
  * zlib

* Unpack all dependency packages into the same folder.

* Open the VS2015 x86 or x64 Native Tools Command Prompt.

* Set the environment variable `WITH_DEVEL` to the path where you have unpacked
  the dependencies. `WITH_DEVEL` defaults to `..\deps`.

* If you want a debug build, do
````
set DEBUG=1
````

* `cd` into the libgd source folder.

* To build the libraries, do:
```
nmake /f windows\Makefile.vc
```

* To build and run the tests, do:
````
nmake /f windows\Makefile.vc check
````

* To build the executable programs, do:
````
nmake /f windows\Makefile.vc build_progs
````

* After a successful build, you find the libraries and test executables in
  `..\gdbuild`. You can change the build folder by setting the environment
  variable `WITH_BUILD` to the desired path.

* Before rebuilding, you may have to run:
```
nmake /f windows\Makefile.vc clean
```
