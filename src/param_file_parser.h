#ifndef TOYMC_PARAM_FILE_PARSER_H
#define TOYMC_PARAM_FILE_PARSER_H

#include "param_file_objects.h"
#include "lexer.h"

/**
 * Error raised by the parser.
 *
 * Fields are \code{.c}
 * char* what; // contains the error message
 * int position; // contains the position to character that raised the error
 * int line; // line where the error was raised
 * int pos_in_line; // position in line
 * \endcode
 */
typedef struct tm_parf_error_ {
    char* what;
    int position;
    int line;
    int pos_in_line;
} tm_parf_error;

tm_parf_t* tm_parf_loads(char* input, tm_parf_error* error);


#endif //TOYMC_PARAM_FILE_PARSER_H
