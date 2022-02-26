//
// Created by pierre on 26/02/2022.
//

#include "input_file_parser.h"

int lexer_translator[] = {
        0x20, TM_TK_WHITESPACE,
        0x0d, TM_TK_WHITESPACE,
        0x0a, TM_TK_WHITESPACE,
        0x09, TM_TK_WHITESPACE,
        '0',  TM_TK_DIGIT,
        '1',  TM_TK_DIGIT,
        '2',  TM_TK_DIGIT,
        '3',  TM_TK_DIGIT,
        '4',  TM_TK_DIGIT,
        '5',  TM_TK_DIGIT,
        '6',  TM_TK_DIGIT,
        '7',  TM_TK_DIGIT,
        '8',  TM_TK_DIGIT,
        '9',  TM_TK_DIGIT,
        '#',  TM_TK_COMMENT,
        ',',  TM_TK_COMMA,
        '.',  TM_TK_DOT,
        '[',  TM_TK_LBRACKET,
        ']',  TM_TK_RBRACKET,
        '\\', TM_TK_ESCAPE,
        '"',  TM_TK_QUOTE,
        '-',  TM_TK_DASH,
        '+',  TM_TK_PLUS,
        '\0', TM_TK_EOS,

        -1
};
