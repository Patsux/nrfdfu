
function(utils_add_library)
	# Create a library target.
	#      - LIB_NAME is the target name.
	#      - SRCS is the list of library's files.
	#      - PUBLIC_INCLUDE_DIRS is the list of public headers that will be shared
	#        with targets linking the library.
	#      - PRIVATE_INCLUDE_DIRS is the list of private headers.
	#      - LINK_LIBS is the list of libraries to link with the target.
	cmake_parse_arguments(ARG
	 		      ""
			      "LIB_NAME"
			      "SRCS;PUBLIC_INCLUDE_DIRS;PRIVATE_INCLUDE_DIRS;LINK_LIBS"
			      ${ARGN})

	add_library(${ARG_LIB_NAME} ${ARG_SRCS})

	target_include_directories(${ARG_LIB_NAME}
	 			   PUBLIC ${ARG_PUBLIC_INCLUDE_DIRS}
				   PRIVATE ${ARG_PRIVATE_INCLUDE_DIRS})

	target_link_libraries(${ARG_LIB_NAME} ${ARG_LINK_LIBS})
endfunction()

function(utils_add_executable)
	# Create a library target.
	#      - EXEC_NAME is the target name.
	#      - SRCS is the list of executable's files.
	#      - PUBLIC_INCLUDE_DIRS is the list of public headers that will
        #        be shared with targets linking the executable.
	#      - PRIVATE_INCLUDE_DIRS is the list of private headers.
	#      - LINK_LIBS is the list of libraries to link with the target.
	cmake_parse_arguments(ARG
			      ""
			      "EXEC_NAME"
			      "SRCS;PUBLIC_INCLUDE_DIRS;PRIVATE_INCLUDE_DIRS;LINK_LIBS"
			      ${ARGN})

	add_executable(${ARG_EXEC_NAME} ${ARG_SRCS})

	target_include_directories(${ARG_EXEC_NAME}
				   PUBLIC ${ARG_PUBLIC_INCLUDE_DIRS}
				   PRIVATE ${ARG_PRIVATE_INCLUDE_DIRS})

	target_link_libraries(${ARG_EXEC_NAME}
			      ${ARG_LINK_LIBS})
endfunction()

function(utils_add_ut)
	# This function creates an executable for unit tests. It creates also a
	# coverage target named coverage-EXEC_NAME.
	#      - EXEC_NAME is the target name.
	#      - SRCS is the list of executable's files.
	#      - PUBLIC_INCLUDE_DIRS is the list of public headers that will
	#        be shared with targets linking the executable..
	#      - PRIVATE_INCLUDE_DIRS is the list of private headers.
	#      - LINK_LIBS is the list of libraries to link with the target.
	cmake_parse_arguments(ARG
			      ""
			      "EXEC_NAME"
			      "SRCS;PUBLIC_INCLUDE_DIRS;PRIVATE_INCLUDE_DIRS;LINK_LIBS"
			      ${ARGN}
	)

	add_executable(${ARG_EXEC_NAME}
		       ${ARG_SRCS}
	)

	target_include_directories(${ARG_EXEC_NAME}
				   PUBLIC ${ARG_PUBLIC_INCLUDE_DIRS}
				   PRIVATE ${ARG_PRIVATE_INCLUDE_DIRS}
	)

	target_link_libraries(${ARG_EXEC_NAME}
			      ${ARG_LINK_LIBS}
			      cmocka
			      gcov
	)

	# Add the coverage options for tested libraries
	foreach(LIB ${ARG_LINK_LIBS})
		target_compile_options(${LIB} PUBLIC --coverage)
	endforeach(LIB)

	set(COVER_REPORT_DIR "${CMAKE_BINARY_DIR}/coverage-reports")

	# TODO This is not perfect, because we don't manage correctly which files
	# are in the report and some absolute paths still present.
	# Consider this as temporary.
	add_custom_target(
		coverage-${ARG_EXEC_NAME}
		DEPENDS ${ARG_EXEC_NAME}
		COMMAND mkdir -p ${COVER_REPORT_DIR}
		COMMAND mkdir -p ${COVER_REPORT_DIR}/html/${ARG_EXEC_NAME}
		COMMAND find . -name "*.gcda" -type f -delete
		COMMAND ${ARG_EXEC_NAME}
		COMMAND lcov --capture --directory . --output-file coverage-${ARG_EXEC_NAME}.info
		COMMAND genhtml coverage-${ARG_EXEC_NAME}.info --output-directory ${COVER_REPORT_DIR}/html/${ARG_EXEC_NAME}
	)
endfunction()

function(utils_add_func)
	cmake_parse_arguments(ARG
			      ""
			      "EXEC_NAME"
			      "SRCS;PUBLIC_INCLUDE_DIRS;PRIVATE_INCLUDE_DIRS;LINK_LIBS"
			      ${ARGN})

	add_executable(${ARG_EXEC_NAME}
		       ${ARG_SRCS}
	)

	target_include_directories(${ARG_EXEC_NAME}
				   PUBLIC ${ARG_PUBLIC_INCLUDE_DIRS}
				   PRIVATE ${ARG_PRIVATE_INCLUDE_DIRS}
	)

	target_link_libraries(${ARG_EXEC_NAME}
			      ${ARG_LINK_LIBS}
	)
endfunction()
