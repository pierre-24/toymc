//
// Created by pierre on 26/02/2022.
//

#ifndef TOYMC_INPUT_FILE_PARSER_H
#define TOYMC_INPUT_FILE_PARSER_H

#include "input_file.h"

typedef enum tm_infi_token_type_ {
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
} tm_infi_token_type;

typedef struct tm_infi_token_ {
    char* value;
    tm_infi_token_type type;
    int position;
    int size;
} tm_infi_token;

typedef struct tm_infi_error_ {
    char* what;
    int position;
} tm_infi_error;

tm_infi_t* tm_infi_loads(char* input, tm_infi_error* error);


#endif //TOYMC_INPUT_FILE_PARSER_H
