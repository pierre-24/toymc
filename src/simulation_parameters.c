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

struct valid_key {
    char* key;
    tm_parf_type type;
    void* ptr;
};

/**
 * Fill the parameters from the object
 * @pre \code{.c}
 * p != NULL && obj != NULL && !TM_PARF_CHECK_P(object, TM_T_OBJECT)
 * \endcode
 * @param p the parameters
 * @param obj the object
 * @return 0 if everything went well, something else otherwise.
 * @post \p p is set accordingly.
 */
int tm_simulation_parameter_fill(tm_simulation_parameters* p, tm_parf_t* obj) {
    if(p == NULL || obj == NULL)
        return -1;

    if(TM_PARF_CHECK_P(obj, TM_T_OBJECT))
        return -2;

    // setup valid keys
    struct valid_key keys[] = {
            {"seed", TM_T_INTEGER, &(p->seed)}
    };

    int num_keys = sizeof(keys) / sizeof(*keys);

    // crawl in
    tm_parf_iterator * it = tm_parf_iterator_new(obj);
    tm_parf_t* elmt;
    int found, error;

    while(tm_parf_iterator_has_next(it)) {
        tm_parf_iterator_next(it, &elmt);
        found = 0;
        for(int i=0; i < num_keys && !found; i++) {
            if(strcmp(keys[i].key, elmt->key) == 0) {
                found = 1;
                error = 0;
                if(keys[i].type != elmt->val_type) {
                    printf("key %s: expected type %d, got type %d", elmt->key, keys[i].type, elmt->val_type);
                    error = -3;
                } else {
                    if (elmt->val_type == TM_T_INTEGER) {
                        long val = 0;
                        if(tm_parf_integer_value(elmt, &val) == 0)
                            *((long *) (keys[i].ptr)) = val;
                        else
                            error = -4;
                    }
                }

                if(error != 0) {
                    tm_parf_iterator_delete(it);
                    return error;
                }
            }
        }

        if(!found) {
            printf("warning: key %s is invalid, maybe there is a mistake?\n", elmt->key);
        }
    }

    tm_parf_iterator_delete(it);

    return 0;
}

/**
 * Read a parameter file and set the simulation parameters accordingly
 * @pre \code{.c}
 * p != NULL && f != NULL
 * \endcode
 * @param p valid parameter structure
 * @param f valid file opened in read mode
 * @return 0 if everything went well, something else otherwise
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

    int r = tm_simulation_parameter_fill(p, obj);
    tm_parf_delete(obj);
    return r;

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
