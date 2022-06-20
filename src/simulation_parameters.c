#include <assert.h>
#include "simulation_parameters.h"
#include "files.h"

/**
 * Create a \p tm_simulation_parameters structure and sets default parameters
 * @return a valid  \p tm_simulation_parameters structure, of \p NULL if \p malloc did not succeed.
 */
tm_simulation_parameters* tm_simulation_parameters_new() {
    tm_simulation_parameters* p = malloc(sizeof(tm_simulation_parameters));

    if(p != NULL) {
        p->n_steps = 1;

        p->path_output = NULL;
        p->output_freq = 5;
        p->print_freq = 1;

        p->path_coordinates = NULL;
        p->seed = time(NULL);
        p->box_length[0] = 1.; p->box_length[1] = 1.; p->box_length[2] = 1.;
        p->VdW_cutoff = .5;
        p->temperature = 1.;
        p->delta_displacement = .1;

        p->use_NpT = 0;
        p->target_pressure = 1.;
        p->delta_volume = .1;
        p->pressure_freq = 1;
    }

    return p;
}

struct valid_key {
    char* key;
    char* types;
    void* ptr;
};

/**
 * Fill a parameter, if of the correct type
 * @pre \code{.c}
 * elmt != NULL && ptr != NULL
 * \endcode
 * @param elmt the object that should contain the value
 * @param type expected type of the object (\p i, \p b, \b r or \p s)
 * @param ptr pointer to the value to fill
 * @return \p TM_ERR_OK if everything went well, something else otherwise.
 * @post \p ptr is set accordingly.
 */
int simulation_parameter_fill_single_value_key(tm_parf_t* elmt, char type, void * ptr) {
    assert(elmt != NULL && ptr != NULL);
    assert(type == 'i' || type == 'b' || type == 'r' || type == 's');

    int error = TM_ERR_OK;
    if(type == 'i') {
        if(elmt->val_type != TM_T_INTEGER) {
            tm_print_error_msg(__FILE__, __LINE__, "key %s: expected type %d, got %d", elmt->key, TM_T_INTEGER, elmt->val_type);
            error = TM_ERR_PARAMETER_FILE;
        } else {
            tm_parf_integer_value(elmt, (long *) ptr);
        }
    } else if(type == 'b') {
        if(elmt->val_type != TM_T_BOOLEAN) {
            tm_print_error_msg(__FILE__, __LINE__, "key %s: expected type %d, got %d\n", elmt->key, TM_T_BOOLEAN, elmt->val_type);
            error = TM_ERR_PARAMETER_FILE;
        } else {
            tm_parf_boolean_value(elmt, (int*) ptr);
        }
    } else if(type == 'r') {
        if(elmt->val_type != TM_T_REAL) {
            tm_print_error_msg(__FILE__, __LINE__, "key %s: expected type %d, got %d\n", elmt->key, TM_T_REAL, elmt->val_type);
            error = TM_ERR_PARAMETER_FILE;
        } else {
            tm_parf_real_value(elmt, (double *) ptr);
        }
    } else if(type == 's') {
        if(elmt->val_type != TM_T_STRING) {
            tm_print_error_msg(__FILE__, __LINE__, "key %s: expected type %d, got %d\n", elmt->key, TM_T_STRING, elmt->val_type);
            error = TM_ERR_PARAMETER_FILE;
        } else {
            char* val, *dest;
            unsigned int sz;
            tm_parf_string_value(elmt, &val);
            tm_parf_string_length(elmt, &sz);
            dest = malloc(sizeof(char) * (sz + 1));
            if (dest == NULL) {
                error = TM_ERR_MALLOC;
            } else {
                strcpy(dest, val);
                *((char **) (ptr)) = dest;
            }
        }
    }

    return error;
}

/**
 * Fill a list parameter, if of the correct type
 * @pre \code{.c}
 * elmt != NULL && ptr != NULL && types != NULL
 * && !TM_PARF_CHECK_P(elmt, TM_T_LIST)
 * \endcode
 * @param elmt the object that should contain the value
 * @param types expected type of the elements of the list (\p i, \p b, \b r or \p s, then the length of the list)
 * @param ptr pointer to the value to fill
 * @return \p TM_ERR_OK if everything went well, something else otherwise.
 * @post \p ptr is set accordingly.
 */
