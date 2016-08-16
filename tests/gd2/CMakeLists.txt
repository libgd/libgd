LIST(APPEND TESTS_FILES
	bug_289
	bug00309
	gd2_empty_file
	gd2_im2im
	gd2_null
	php_bug_72339
	gd2_read
	gd2_read_corrupt
	too_few_image_data
)

ADD_GD_TESTS()
