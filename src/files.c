#include "files.h"
#include "errors.h"

#include <stdlib.h>
#include <assert.h>

/**
 * Read the whole file in one shot.
 * @param f an open file
 * @param buffer pointer to the buffer
 * @post \p *buffer contains the content of the file, caller is responsible to free it.
 * @return  \p TM_ERR_OK if everything was ok, something else otherwise.
 */
int tm_read_file(FILE *f, char **buffer) {
    assert(f != NULL && buffer != NULL);

    // get size
    fseek(f, 0, SEEK_END);
    size_t length = ftell(f);
    fseek(f, 0, SEEK_SET);

    // allocate
    *buffer = malloc((length + 1) * sizeof (char));

    if(*buffer == NULL)
        return TM_ERR_MALLOC;

    // read
    if(fread(*buffer, 1, length, f) != length) {
        free(*buffer);
        return TM_ERR_READ;
    }

    (*buffer)[length] = '\0';

    return TM_ERR_OK;
}
