LIST(APPEND TESTS_FILES
	animated_gif_reader
	bug00005_2
	bug00181
	bug00227
	bug00499
	gif_null
	gif_nocolormaps
	ossfuzz5700
	php_bug_75571
	uninitialized_memory_read
	bug00006
	bug00060
	gif_im2im
	gif_conformance
	bug00415
	bug000953
	bug000971
)

IF(PNG_FOUND)
LIST(APPEND TESTS_FILES
	bug000498
	bug00005
	bug00066
)
ENDIF(PNG_FOUND)

ADD_GD_TESTS()
