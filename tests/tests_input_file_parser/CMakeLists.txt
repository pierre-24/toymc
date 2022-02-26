set(TEST_FILENAME "tests_input_file_parser")
set(TEST_DESCRIPTION "tests: input_file_parser")

configure_file(../main.c.in ${CMAKE_CURRENT_SOURCE_DIR}/main.c)
configure_file(../tests.h.in ${CMAKE_CURRENT_SOURCE_DIR}/tests.h)

# program
file(GLOB TEST_SOURCES "*.c")

add_executable(tests_input_file_parser ${TEST_SOURCES})
target_link_libraries(tests_input_file_parser toymc ${CHECK_LIBRARIES})