int simulation_parameter_fill_multiple_values_key(tm_parf_t* elmt, char* types, void * ptr) {
    assert(elmt != NULL && ptr != NULL && types != NULL);
    assert(!TM_PARF_CHECK_P(elmt, TM_T_LIST));

    unsigned int sz = atoi(types + 1), szi, szp;
    tm_parf_list_length(elmt, &szi);

    if (sz != szi) {
        tm_print_error_msg(__FILE__, __LINE__, "key %s: expected size %d, got %d", elmt->key, sz, szi);
        return TM_ERR_PARAMETER_FILE;
    }

    switch (types[0]) {
        case 'i':
            szp = sizeof(long);
            break;
        case 'r':
            szp = sizeof(double);
            break;
        case 'b':
            szp = sizeof(int);
            break;
        default:
            szp = 1;
            break;
    }

    int error = 0;
    tm_parf_iterator* it = tm_parf_iterator_new(elmt);
    tm_parf_t* elmt_list;
    char* ptr2 = (char*) ptr;
    for(unsigned int i = 0; i < szi && error == TM_ERR_OK; i++) {
        tm_parf_iterator_next(it, &elmt_list);
        error = simulation_parameter_fill_single_value_key(elmt_list, types[0], (void*) (ptr2 + i * szp));
    }

    tm_parf_iterator_delete(it);
    return error;
}


/**
 * Fill the parameters from the object
 * @pre \code{.c}
 * p != NULL && obj != NULL && !TM_PARF_CHECK_P(obj, TM_T_OBJECT)
 * \endcode
 * @param p the parameters
 * @param obj the object
 * @return \p TM_ERR_OK if everything went well, something else otherwise.
 * @post \p p is set accordingly.
 */
int tm_simulation_parameter_fill(tm_simulation_parameters* p, tm_parf_t* obj) {
    assert(p != NULL && obj != NULL);
    assert(!TM_PARF_CHECK_P(obj, TM_T_OBJECT));

    // setup valid keys
    struct valid_key keys[] = {
            // integers:
            {"n_steps", "i", &(p->n_steps)},
            {"seed", "i", &(p->seed)},
            {"output_freq", "i", &(p->output_freq)},
            {"print_freq", "i", &(p->print_freq)},
            {"pressure_freq", "i", &(p->pressure_freq)},

            // boolean
            {"use_NpT", "b", &(p->use_NpT)},

            // double
            {"VdW_cutoff", "r", &(p->VdW_cutoff)},
            {"temperature", "r", &(p->temperature)},
            {"delta_displacement", "r", &(p->delta_displacement)},
            {"target_pressure", "r", &(p->target_pressure)},
            {"delta_volume", "r", &(p->delta_volume)},

            // string
            {"output", "s", &(p->path_output)},
            {"coordinates", "s", &(p->path_coordinates)},

            // list
            {"box_length", "r3", &(p->box_length)}
    };

    int num_keys = sizeof(keys) / sizeof(*keys);

    // crawl in
    tm_parf_iterator * it = tm_parf_iterator_new(obj);
    tm_parf_t* elmt;
    int found, error;

    while(tm_parf_iterator_has_next(it)) {
        tm_parf_iterator_next(it, &elmt);
        found = 0;
        error = 0;

        for(int i=0; i < num_keys && !found; i++) {
            if(strcmp(keys[i].key, elmt->key) == 0) {
                found = 1;
                tm_print_debug_msg(__FILE__, __LINE__, "treating key %s (kind %s)", keys[i].key, keys[i].types);
                if(strlen(keys[i].types) == 1) {
                    error = simulation_parameter_fill_single_value_key(elmt, keys[i].types[0], keys[i].ptr);
                } else {
                    error = simulation_parameter_fill_multiple_values_key(elmt, keys[i].types, keys[i].ptr);
                }
            }
        }

        if(error != TM_ERR_OK) {
            tm_parf_iterator_delete(it);
            return error;
        }

        if(!found) {
            tm_print_warning_msg(__FILE__, __LINE__, "key %s is unknown, maybe there is a mistake?", elmt->key);
        }
    }

    tm_parf_iterator_delete(it);

    return 0;
}

/**
 * Read a parameter file and set the simulation parameters accordingly.
 * @pre \code{.c}
 * p != NULL && f != NULL
 * \endcode
 * @param p valid parameter structure
 * @param f valid file opened in read mode
 * @return \p TM_ERR_OK if everything went well, something else otherwise
 */
int tm_simulation_parameters_read(tm_simulation_parameters* p, FILE* f) {
    assert(p != NULL && f != NULL);

    char* buffer;
    int r = tm_read_file(f, &buffer);
    if(r != TM_ERR_OK) {
        return r;
    }

    // getting object
    tm_parf_t* obj = tm_parf_loads(buffer);
    free(buffer);

    if (obj == NULL) {
        return TM_ERR_PARAMETER_FILE;
    }

    r = tm_simulation_parameter_fill(p, obj);
    tm_parf_delete(obj);
    return r;

}

/**
 * Delete a  \p tm_simulation_parameters structure.
 * @pre \code{.c}
 * p != NULL
 * \endcode
 * @param p valid structure
 * @return \p TM_ERR_OK
 */
int tm_simulation_parameters_delete(tm_simulation_parameters* p) {
    assert(p != NULL);

    if(p->path_output != NULL)
        free(p->path_output);

    if(p->path_coordinates != NULL)
        free(p->path_coordinates);

    free(p);
    return TM_ERR_OK;
}
