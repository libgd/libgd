SET(TESTS_FILES
	path_structure
	raster_parity
)

IF(PNG_FOUND)
	LIST(APPEND TESTS_FILES visual)
ENDIF(PNG_FOUND)

ADD_GD_TESTS(${GD_LIB_STATIC})

SET_TESTS_PROPERTIES(test_vector2d_path_structure PROPERTIES LABELS "vector2d-structural")
SET_TESTS_PROPERTIES(test_vector2d_raster_parity PROPERTIES LABELS "vector2d-parity")
IF(PNG_FOUND)
	SET_TESTS_PROPERTIES(test_vector2d_visual PROPERTIES LABELS "vector2d-visual")
ENDIF(PNG_FOUND)
