LIST(APPEND TESTS_FILES
	gdimageline_aa_outofrange
)

IF(PNG_FOUND)
LIST(APPEND TESTS_FILES
	bug00077
	bug00072
	bug00111
	gdimageline_aa
	gdimageline_bug5
	gdImageAALine_thickness
	github_bug_167
)
ENDIF(PNG_FOUND)

ADD_GD_TESTS(m)
