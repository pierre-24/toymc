
# add files
set(TEST_FILENAME "tests_param_file_parser")
set(TEST_DESCRIPTION "tests: param_file_parser")

configure_file(../main.c.in ${CMAKE_CURRENT_SOURCE_DIR}/main.c)
configure_file(../tests.h.in ${CMAKE_CURRENT_SOURCE_DIR}/${TEST_FILENAME}.h)