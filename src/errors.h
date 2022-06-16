//
// Created by pierre on 5/28/22.
//

#ifndef TOYMC_ERRORS_H
#define TOYMC_ERRORS_H

enum {
    TM_ERR_OK,

    // unexpected
    TM_ERR_MALLOC,
    TM_ERR_READ,
    TM_ERR_NOT_FOUND,

    // module
    TM_ERR_LEXER,
    TM_ERR_PARAMETER_FILE,
    TM_ERR_SIMULATION_PARAMETERS,
    TM_ERR_XYZ,

    TM_ERR_LAST
};

static char* ERROR_EXPLS[] = {
        "Not an error (OK)",

        "malloc() failed",
        "Error while reading file",
        "Not found",

        "Error in lexer",
        "Error in parameter file",
        "Error in simulation parameter",
        "Error in xyz",

        "Not an error (LAST)"
};

void tm_set_debug_level(int level);
void tm_print_debug_msg(char* file, int line, char* format, ...);
void tm_print_warning_msg(char *file, int line, char *format, ...);
void tm_print_error_msg(char* file, int line, char* format, ...);
void tm_print_error_code(char* file, int line, int errcode);

#endif //TOYMC_ERRORS_H
