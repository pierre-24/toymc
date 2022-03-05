
# add files
set(TEST_FILENAME "tests_simulation_parameters")
set(TEST_DESCRIPTION "tests: simulation_parameters")

configure_file(../main.c.in ${CMAKE_CURRENT_SOURCE_DIR}/main.c)
configure_file(../tests.h.in ${CMAKE_CURRENT_SOURCE_DIR}/${TEST_FILENAME}.h)