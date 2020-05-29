prefix=@CMAKE_INSTALL_PREFIX@
exec_prefix=@CMAKE_INSTALL_PREFIX@
libdir=${exec_prefix}/@CMAKE_INSTALL_LIBDIR@
includedir=${prefix}/@CMAKE_INSTALL_INCLUDEDIR@

Name: gd
Description: GD graphics library
Version: @GDLIB_VERSION@
Cflags: -I${includedir}
Libs.private: @LIBGD_DEP_LIBS@
Libs: -L${libdir} -lgd
