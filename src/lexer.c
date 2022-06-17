#include <ctype.h>
#include <assert.h>
#include <stdarg.h>

#include "param_file_parser.h"
#include "lexer.h"

int lexer_translator[] = {
        0x20, TM_TK_WHITESPACE,
        0x0a, TM_TK_CR,
        0x0d, TM_TK_NL,
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
 * Initialize the token \p tk with the first character of \p input.
 * @pre \code{.c}
 * tk != NULL && input != NULL
 * && strlen(input) >= 0 // not checked
 * \endcode
 * @param tk the token
 * @post \p tk is initialized.
 * @return \p TM_ERR_OK if everything went well, something else otherwise
 */
int tm_lexer_token_init(tm_parf_token* tk, char* input) {
    assert(tk != NULL && input != NULL);

    tk->position = 0;
    tk->line = 1;
    tk->pos_in_line = 0;

    return tm_lexer_advance(tk, input, 0);
}

/**
 * Set the token according to the character at \p position
 * @pre \code{.c}
 * tk != NULL && input != NULL && (shift == 0 || shift == 1)
 * && 0 <= tk->position + shift < strlen(input) // not checked
 * \endcode
 * @param tk token object
 * @param input input string
 * @param shift shift with respect to the current position (should be 0 or 1)
 * @return \p TM_ERR_OK
 */
int tm_lexer_advance(tm_parf_token *tk, char *input, int shift) {
    assert(tk != NULL && input != NULL);
    assert(shift == 0 || shift == 1);

    if (tk->type == TM_TK_EOS)
        return TM_ERR_OK;

    char c = input[tk->position + shift];
    tm_parf_token_type t = TM_TK_CHAR;

    if(isalpha(c))
        t = TM_TK_ALPHA;
    else {
        int *tr = lexer_translator;
        while (*tr != -1) {
            if (c == *tr) {
                t = *(tr + 1);
                break;
            }

            tr += 2;
        }
    }

    tk->position += shift;
    tk->type = t;
    tk->value = input + tk->position;

    if(tk->type == TM_TK_NL) {
        tk->line += 1;
        tk->pos_in_line = 0;
    } else {
        tk->pos_in_line += shift;
    }

    return TM_ERR_OK;
}

/**
 * Advance to the next token if the current one if of type \p t
 * @pre \code{.c}
 * tk != NULL && input != NULL
 * && 0 <= tk->position < strlen(input) // not checked
 * \endcode
 * @param tk valid token
 * @param input input string
 * @param t type
 * @post if token is of type \p t, advance to the next token
 * @return \p TM_ERR_OK if token was of type \p t, something else otherwise
 */
int tm_lexer_eat(tm_parf_token *tk, char *input, tm_parf_token_type t) {
    assert(tk != NULL && input != NULL);

    if (tk->type != t)
        return TM_ERR_LEXER;

    return tm_lexer_advance(tk, input, 1);
}

/**
 * Advance to the next token until one is of type \p t.
 * @pre \code{.c}
 * tk != NULL && input != NULL
 * && 0 <= tk->position < strlen(input) // not checked
 * \endcode
 * @param tk valid token
 * @param input input string
 * @param t type
 * @post the token is of type \p t
 * @return \p TM_ERR_OK if token was of type \p t, something else otherwise
 */
int tm_lexer_skip(tm_parf_token *tk, char *input, tm_parf_token_type t) {
    assert(tk != NULL && input != NULL);

    while (tk->type == t)
        tm_lexer_advance(tk, input, 1);

    return TM_ERR_OK;
}

/**
 * Skip \p TM_TK_NL and \p TM_TK_WHITESPACE
 * @pre \code{.c}
 * tk != NULL && input != NULL
 * && 0 <= tk->position < strlen(input) // not checked
 * \endcode
 * @param tk valid token
 * @param input input string
 * @param t type
 * @post the token is of type \p t
 * @return \p TM_ERR_OK if token was of type \p t, something else otherwise
 */
int tm_lexer_skip_whitespace_and_nl(tm_parf_token *tk, char *input) {
    assert(tk != NULL && input != NULL);

    while (tk->type == TM_TK_WHITESPACE || tk->type == TM_TK_NL) {
        tm_lexer_advance(tk, input, 1);
    }

    return TM_ERR_OK;
}

/**
 * Print an error message in \p stderr, but including the current token.
 * @pre \code{.c}
 * file != NULL && line >= 0
 * \endcode
 * @param file source file (use \p __FILE__ )
 * @param line line (use \p __LINE__)
 * @param tk A valid token
 * @param format format of the string
 * @param ... extra parameters
 */
void tm_print_error_msg_with_token(char *file, int line, tm_parf_token* tk, char *format, ...) {
    assert(file != NULL && tk != NULL && format != NULL);

    va_list arglist;

    fprintf(stderr, "ERROR (%s:%d) :: ", file, line);
    fprintf(stderr, "Token `%c` (file:%d:%d) :: ", (*(tk->value) == '\0') ? '0' : *(tk->value), tk->line, tk->pos_in_line);
    va_start(arglist, format);
    vfprintf(stderr, format, arglist);
    va_end(arglist);
    fprintf(stderr, "\n");
}
