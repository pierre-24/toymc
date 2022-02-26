cmake_minimum_required(VERSION 3.21)
project(toymc
        VERSION 0.1
        DESCRIPTION "Me playing with MPI"
        LANGUAGES C)

set(CMAKE_C_STANDARD 99)

include_directories(${PROJECT_SOURCE_DIR}/include)

file(GLOB SOURCES "src/*.c")

add_executable(toymc ${SOURCES})
target_link_libraries(toymc m)
target_compile_options(toymc PRIVATE -Wall -Wextra -Wpedantic)
