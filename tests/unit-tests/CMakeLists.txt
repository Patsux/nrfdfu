
cmake_minimum_required(VERSION 3.0)

# Include cmake module.
include(${CMAKE_CURRENT_SOURCE_DIR}/../../cmake/modules/utils.cmake)

set(NRFDFU_SRC_DIR "../../lib")

# Add some global compiler flags that will be used for all targets.
add_compile_options(-Wall -Wextra -Wno-unused-function -Wno-unused-parameter)

# Add the NRFDFU library
set(NRFDFU_LIB_NAME nrfdfu)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/${NRFDFU_SRC_DIR} "nrfdfu-build")

utils_add_ut(EXEC_NAME nrfdfu-ut
	     SRCS src/main.c
	          src/controller/test-controller.c
	     PUBLIC_INCLUDE_DIRS
	     PRIVATE_INCLUDE_DIRS include
	     LINK_LIBS ${NRFDFU_LIB_NAME}
)

# Compile and execute UT to generate coverage report
# These targets are automatically created in add_lib_ut().
add_custom_target(coverage DEPENDS coverage-nrfdfu-ut)

