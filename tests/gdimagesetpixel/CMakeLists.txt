LIST(APPEND TESTS_FILES
	bug00186
	gdeffectoverlay
	gdeffectmultiply
	gdeffectdiff
)

IF(PNG_FOUND)
LIST(APPEND TESTS_FILES
	alpha_blending
)
ENDIF(PNG_FOUND)

ADD_GD_TESTS()
