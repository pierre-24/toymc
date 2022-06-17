#include "param_file_parser.h"
#include <ctype.h>
#include <assert.h>

/**
 * Parse a string. Caller \b must free it.
 * @pre \code{.c}
 * tk != NULL && input != NULL
 * && 0 <= tk->position < strlen(input)
 * && tk->type == TM_TK_QUOTE
 * \endcode
 * @param tk valid token
 * @param input input string
 * @param error error, if any
 * @return \p NULL if it was not able to read the string, a pointer to the string otherwise
 */
char *_parse_string(tm_parf_token *tk, char *input) {
    assert(tk != NULL && input != NULL);
    assert(tk->type == TM_TK_QUOTE);

    tm_lexer_eat(tk, input, TM_TK_QUOTE);

    int sz = 0;
    int mul = 64;
    int fac = 1;
    char* tmp = malloc(fac * mul * sizeof(char));
    int escape = 0;

    if(tmp == NULL) {
        return NULL;
    }

    while (tk->type != TM_TK_QUOTE || (escape && tk->type == TM_TK_QUOTE)) {
        if (tk->type == TM_TK_EOS) {
            free(tmp);
            tm_print_error_msg_with_token(__FILE__, __LINE__, tk, "got EOS while reading string");
            return NULL;
        }

        tmp[sz] = *(tk->value);
        sz++;

        if (escape) {
            escape = 0;
        } else if (tk->type == TM_TK_ESCAPE) {
            escape = 1;
        }

        if (sz == fac*mul) {
            fac++;
            tmp = realloc(tmp, fac * mul * sizeof(char));
        }

        tm_lexer_advance(tk, input, 1);
    }

    tm_lexer_eat(tk, input, TM_TK_QUOTE);

    tmp[sz] = '\0';
    return tmp;
}

/**
 * Parse a string: \code string := QUOTE (NOT_QUOTE | BACKSLASH QUOTE)* QUOTE; \endcode
 * @pre \code{.c}
 * tk != NULL && input != NULL
 * && 0 <= tk->position < strlen(input)
 * \endcode
 * @param tk valid token
 * @param input input string
 * @param error error, if any
 * @return \p NULL if there was an error, the object (of type \p TM_T_STRING)  otherwise
 */
tm_parf_t *tm_parf_parse_string(tm_parf_token *tk, char *input) {
    assert(tk != NULL && input != NULL);
    tm_parf_t* object = NULL;

    char* tmp = _parse_string(tk, input);

    if (tmp != NULL) {
        object = tm_parf_string_new(tmp);
        free(tmp);
    }

    return object;
}

/**
 * Parse a number (integer or real):
 * \code
 * NUMBER := INT | REAL;
 * INT := (PLUS | DASH)? DIGIT*;
 * REAL := INT? DOT DIGIT DIGIT* (('E' | 'e') (PLUS | MINUS)? DIGIT DIGIT*)?;
 * \endcode
 * In the end, \p strtol and \p strtod are used to parse the number.
 * @pre \code{.c}
 * tk != NULL && input != NULL
 * && 0 <= tk->position < strlen(input)
 * && (tk->type == TM_TK_DIGIT || tk->type == TM_TK_DASH || tk->type == TM_TK_PLUS || tk->type == TM_TK_DOT)
 * \endcode
 * @param tk valid token
 * @param input input string
 * @param error error, if any
 * @return \p NULL if there was an error, the object (of type \p TM_T_REAL or \p TM_T_INTEGER) otherwise
 */
tm_parf_t *tm_parf_parse_number(tm_parf_token *tk, char *input) {
    assert(tk != NULL && input != NULL);
    assert(tk->type == TM_TK_DIGIT || tk->type == TM_TK_DASH || tk->type == TM_TK_PLUS || tk->type == TM_TK_DOT);

    char* beg = tk->value;
    int beg_pos = tk->position;

    int dot_found = tk->type == TM_TK_DOT;
    int exp_found = 0;

    tm_lexer_advance(tk, input, 1);

    tm_lexer_skip(tk, input, TM_TK_DIGIT);

    if (tk->type == TM_TK_DOT) {
        dot_found = 1;

        tm_lexer_eat(tk, input, TM_TK_DOT);
        tm_lexer_skip(tk, input, TM_TK_DIGIT);
    }

    if (tk->type == TM_TK_ALPHA) {
        if (*(tk->value) == 'e' || *(tk->value) == 'E') {
            exp_found = 1;
            tm_lexer_eat(tk, input, TM_TK_ALPHA);

            if (tk->type == TM_TK_DASH || tk->type == TM_TK_PLUS)
                tm_lexer_advance(tk, input, 1);

            tm_lexer_skip(tk, input, TM_TK_DIGIT);
        }
    }

    char* end;
    tm_parf_t* obj = NULL;
    if (dot_found || exp_found) { // then it is a real
        obj = tm_parf_real_new(strtod(beg, &end));
    } else { // nope, it is an int
        obj = tm_parf_integer_new(strtol(beg, &end, 10));
    }

    if ((int) (end-beg) != tk->position - beg_pos) {
        tm_parf_delete(obj);
        tm_print_error_msg_with_token(__FILE__, __LINE__, tk, "unknown number");
        return NULL;
    }

    return obj;
}

