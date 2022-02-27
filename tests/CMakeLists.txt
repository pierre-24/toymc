include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../src)

# find check
set(CHECK_INSTALL_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../vendor)
find_package(Check REQUIRED)
include_directories(${CHECK_INCLUDE_DIRS})
link_directories(${CHECK_LIBRARY_DIRS})

set(CHECK_EXTRA_LIBS m subunit rt pthread)

# add subdirectories
add_subdirectory(tests_input_file)