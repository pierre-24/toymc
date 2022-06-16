#include <stdio.h>
#include <stdarg.h>
#include <assert.h>

#include "errors.h"

int DEBUG_LVL = 0;

void tm_set_debug_level(int level) {
    DEBUG_LVL = level;
}

/**
 * Print a debug message in \p stdout, if \p DEBUG_LVL is above 0.
 * @pre \code{.c}
 * file != NULL && format != NULL
 * \endcode
 * @param file source file (use \p __FILE__ )
 * @param line line (use \p __LINE__)
 * @param format format of the string
 * @param ... extra parameters
 */
void tm_print_debug_msg(char *file, int line, char *format, ...) {
    assert(file != NULL && format != NULL);

    if(DEBUG_LVL < 1)
        return;

    va_list arglist;

    printf("DEBUG (%s:%d) :: ", file, line);
    va_start(arglist, format);
    vprintf(format, arglist);
    va_end(arglist);
    printf("\n");
}

/**
 * Print a warning message in \p stdout.
 * @pre \code{.c}
 * file != NULL && format != NULL
 * \endcode
 * @param file source file (use \p __FILE__ )
 * @param line line (use \p __LINE__)
 * @param format format of the string
 * @param ... extra parameters
 */
void tm_print_warning_msg(char *file, int line, char *format, ...) {
    assert(file != NULL && format != NULL);

    va_list arglist;

    printf("WARNING (%s:%d) :: ", file, line);
    va_start(arglist, format);
    vprintf(format, arglist);
    va_end(arglist);
    printf("\n");
}

/**
 * Print an error message in \p stderr.
 * @pre \code{.c}
 * file != NULL && format != NULL
 * \endcode
 * @param file source file (use \p __FILE__ )
 * @param line line (use \p __LINE__)
 * @param format format of the string
 * @param ... extra parameters
 */
void tm_print_error_msg(char *file, int line, char *format, ...) {
    assert(file != NULL && format != NULL);

    va_list arglist;

    fprintf(stderr, "ERROR (%s:%d) :: ", file, line);
    va_start(arglist, format);
    vfprintf(stderr, format, arglist);
    va_end(arglist);
    fprintf(stderr, "\n");
}

/**
 * Print an error message in \p stderr, from a valid \p errcode
 * @pre \code{.c}
 * file != NULL
 * \endcode
 * @param file source file (use \p __FILE__ )
 * @param line line (use \p __LINE__)
 * @param errcode the (valid) error code
 */
void tm_print_error_code(char* file, int line, int errcode) {
    assert(file != NULL);

    if(errcode < 0 || errcode >= TM_ERR_LAST)
        tm_print_error_msg(__FILE__, __LINE__, "errcode %d (thrown from %d:%d) is unknown", errcode, file, line);
    else
        tm_print_error_msg(file, line, ERROR_EXPLS[errcode]);
}
