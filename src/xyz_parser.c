#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "xyz_parser.h"

/**
 * Parse a (simple) real (without scientific notation):
 * \code
 * FLOAT := (PLUS | DASH)? DIGIT* DOT DIGIT*;
 * \endcode
 * In the end \p strtod is used.
 * @pre \code{.c}
 * tk != NULL && input != NULL
 * && 0 <= tk->position < strlen(input)
 * \endcode
 * @param tk valid token
 * @param input input string
 * @param out output output value
 * @post \p out is set
 * @return \p TM_ERR_OK if the value is set, something else otherwise
 */
int tm_xyz_parse_real(tm_parf_token* tk, char* input, double *out) {
    char* beg = tk->value;
    int beg_pos = tk->position;

    // starts with (DIGIT | DASH | PLUS | DOT)
    if (tm_lexer_eat(tk, input, TM_TK_DIGIT) != TM_ERR_OK
        && tm_lexer_eat(tk, input, TM_TK_DASH) != TM_ERR_OK
        && tm_lexer_eat(tk, input, TM_TK_PLUS) != TM_ERR_OK
        && tm_lexer_eat(tk, input, TM_TK_DOT) != TM_ERR_OK) {
        return TM_ERR_XYZ_NOT_A_FLOAT;
    }

    // move
    tm_lexer_skip(tk, input, TM_TK_DIGIT);

    // get DOT
    if (tk->type == TM_TK_DOT) {
        tm_lexer_eat(tk, input, TM_TK_DOT);
        tm_lexer_skip(tk, input, TM_TK_DIGIT);
    }

    char* end;
    *out = strtod(beg, &end);

    if ((int) (end-beg) != tk->position - beg_pos) {
        return TM_ERR_XYZ_NOT_A_FLOAT;
    }

    return TM_ERR_OK;
}

/**
 * Parse a positive integer
 * @pre \code{.c}
 * tk != NULL && input != NULL
 * && 0 <= tk->position < strlen(input)
 * \endcode
 * In the end \p strtol is used.
 * @param tk valid token
 * @param input input string
 * @param out output
 * @post \p out is set
 * @return \p TM_ERR_OK if the value is set, something else otherwise
 */
int tm_xyz_parse_positive_int(tm_parf_token* tk, char* input, long *out) {
    char* beg = tk->value;
    int beg_pos = tk->position;

    // starts with a DIGIT?
    if (tm_lexer_eat(tk, input, TM_TK_DIGIT) != TM_ERR_OK) {
        return TM_ERR_XYZ_NOT_AN_INT;
    }

    // move
    tm_lexer_skip(tk, input, TM_TK_DIGIT);

    char* end;
    *out = strtol(beg, &end, 10);

    if ((int) (end-beg) != tk->position - beg_pos) {
        return TM_ERR_XYZ_NOT_AN_INT;
    }

    return TM_ERR_OK;
}

/**
 * Parse a string which represent a valid XYZ file
 * @pre \code{.c}
 * input != NULL
 * \endcode
 * @param input the input
 * @return \p NULL if there was an error, the geometry otherwise
 */
tm_geometry *tm_xyz_loads(char *input) {
    tm_parf_token tk;

    // bootstrap
    tm_lexer_token_init(&tk, input);
    tm_lexer_skip(&tk, input, TM_TK_WHITESPACE);

    // read number of atoms
    long N = 0;

    int r = tm_xyz_parse_positive_int(&tk, input, &N);
    if(r != TM_ERR_OK) {
        tm_print_error_code(__FILE__, __LINE__, r);
        return NULL;
    }

    // create geometry
    tm_geometry * g = tm_geometry_new(N);
    if(g == NULL) {
        tm_print_error_code(__FILE__, __LINE__, TM_ERR_MALLOC);
        return NULL;
    }

    tm_lexer_skip(&tk, input, TM_TK_WHITESPACE);
    if(tm_lexer_eat(&tk, input, TM_TK_NL) != TM_ERR_OK) {
        tm_print_error_msg(__FILE__, __LINE__, "error while reading XYZ file: no title and coordinate");
        tm_geometry_delete(g);
        return NULL;
    }

    int line = 1;

    return g;
}
