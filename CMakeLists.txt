cmake_minimum_required(VERSION 3.18)
project(toymc
        VERSION 0.1
        DESCRIPTION "Me playing with MPI"
        LANGUAGES C)

set(CMAKE_C_STANDARD 99)
set(CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

# Library and program
add_subdirectory(src)

# Unit tests
set(MEMORYCHECK_COMMAND_OPTIONS "--trace-children=yes --leak-check=full --error-exitcode=1 --errors-for-leak-kinds=definite,possible,reachable" )
include (CTest)
enable_testing()

add_subdirectory(tests)
