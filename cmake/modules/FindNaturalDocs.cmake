# - Find NaturalDocs
# This module finds if NaturalDocs is installed, and adds it as an external
# dependency if it isn't. This code sets the following variables:
#
#  NATURALDOCS_EXECUTABLE       = full path to the naturaldocs binary

find_program(NATURALDOCS_EXECUTABLE NAMES naturaldocs NaturalDocs)

if(NATURALDOCS_EXECUTABLE)
    execute_process(COMMAND ${NATURALDOCS_EXECUTABLE} --help
      RESULT_VARIABLE res
      OUTPUT_VARIABLE var
      ERROR_VARIABLE var
      OUTPUT_STRIP_TRAILING_WHITESPACE
      ERROR_STRIP_TRAILING_WHITESPACE)
    if(res)
      if(${NaturalDocs_FIND_REQUIRED})
        message(FATAL_ERROR "Error executing naturaldocs --help")
      elseif(NOT NaturalDocs_FIND_QUIETLY)
        message(WARNING "Warning, could not run naturaldocs --help")
      endif()
    else()
      if(var MATCHES ".*Natural Docs, version [0-9]+\\.[0-9]+.*")
        string(REGEX REPLACE ".*Natural Docs, version ([0-9]+\\.[0-9]+).*"
               "\\1" NATURALDOCS_VERSION_STRING "${var}")
      else()
        if(NOT NaturalDocs_FIND_QUIETLY)
          message(WARNING "regex not supported: {$var}.")
        endif()
      endif()

      string( REGEX REPLACE "([0-9]+).*" "\\1" NATURALDOCS_VERSION_MAJOR "${NATURALDOCS_VERSION_STRING}" )
      string( REGEX REPLACE "[0-9]+\\.([0-9]+)" "\\1" NATURALDOCS_VERSION_MINOR "${NATURALDOCS_VERSION_STRING}" )
      set(NATURALDOCS_VERSION ${NATURALDOCS_VERSION_MAJOR}.${NATURALDOCS_VERSION_MINOR})
    endif()
endif()

mark_as_advanced(
  NATURALDOCS_EXECUTABLE
  )

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(NaturalDocs
  REQUIRED_VARS NATURALDOCS_EXECUTABLE
  VERSION_VAR NATURALDOCS_VERSION)