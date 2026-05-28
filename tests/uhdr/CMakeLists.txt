IF(HAVE_LIBUHDR)
LIST(APPEND TESTS_FILES
	uhdr_null
	uhdr_plain_jpeg_rejected
	uhdr_roundtrip
	uhdr_ptr_roundtrip
)

ADD_GD_TESTS()

FOREACH(test_name ${TESTS_FILES})
	SET(test_prog_name "test_uhdr_${test_name}")
	target_compile_definitions(${test_prog_name} PRIVATE
		GD_UHDR_SAMPLE_PATH="${PROJECT_BINARY_DIR}/tmptest/generated_uhdr.jpg")
ENDFOREACH(test_name)
ENDIF(HAVE_LIBUHDR)
