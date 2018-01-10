libdir=@CMAKE_INSTALL_FULL_LIBDIR@
includedir=@CMAKE_INSTALL_FULL_INCLUDEDIR@

Name: gd
Description: GD graphics library
Version: @GDLIB_VERSION@
Cflags: -I${includedir}
Libs.private: @LIBGD_DEP_LIBS@
Libs: -L${libdir} -lgd
