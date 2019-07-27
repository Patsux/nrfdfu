
cmake_minimum_required(VERSION 3.0)

set(NRFDFU_SRC_DIR "../../lib")

# Add the NRFDFU library
set(NRFDFU_LIB_NAME nrfdfu)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/${NRFDFU_SRC_DIR} "nrfdfu-build")

# Set the name of the executable.
set(EXEC_NAME "nrfdfu_linux_serial")

# Set the list of application sources.
set(EXEC_SRCS "app.c")

# Create the executable.
add_executable(${EXEC_NAME} ${EXEC_SRCS})

# Link Zefus to the executable.
target_link_libraries(${EXEC_NAME}
                      ${NRFDFU_LIB_NAME})
