prefix=@CMAKE_INSTALL_PREFIX@
exec_prefix=@CMAKE_INSTALL_PREFIX@
libdir=${exec_prefix}/@CMAKE_INSTALL_LIBDIR@
includedir=${prefix}/@CMAKE_INSTALL_INCLUDEDIR@

Name: gd
Description: GD graphics library
Version: @GDLIB_VERSION@
Requires:
Requires.private: @PKG_REQUIRES_PRIVATES@
Cflags: -I${includedir}
Libs.private: @LIBS_PRIVATES@
Libs: -L${libdir} -lgd
