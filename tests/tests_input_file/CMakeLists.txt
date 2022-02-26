# find check
find_package(Check REQUIRED)
include_directories(${CHECK_INCLUDE_DIRS})
link_directories(${CHECK_LIBRARY_DIRS})

# add files
set(TEST_FILENAME "tests_input_file")
set(TEST_DESCRIPTION "tests: input_file")

configure_file(../main.c.in ${CMAKE_CURRENT_SOURCE_DIR}/main.c)
configure_file(../tests.h.in ${CMAKE_CURRENT_SOURCE_DIR}/tests.h)

# program
file(GLOB TEST_SOURCES "*.c")

add_executable(tests_input_file ${TEST_SOURCES})
target_link_libraries(tests_input_file toymc m ${CHECK_LIBRARIES})