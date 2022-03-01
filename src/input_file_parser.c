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

/**
 * Set the token according to the character at \p position
 * @pre \code{.c}
 * tk != NULL && input != NULL
 * && 0 <= position < strlen(input)
 * \endcode
 * @param tk token object
 * @param input input string
 * @param position position
 * @return 0 if everything went well, something else otherwise
 */
int tm_infi_lexer(tm_infi_token *tk, char *input, int position) {
    if (tk == NULL || input == NULL)
        return -1;

    if (position < 0)
        return -1;

    char c = input[position];
    tm_infi_token_type t = TM_TK_CHAR;

    int* tr = lexer_translator;
    while (*tr != -1) {
        if (c == *tr) {
            t = *(tr+1);
            break;
        }

        tr += 2;
    }

    tk->position = position;
    tk->type = t;
    tk->value = input+position;
    tk->size = 1;

    return 0;
}

/**
 * Advance to the next token if the current one if of type \p t
 * @pre \code{.c}
 * tk != NULL && input != NULL
 * \endcode
 * @param tk valid token
 * @param input input string
 * @param t type
 * @post if token is of type \p t, advance to the next token
 * @return 0 if token was of type \p t, something else otherwise
 */
int _eat(tm_infi_token *tk, char *input, tm_infi_token_type t) {
    if (tk->type != t)
        return -1;

    return tm_infi_lexer(tk, input, tk->position + 1);
}

/**
 * Advance to the next token until one is of type \p t.
 * @pre \code{.c}
 * tk != NULL && input != NULL
 * \endcode
 * @param tk valid token
 * @param input input string
 * @param t type
 * @post the token is of type \p t
 * @return 0 if token was of type \p t, something else otherwise
 */
int _skip(tm_infi_token *tk, char *input, tm_infi_token_type t) {
    int r = 0;

    while (tk->type == t) {
        r = tm_infi_lexer(tk, input, tk->position + 1);
        if (r < 0)
            return r;
    }

    return 0;
}

/**
 * Parse a string. Caller \b must free it.
 * @pre \code{.c}
 * tk != NULL && input != NULL && error != NULL
 * \endcode
 * @param tk valid token
 * @param input input string
 * @param error error, if any
 * @return \p NULL if it was not able to read the string, a pointer to the string otherwise
 */
char* _parse_string(tm_infi_token* tk, char* input, tm_infi_error* error) {
    if (_eat(tk, input, TM_TK_QUOTE) != 0) {
        error->what = "expected quote at beginning of string";
        error->position = tk->position;
        return NULL;
    }

    int sz = 0;
    int mul = 64;
    int fac = 1;
    char* tmp = malloc(fac * mul * sizeof(char));
    int escape = 0;

    while (tk->type != TM_TK_QUOTE || (escape && tk->type == TM_TK_QUOTE)) {
        if (tk->type == TM_TK_EOS) {
            free(tmp);
            error->what = "got EOS while getting string";
            error->position = tk->position;
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

        tm_infi_lexer(tk, input, tk->position + 1);
    }

    _eat(tk, input, TM_TK_QUOTE);

    tmp[sz] = '\0';
    return tmp;
}

/**
 * Parse a string: \code string := QUOTE (NOT_QUOTE | BACKSLASH QUOTE)* QUOTE; \endcode
 * @pre \code{.c}
 * tk != NULL && input != NULL
 * \endcode
 * @param tk valid token
 * @param input input string
 * @param error error, if any
 * @return NULL if there was an error, the object (of type \p TM_INFI_STRING)  otherwise
 */
tm_infi_t* tm_infi_parse_string(tm_infi_token* tk, char* input, tm_infi_error* error) {
    tm_infi_t* object = NULL;

    char* tmp = _parse_string(tk, input, error);

    if (tmp != NULL) {
        object = tm_infi_string_new(tmp);
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
 * tk != NULL && input != NULL && error != NULL
 * \endcode
 * @param tk valid token
 * @param input input string
 * @param error error, if any
 * @return NULL if there was an error, the object (of type \p TM_INFI_REAL or \p TM_INFI_INTEGER) otherwise
 */
tm_infi_t* tm_infi_parse_number(tm_infi_token* tk, char* input, tm_infi_error* error) {
    char* beg = tk->value;
    int beg_pos = tk->position;

    int dot_found = tk->type == TM_TK_DOT;
    int exp_found = 0;

    if (_eat(tk, input, TM_TK_DIGIT) != 0 && _eat(tk, input, TM_TK_DASH) && _eat(tk, input, TM_TK_PLUS) && _eat(tk, input, TM_TK_DOT)) {
        error->what = "expected digit/dash/plus/dot to begin a number";
        error->position = tk->position;
        return NULL;
    }

    _skip(tk, input, TM_TK_DIGIT);

    if (tk->type == TM_TK_DOT) {
        dot_found = 1;

        _eat(tk, input, TM_TK_DOT);
        _skip(tk, input, TM_TK_DIGIT);
    }

    if (tk->type == TM_TK_CHAR) {
        if (*(tk->value) == 'e' || *(tk->value) == 'E') {
            exp_found = 1;
            _eat(tk, input, TM_TK_CHAR);

            if (tk->type == TM_TK_DASH || tk->type == TM_TK_PLUS)
                tm_infi_lexer(tk, input, tk->position + 1);

            _skip(tk, input, TM_TK_DIGIT);
        }
    }

    char* end;
    tm_infi_t* obj = NULL;
    if (dot_found || exp_found) { // then it is a real
        obj = tm_infi_real_new(strtod(beg, &end));
    } else { // nope, it is an int
        obj = tm_infi_integer_new((int) strtol(beg, &end, 10));
    }

    if ((int) (end-beg) != tk->position - beg_pos) {
        tm_infi_delete(obj);
        error->what = "error while parsing number";
        error->position = tk->position;
        return NULL;
    }

    return obj;
}

/**
 * Parse a boolean:
 * \code
 * BOOLEAN := "true" | "false";
 * \endcode
 * @pre \code{.c}
 * tk != NULL && input != NULL && error != NULL
 * \endcode
 * @param tk valid token
 * @param input input string
 * @param error error, if any
 * @return NULL if there was an error, the object (of type \p TM_INFI_BOOLEAN)  otherwise
 */
tm_infi_t* tm_infi_parse_boolean(tm_infi_token* tk, char* input, tm_infi_error* error) {
    if (tk->type != TM_TK_CHAR) {
        error->what = "expected a character for boolean";
        error->position = tk->position;
        return NULL;
    }

    char* expected;
    tm_infi_t* object = NULL;

    if (input[tk->position] == 't') { // true ?
        expected = "true";
        object = tm_infi_boolean_new(1);
    } else if (input[tk->position] == 'f') { // false ?
        expected = "false";
        object = tm_infi_boolean_new(0);
    } else {
        error->what = "expected a character for boolean";
        error->position = tk->position;
        return NULL;
    }

    if(object != NULL) {
        int i = 0;

        while (expected[i] != '\0') {
            if (tk->type != TM_TK_CHAR || *(tk->value) != expected[i]) {
                tm_infi_delete(object);
                error->what = "unexpected token in boolean";
                error->position = tk->position;
                return NULL;
            }

            tm_infi_lexer(tk, input, tk->position + 1);
            i++;
        }
    }

    return object;
}
