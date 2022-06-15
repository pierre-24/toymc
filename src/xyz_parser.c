#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

#include "xyz_parser.h"

/**
 * Parse a (simple) real (without scientific notation):
 * \code
 * FLOAT := (PLUS | DASH)? DIGIT* DOT DIGIT*;
 * \endcode
 * In the end \p strtod is used.
 * @pre \code{.c}
 * tk != NULL && input != NULL && out != NULL
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
 * tk != NULL && input != NULL && out != NULL
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
 * Parse an atom type \code
 * ATOM_TYPE := ALPHA (ALPHA | DIGIT)*
 * \endcode
 * Caller is responsible to free the result
 * @pre \code{.c}
 * tk != NULL && input != NULL && out != NULL
 * && 0 <= tk->position < strlen(input)
 * \endcode
 * @param tk valid token
 * @param input input string
 * @param out output
 * @post \p out is set
 * @return \p TM_ERR_OK if the value is set, something else otherwise
 */
int tm_xyz_parse_atom_type(tm_parf_token* tk, char* input, char **out) {
    int pos_start = tk->position;

    if(tk->type != TM_TK_ALPHA) // start with ALPHA?
        return TM_ERR_XYZ_NOT_ATOM_TYPE;

    while((tk->type == TM_TK_ALPHA || tk->type == TM_TK_DIGIT) && tk->type != TM_TK_EOS)
        tm_lexer_advance(tk, input, 1);

    if(tk->position == pos_start) // read nothing?
        return TM_ERR_XYZ_NOT_ATOM_TYPE;

    *out = malloc((tk->position - pos_start + 1) * sizeof(char));
    if(*out == NULL)
        return TM_ERR_MALLOC;

    memcpy(*out, input + pos_start, tk->position - pos_start);
    (*out)[tk->position - pos_start] = '\0';

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

    tm_print_debug_msg(__FILE__, __LINE__, "XYZ should contain %d atoms", N);

    // create geometry
    tm_geometry * g = tm_geometry_new(N);
    if(g == NULL) {
        tm_print_error_code(__FILE__, __LINE__, TM_ERR_MALLOC);
        return NULL;
    }

    tm_lexer_skip(&tk, input, TM_TK_WHITESPACE);
    if(tm_lexer_eat(&tk, input, TM_TK_NL) != TM_ERR_OK) {
        tm_print_error_msg(__FILE__, __LINE__, "error while reading XYZ file: no title and coordinates");
        tm_geometry_delete(g);
        return NULL;
    }

    // read title
    int pos_start =  tk.position;
    while (tk.type != TM_TK_NL && tk.type != TM_TK_EOS)
        tm_lexer_advance(&tk, input, 1);

    char* title = malloc((tk.position - pos_start + 1) * sizeof (char));
    if(title == NULL) {
        tm_print_error_code(__FILE__, __LINE__, TM_ERR_MALLOC);
        tm_geometry_delete(g);
        return NULL;
    }

    memcpy(title, input + pos_start, tk.position - pos_start);
    title[tk.position - pos_start] = '\0';

    tm_print_debug_msg(__FILE__, __LINE__, "Title of XYZ is `%s`", title);
    free(title);

    if(tm_lexer_eat(&tk, input, TM_TK_NL) != TM_ERR_OK) {
        tm_print_error_msg(__FILE__, __LINE__, "error while reading XYZ file: no coordinates");
        tm_geometry_delete(g);
        return NULL;
    }

    // read coordinates
    int atom_i = 0;
    int type_i;
    char* atom_type;
    double x, y, z;

    while(atom_i < N && tk.type != TM_TK_EOS) {
        // atom type
        tm_lexer_skip(&tk, input, TM_TK_WHITESPACE);
        r = tm_xyz_parse_atom_type(&tk, input, &atom_type);
        if(r != TM_ERR_OK)
            break;

        tm_print_debug_msg(__FILE__, __LINE__, "Read atom %s on line %d", atom_type, tk.line);
        type_i = 0;
        while(type_i < N) {
            if(g->type_vals[type_i] == NULL) {
                g->type_vals[type_i] = atom_type;
                g->types[atom_i] = type_i;
                break;
            }
            else if(strcmp(atom_type, g->type_vals[type_i]) == 0) {
                g->types[atom_i] = type_i;
                free(atom_type); // no need for that anymore
                break;
            }
            type_i++;
        }

        // coordinates
        tm_lexer_skip(&tk, input, TM_TK_WHITESPACE);
        r = tm_xyz_parse_real(&tk, input, &x);
        if(r != TM_ERR_OK)
            break;

        tm_lexer_skip(&tk, input, TM_TK_WHITESPACE);
        r = tm_xyz_parse_real(&tk, input, &y);
        if(r != TM_ERR_OK)
            break;

        tm_lexer_skip(&tk, input, TM_TK_WHITESPACE);
        r = tm_xyz_parse_real(&tk, input, &z);
        if(r != TM_ERR_OK)
            break;

        g->positions[0 * g->N + atom_i] = x;
        g->positions[1 * g->N + atom_i] = y;
        g->positions[2 * g->N + atom_i] = z;

        atom_i++;
        tm_lexer_skip(&tk, input, TM_TK_WHITESPACE);

        if(atom_i < N) {
            if(tm_lexer_eat(&tk, input, TM_TK_NL)) {
                r = TM_ERR_XYZ_TOO_SHORT;
                break;
            }
        }
    }

    if (r != TM_ERR_OK) {
        tm_print_error_msg(__FILE__, __LINE__, "Error while reading XYZ (on line %d) -- %s", tk.line, ERROR_EXPLS[r]);
        tm_geometry_delete(g);
        return NULL;
    }

    // too long?
    tm_lexer_skip_whitespace_and_nl(&tk, input);

    if(tk.type != TM_TK_EOS) {
        tm_print_error_msg(__FILE__, __LINE__, "Error while reading XYZ (on line %d) -- more atom than expected!", tk.line);
        tm_geometry_delete(g);
        return NULL;
    }

    return g;
}