/**
 * Parse a boolean:
 * \code
 * BOOLEAN := "true" | "false" | "yes" | "no";
 * \endcode
 * @pre \code{.c}
 * tk != NULL && input != NULL
 * && 0 <= tk->position < strlen(input)
 * && tk->type == TM_TK_ALPHA
 * \endcode
 * @param tk valid token
 * @param input input string
 * @param error error, if any
 * @return \p NULL if there was an error, the object (of type \p TM_T_BOOLEAN)  otherwise
 */
tm_parf_t *tm_parf_parse_boolean(tm_parf_token *tk, char *input) {
    assert(tk != NULL && input != NULL);
    assert(tk->type == TM_TK_ALPHA);

    char buff[8];
    int i = 0;
    tm_parf_t* obj = NULL;

    while (tk->type == TM_TK_ALPHA && i < 7) {
        buff[i] = *(tk->value);
        tm_lexer_advance(tk, input, 1);
        i++;
    }

    buff[i] = '\0';

    if((i == 2 && strcmp(buff, "on") == 0) || (i == 3 && strcmp(buff, "yes") == 0) || (i == 4 && strcmp(buff, "true") == 0))
        obj = tm_parf_boolean_new(1);
    else if((i == 2 && strcmp(buff, "no") == 0) || (i == 3 && strcmp(buff, "off") == 0) || (i == 5 && strcmp(buff, "false") == 0))
        obj = tm_parf_boolean_new(0);

    return obj;
}

tm_parf_t* tm_parf_parse_value(tm_parf_token* tk, char* input); // forward decl

/**
 * Parse a list
 * \code
 * LIST := LBRACKET (VALUE (COMMA VALUE)*)? RBRACKET;
 * \endcode
 * @pre \code{.c}
 * tk != NULL && input != NULL
 * && 0 <= tk->position < strlen(input)
 * && tk->type == TM_TK_LBRACKET
 * \endcode
 * @param tk valid token
 * @param input input string
 * @param error error, if any
 * @return \p NULL if there was an error, the object (of type \p TM_T_LIST) otherwise
 */
tm_parf_t *tm_parf_parse_list(tm_parf_token *tk, char *input) {
    assert(tk != NULL && input != NULL);
    assert(tk->type == TM_TK_LBRACKET);

    tm_lexer_advance(tk, input, 1); // skip LBRACKET

    tm_parf_t* object = tm_parf_list_new();
    tm_parf_t* val;

    tm_lexer_skip_whitespace_and_nl(tk, input);

    while (tk->type != TM_TK_RBRACKET && tk->type != TM_TK_EOS) {
        val = tm_parf_parse_value(tk, input);

        if (val == NULL) {
            tm_parf_delete(object);
            return NULL;
        } else {
            tm_parf_list_append(object, val);
        }

        tm_lexer_skip_whitespace_and_nl(tk, input);
    }

    if(tm_lexer_eat(tk, input, TM_TK_RBRACKET) != TM_ERR_OK) {
        tm_print_error_msg_with_token(__FILE__, __LINE__, tk, "expected RBRACKET to end STRING");
        tm_parf_delete(object);
        return NULL;
    }

    return object;
}

/**
 * Parse a value
 * \code
 * VALUE := INTEGER | FLOAT | BOOLEAN | STRING | LIST;
 * \endcode
 * @pre \code{.c}
 * tk != NULL && input != NULL
 * && 0 <= tk->position < strlen(input)
 * \endcode
 * @param tk valid token
 * @param input input string
 * @param error error, if any
 * @return \p NULL if there was an error, the object (of correct type) otherwise
 */
