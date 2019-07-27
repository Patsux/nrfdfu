
cmake_minimum_required(VERSION 3.0)

# Include utils scripts
include(${CMAKE_CURRENT_SOURCE_DIR}/../cmake/modules/utils.cmake)

# If the library name is not specified we use the default one.
if (NOT NRFDFU_LIB_NAME)
	set(NRFDFU_LIB_NAME nrfdfu)
endif()

if (NOT TARGET ${NRFDFU_LIB_NAME})
	utils_add_library(LIB_NAME ${NRFDFU_LIB_NAME}
		SRCS src/nrfdfu_contr.c
		PUBLIC_INCLUDE_DIRS include/
		PRIVATE_INCLUDE_DIRS
		LINK_LIBS
	)
else()
	message(WARNING "Cannot generate the target ${NRFDFU_LIB_NAME} that already exists.")
endif()
