ACLOCAL_AMFLAGS = -I m4
SUBDIRS = src config tests
EXTRA_DIST = test docs examples windows VMS cmake netware bootstrap.sh CMakeLists.txt CONTRIBUTORS README.md

clean-local:
	find -maxdepth 3 \
		'(' \
			-name CMakeCache.txt -o \
			-name CMakeFiles -o \
			-name CTestTestfile.cmake -o \
			-name cmake_install.cmake \
		')' \
		-exec rm -rf {} +
	rm -rf Bin CPackConfig.cmake CPackSourceConfig.cmake