tm_parf_t *tm_parf_parse_value(tm_parf_token *tk, char *input) {
    assert(tk != NULL && input != NULL);

    tm_lexer_skip_whitespace_and_nl(tk, input);

    tm_parf_t* object = NULL;

    switch (tk->type) {
        case TM_TK_LBRACKET:
            object = tm_parf_parse_list(tk, input);
            break;
        case TM_TK_QUOTE:
            object = tm_parf_parse_string(tk, input);
            break;
        case TM_TK_DOT:
        case TM_TK_DASH:
        case TM_TK_PLUS:
        case TM_TK_DIGIT:
            object = tm_parf_parse_number(tk, input);
            break;
        case TM_TK_CHAR:
            object = tm_parf_parse_boolean(tk, input);
            break;
        default:
            tm_print_error_msg_with_token(__FILE__, __LINE__, tk, "unexpected token to start value");
            break;
    }

    return object;
}

/**
 * Parse a name literal (i.e., a key). Caller must free the string.
 * \code
 * NAME_LITERAL := (DIGIT | CHAR_LIT)*;
 * CHAR_LIT := [a-zA-Z_-];
 * \endcode
 * @pre \code{.c}
 * tk != NULL && input != NULL
 * && 0 <= tk->position < strlen(input)
 * && tk->type == TM_TK_ALPHA || tk->type == TM_TK_DIGIT || input[tk->position] == '_' || input[tk->position] == '-'
 * \endcode
 * @param tk valid token
 * @param input input string
 * @param error error, if any
 * @return \p NULL if there was an error, the name otherwise.
 */
char *_parse_name_lit(tm_parf_token *tk, char *input) {
    assert(tk != NULL && input != NULL);
    assert(tk->type == TM_TK_ALPHA || tk->type == TM_TK_DIGIT || input[tk->position] == '_' || input[tk->position] == '-');

    int sz = 0;
    int mul = 64;
    int fac = 1;
    char* tmp = malloc(fac * mul * sizeof(char));

    while (tk->type == TM_TK_ALPHA || tk->type == TM_TK_DIGIT || input[tk->position] == '_'  || input[tk->position] == '-') {
        tmp[sz] = *(tk->value);
        sz++;

        if (sz == fac*mul) {
            fac++;
            tmp = realloc(tmp, fac * mul * sizeof(char));
        }

        tm_lexer_advance(tk, input, 1);
    }

    tmp[sz] = '\0';
    return tmp;
}


/**
 * Skip comments
 * \code
 * COMMENT := '#' NOT_NL* NL
 * \endcode
 * @pre \code{.c}
 * tk != NULL && input != NULL
 * && 0 <= tk->position < strlen(input)
 * && tk->type == TM_TK_COMMENT
 * \endcode
 * @param tk valid token
 * @param input input string
 * @return \p TM_ERR_OK if everthing went ok, something else otherwise
 */
void _skip_comment(tm_parf_token *tk, char *input) {
    assert(tk != NULL && input != NULL);
    assert(tk->type == TM_TK_COMMENT);

    while (tk->type != TM_TK_EOS && tk->type != TM_TK_NL) {
        tm_lexer_advance(tk, input, 1);
    }
}

/**
 * Parse a string representing an object.
 * \code
 * OBJECT := (COMMENT | NAME_LITERAL VALUE)* EOS;
 * \endcode
 * @pre \code{.c}
 * input != NULL
 * \endcode
 * @param input the input
 * @param error an eventual error
 * @return \p NULL if there was an error, the object (of type \p TM_T_OBJECT) otherwise
 */
tm_parf_t *tm_parf_loads(char *input) {
    assert(input != NULL);

    tm_parf_token tk;
    tm_parf_t* obj = tm_parf_object_new();

    // bootstrap
    tm_lexer_token_init(&tk, input);
    tm_lexer_skip_whitespace_and_nl(&tk, input);

    // read the stuff
    while(tk.type != TM_TK_EOS) {
        if(tk.type == TM_TK_COMMENT) {
            _skip_comment(&tk, input);
            tm_lexer_skip_whitespace_and_nl(&tk, input);
        }
        else {
            char* key = _parse_name_lit(&tk, input);
            if(tk.type != TM_TK_WHITESPACE) {
                tm_print_error_msg_with_token(__FILE__, __LINE__, &tk, "expected whitespace after key");
            }

            tm_lexer_skip_whitespace_and_nl(&tk, input);

            tm_parf_t* value = tm_parf_parse_value(&tk, input);
            if (value == NULL) {
                free(key);
                tm_parf_delete(obj);
                return NULL;
            }

            tm_parf_object_set(obj, key, value);
            free(key);
        }

        tm_lexer_skip_whitespace_and_nl(&tk, input);
    }

    return obj;
}
