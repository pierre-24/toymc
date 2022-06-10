include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../src)

### Macro to add the test and setup everything
### (due to https://bertvandenbroucke.netlify.app/2019/12/12/unit-testing-with-ctest/)
macro(add_unit_test)
    set(oneValueArgs NAME)
    set(multiValueArgs SOURCES LIBS)
    cmake_parse_arguments(TEST "${options}" "${oneValueArgs}"
            "${multiValueArgs}" ${ARGN})
    message(STATUS "generating ${TEST_NAME}")
    add_executable(${TEST_NAME} EXCLUDE_FROM_ALL ${TEST_SOURCES})
    set_target_properties(${TEST_NAME} PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/rundir/test)
    target_link_libraries(${TEST_NAME} ${TEST_LIBS})

    set(TESTCOMMAND ${TEST_NAME})
    add_test(NAME ${TEST_NAME} COMMAND ${TESTCOMMAND} WORKING_DIRECTORY ${PROJECT_BINARY_DIR}/rundir/test)

    set(TESTNAMES ${TESTNAMES} ${TEST_NAME})
endmacro(add_unit_test)

### find check
set(CHECK_INSTALL_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../vendor)
find_package(Check REQUIRED)
include_directories(${CHECK_INCLUDE_DIRS})
link_directories(${CHECK_LIBRARY_DIRS})

set(CHECK_EXTRA_LIBS m subunit rt pthread)

### add tests

# -- test_param_file
add_unit_test(
        NAME tests_param_file
        SOURCES tests_param_file_objects.c
        LIBS toymc ${CHECK_LIBRARIES} ${CHECK_EXTRA_LIBS}
)

# -- tests_lexer
add_unit_test(
        NAME tests_lexer
        SOURCES tests_lexer.c
        LIBS toymc ${CHECK_LIBRARIES} ${CHECK_EXTRA_LIBS}
)

# -- test_param_file_parser
add_unit_test(
        NAME tests_param_file_parser
        SOURCES tests_param_file_parser.c
        LIBS toymc ${CHECK_LIBRARIES} ${CHECK_EXTRA_LIBS}
)

# -- test_simulation_parameters
configure_file(
        ${CMAKE_CURRENT_SOURCE_DIR}/test_dummy_input.inp
        ${PROJECT_BINARY_DIR}/rundir/test/test_dummy_input.inp
        COPYONLY
)

add_unit_test(
        NAME tests_simulation_parameters
        SOURCES tests_simulation_parameters.c
        LIBS toymc ${CHECK_LIBRARIES} ${CHECK_EXTRA_LIBS}
)

## add an extra "check" target
add_custom_target(checks COMMAND ${CMAKE_CTEST_COMMAND} DEPENDS ${TESTNAMES})
add_custom_target(build_checks COMMAND true DEPENDS ${TESTNAMES})