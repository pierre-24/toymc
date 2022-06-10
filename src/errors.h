//
// Created by pierre on 5/28/22.
//

#ifndef TOYMC_ERRORS_H
#define TOYMC_ERRORS_H

enum {
    TM_ERR_OK,

    // unexpected
    TM_ERR_MALLOC,

    // logic (should not happen)
    TM_ERR_PARAM_NULL,
    TM_ERR_API,

    // file
    TM_ERR_READ,

    // lexer
    TM_ERR_LEXER_SHIFT,
    TM_ERR_LEXER_UNEXPECTED_TOKEN,

    // parf
    TM_ERR_PARF,
    TM_ERR_PARF_NCHECK,
    TM_ERR_PARF_KNF,
    TM_ERR_PARF_UNEXPECTED_TYPE,
    TM_ERR_PARF_LIST_SIZE,

    TM_ERR_LAST
};

static char* error_expl[] = {
        "Not an error (OK)",

        "malloc() failed",

        "One of the parameter is NULL (STH)",
        "Programmer error (STH)",

        "Error while reading file",

        "Inconsistent shift (LEXER)",
        "Unexpected token (LEXER)",

        "Error in PARF",
        "Unexpected type (PARF)",
        "Key not found (PARF)",
        "Unexpected type (PARF)",
        "List size problem (PARF)",

        "Not an error (LAST)"
};

void tm_set_debug_level(int level);
void tm_print_debug_msg(char* file, int line, char* format, ...);
void tm_print_warning_msg(char *file, int line, char *format, ...);
void tm_print_error_msg(char* file, int line, char* format, ...);
void tm_print_error_code(char* file, int line, int errcode);

#endif //TOYMC_ERRORS_H
