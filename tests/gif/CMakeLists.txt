LIST(APPEND TESTS_FILES
	bug00005_2
	bug00181
	bug00227
	bug00499
	gif_null
	ossfuzz5700
	php_bug_75571
	uninitialized_memory_read
	bug00006
	bug00060
	gif_im2im
)

IF(PNG_FOUND)
LIST(APPEND TESTS_FILES
	bug00005
	bug00066
)
ENDIF(PNG_FOUND)

ADD_GD_TESTS()
