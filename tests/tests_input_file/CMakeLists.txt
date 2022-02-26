
# add files
set(TEST_FILENAME "tests_input_file")
set(TEST_DESCRIPTION "tests: input_file")

configure_file(../main.c.in ${CMAKE_CURRENT_SOURCE_DIR}/main.c)
configure_file(../tests.h.in ${CMAKE_CURRENT_SOURCE_DIR}/tests.h)

# program
file(GLOB TEST_SOURCES "*.c")

add_executable(${TEST_FILENAME} ${TEST_SOURCES})
target_link_libraries(${TEST_FILENAME} toymc ${CHECK_LIBRARIES} ${CHECK_EXTRA_LIBS})