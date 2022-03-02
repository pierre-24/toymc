//
// Created by pierre on 26/02/2022.
//

#ifndef TOYMC_PARAM_FILE_PARSER_H
#define TOYMC_PARAM_FILE_PARSER_H

#include "param_file.h"

typedef enum tm_parf_token_type_ {
    TM_TK_WHITESPACE, // space (U+0020), linefeed (U+000D), carriage return (U+000A), horizontal tab (U+0009)
    TM_TK_DIGIT, // [0-9]
    TM_TK_COMMA, // ","
    TM_TK_DOT, // "."
    TM_TK_LBRACKET, // "["
    TM_TK_RBRACKET, // "]"
    TM_TK_ESCAPE, // "\"
    TM_TK_QUOTE, // "
    TM_TK_DASH, // "-"
    TM_TK_PLUS, // "+"
    TM_TK_COMMENT, // "#"

    TM_TK_CHAR, // anything but what was previously mentioned

    TM_TK_EOS, // → end of string

    TM_TK_LAST
} tm_parf_token_type;

/**
 * @brief Token used by the Lexer. Serve as placeholder of the position.
 * Fields are \code{.c}
 * char* value; // pointer to the char in the string (aka &input[position])
 * tm_parf_token_type type; // type of the token
 * int position; // position in the string
 * int line; // the line (one-based)
 * int pos_in_line; // the position in line (one-based)
 * \endcode
 */
typedef struct tm_parf_token_ {
    char* value;
    tm_parf_token_type type;
    int position;
    int line;
    int pos_in_line;
} tm_parf_token;

int tm_parf_token_init(tm_parf_token* tk, char* input);

/**
 * Error raised by the parser.
 *
 * Fields are \code{.c}
 * char* what; // contains the error message
 * int position; // contains the position to character that raised the error
 * \endcode
 */
typedef struct tm_parf_error_ {
    char* what;
    int position;
} tm_parf_error;

tm_parf_t* tm_parf_loads(char* input, tm_parf_error* error);


#endif //TOYMC_PARAM_FILE_PARSER_H
