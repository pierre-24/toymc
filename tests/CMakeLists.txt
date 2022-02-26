# find check
find_package(Check REQUIRED)
include_directories(${CHECK_INCLUDE_DIRS})
link_directories(${CHECK_LIBRARY_DIRS})

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../src)

# add subdirectories
add_subdirectory(tests_input_file)
add_subdirectory(tests_input_file_parser)