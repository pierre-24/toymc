#include "simulation_parameters.h"

/**
 * Create a \p tm_simulation_parameters structure and sets default parameters
 * @return a valid  \p tm_simulation_parameters structure, of \p NULL if \p malloc did not succeed.
 */
tm_simulation_parameters* tm_simulation_parameters_new() {
    tm_simulation_parameters* p = malloc(sizeof(tm_simulation_parameters));

    if(p != NULL) {
        p->output = NULL;
        p->output_freq = 5;
        p->print_freq = 1;

        p->coordinates = NULL;
        p->seed = time(NULL);
        p->box_length[0] = 1.; p->box_length[1] = 1.; p->box_length[2] = 1.;
        p->VdW_cutoff = .5;
        p->temperature = 1.;
        p->delta_displacement = .1;

        p->use_NpT = 0;
        p->target_pressure = 1.;
        p->delta_volume = .1;
    }

    return p;
}

/**
 * Read a parameter file and set the simulation parameters accordingly
 * @pre \code{.c}
 * p != NULL && f != NULL
 * \endcode
 * @param p valid parameter structure
 * @param f valid file opened in read mode
 * @return
 */
int tm_simulation_parameters_read(tm_simulation_parameters* p, FILE* f) {
    if(p == NULL || f == NULL)
        return -1;

    // read file
    fseek(f, 0, SEEK_END);
    long length = ftell(f);
    fseek(f, 0, SEEK_SET);
    char* buffer = malloc((length + 1) * sizeof (char ));

    if(buffer == NULL)
        return -2;

    if(fread(buffer, 1, length, f) != length) {
        free(buffer);
        return -3;
    }

    buffer[length] = '\0';

    // getting object
    tm_parf_error e;
    tm_parf_t* obj = tm_parf_loads(buffer, &e);
    free(buffer);

    if (obj == NULL) {
        printf("error while reading parameter file (on line %d): %s", e.line, e.what);
        return -4;
    }

    tm_parf_delete(obj);
    return 0;

}

/**
 * Delete a  \p tm_simulation_parameters structure.
 * @param p valid structure
 * @return 0 if the structure was delete, something else otherwise
 */
int tm_simulation_parameters_delete(tm_simulation_parameters* p) {
    if(p == NULL)
        return -1;

    if(p->output != NULL)
        free(p->output);

    if(p->coordinates != NULL)
        free(p->coordinates);

    free(p);
    return 0;
}
