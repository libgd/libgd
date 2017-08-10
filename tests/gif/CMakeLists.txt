LIST(APPEND TESTS_FILES
	bug00005_2
	bug00181
	bug00227
	gif_null
)

IF(PNG_FOUND)
LIST(APPEND TESTS_FILES
	bug00005
	bug00006
	bug00060
	bug00066
	gif_im2im
	uninitialized_memory_read
)
ENDIF(PNG_FOUND)

ADD_GD_TESTS()
