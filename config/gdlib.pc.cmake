libdir=@CMAKE_INSTALL_FULL_LIBDIR@
includedir=@CMAKE_INSTALL_FULL_INCLUDEDIR@

Name: gd
Description: GD graphics library
Version: @GDLIB_VERSION@
Requires:
Requires.private: @PKG_REQUIRES_PRIVATES@
Cflags: -I${includedir}
Libs.private: @LIBS_PRIVATES@
Libs: -L${libdir} -lgd
