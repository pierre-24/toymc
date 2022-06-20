set(LIB_SOURCES
        param_file_objects.c
        param_file_parser.c
        simulation_parameters.c
        pcg32.c
        lexer.c error.c geometry.c xyz_parser.c files.c files.h potentials.c potentials.h)

set(PROG_SOURCES
        main.c)

FILE(GLOB HEADERS *.h)

# library
add_library(toymc STATIC ${LIB_SOURCES} ${HEADERS})
target_compile_options(toymc PRIVATE -Wall -Wextra -Wpedantic)

# executable
add_executable(run_toymc ${PROG_SOURCES} ${HEADERS})
target_link_libraries(run_toymc m toymc)