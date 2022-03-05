cmake_minimum_required(VERSION 3.18)
project(toymc
        VERSION 0.1
        DESCRIPTION "Me playing with MPI"
        LANGUAGES C)

set(CMAKE_C_STANDARD 99)

# find check
set(CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

# subdirs
add_subdirectory(src)

# Unit tests
set( MEMORYCHECK_COMMAND_OPTIONS "--trace-children=yes --leak-check=full --error-exitcode=1 --errors-for-leak-kinds=definite,possible,reachable" )
include (CTest)
enable_testing()

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/src)

macro(add_unit_test)
   # macro due to https://bertvandenbroucke.netlify.app/2019/12/12/unit-testing-with-ctest/
   set(oneValueArgs NAME)
   set(multiValueArgs SOURCES LIBS)
   cmake_parse_arguments(TEST "${options}" "${oneValueArgs}"
           "${multiValueArgs}" ${ARGN})
   message(STATUS "generating ${TEST_NAME}")
   add_executable(${TEST_NAME} EXCLUDE_FROM_ALL ${TEST_SOURCES})
   # set_target_properties(${TEST_NAME} PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/rundir/test)
   target_link_libraries(${TEST_NAME} ${TEST_LIBS})

   set(TESTCOMMAND ${TEST_NAME})
   add_test(NAME ${TEST_NAME} COMMAND ${TESTCOMMAND}) # WORKING_DIRECTORY ${PROJECT_BINARY_DIR}/rundir/test)

   set(TESTNAMES ${TESTNAMES} ${TEST_NAME})
endmacro(add_unit_test)

# find check
set(CHECK_INSTALL_DIR ${CMAKE_CURRENT_SOURCE_DIR}/vendor)
find_package(Check REQUIRED)
include_directories(${CHECK_INCLUDE_DIRS})
link_directories(${CHECK_LIBRARY_DIRS})

set(CHECK_EXTRA_LIBS m subunit rt pthread)

# add test
add_unit_test(
        NAME tests_param_file
        SOURCES tests/tests_param_file/tests_param_file.c tests/tests_param_file/main.c tests/tests_param_file/tests_param_file.h
        LIBS toymc ${CHECK_LIBRARIES} ${CHECK_EXTRA_LIBS}
)

add_unit_test(
        NAME tests_param_file_parser
        SOURCES tests/tests_param_file_parser/tests_param_file_parser.c tests/tests_param_file_parser/main.c tests/tests_param_file_parser/tests_param_file_parser.h
        LIBS toymc ${CHECK_LIBRARIES} ${CHECK_EXTRA_LIBS}
)

add_custom_target(checks COMMAND ${CMAKE_CTEST_COMMAND} DEPENDS ${TESTNAMES})
