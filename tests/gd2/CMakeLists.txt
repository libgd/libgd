LIST(APPEND TESTS_FILES
	bug_289
	bug00209
	bug00309
	bug00354
	gd2_empty_file
	php_bug_72339
	gd2_read_corrupt
	too_few_image_data
)

IF(ZLIB_FOUND)
LIST(APPEND TESTS_FILES
	gd2_null
)

IF(PNG_FOUND)
LIST(APPEND TESTS_FILES
	gd2_im2im
	gd2_read
)
ENDIF(PNG_FOUND)
ENDIF(ZLIB_FOUND)

ADD_GD_TESTS()
