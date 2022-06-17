#include "simulation_parameters.h"
#include "files.h"

/**
 * Create a \p tm_simulation_parameters structure and sets default parameters
 * @return a valid  \p tm_simulation_parameters structure, of \p NULL if \p malloc did not succeed.
 */
tm_simulation_parameters* tm_simulation_parameters_new() {
    tm_simulation_parameters* p = malloc(sizeof(tm_simulation_parameters));

    if(p != NULL) {
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
    int error = 0;
    if(type == 'i') {
        if(elmt->val_type != TM_T_INTEGER) {
            printf("error: key %s: expected type %d, got %d\n", elmt->key, TM_T_INTEGER, elmt->val_type);
            error = TM_ERR_PARF_UNEXPECTED_TYPE;
        } else {
            long val = 0;
            if(tm_parf_integer_value(elmt, &val) == 0)
                *((long *) (ptr)) = val;
            else
                error = TM_ERR_API;
        }
    } else if(type == 'b') {
        if(elmt->val_type != TM_T_BOOLEAN) {
            printf("error: key %s: expected type %d, got %d\n", elmt->key, TM_T_BOOLEAN, elmt->val_type);
            error = TM_ERR_PARF_UNEXPECTED_TYPE;
        } else {
            int val = 0;
            if(tm_parf_boolean_value(elmt, &val) == 0)
                *((int *) (ptr)) = val;
            else
                error = TM_ERR_API;
        }
    } else if(type == 'r') {
        if(elmt->val_type != TM_T_REAL) {
            printf("error: key %s: expected type %d, got %d\n", elmt->key, TM_T_REAL, elmt->val_type);
            error = TM_ERR_PARF_UNEXPECTED_TYPE;
        } else {
            double val = 0;
            if(tm_parf_real_value(elmt, &val) == 0)
                *((double *) (ptr)) = val;
            else
                error = TM_ERR_API;
        }
    } else if(type == 's') {
        if(elmt->val_type != TM_T_STRING) {
            printf("error: key %s: expected type %d, got %d\n", elmt->key, TM_T_STRING, elmt->val_type);
            error = TM_ERR_PARF_UNEXPECTED_TYPE;
        } else {
            char* val, *dest;
            unsigned int sz;
            if(tm_parf_string_value(elmt, &val) == 0) {
                tm_parf_string_length(elmt, &sz);
                dest = malloc(sizeof(char) * (sz + 1));
                if (dest == NULL) {
                    error = TM_ERR_MALLOC;
                } else {
                    strcpy(dest, val);
                    *((char **) (ptr)) = dest;
                }
            } else
                error = TM_ERR_API;
        }
    } else {
        error = TM_ERR_API;
    }

    return error;
}

/**
 * Fill a list parameter, if of the correct type
 * @pre \code{.c}
 * elmt != NULL && ptr != NULL && !TM_PARF_CHECK_P(elmt, TM_T_LIST)
 * \endcode
 * @param elmt the object that should contain the value
 * @param types expected type of the elements of the list (\p i, \p b, \b r or \p s, then the length of the list)
 * @param ptr pointer to the value to fill
 * @return \p TM_ERR_OK if everything went well, something else otherwise.
 * @post \p ptr is set accordingly.
 */
int simulation_parameter_fill_multiple_values_key(tm_parf_t* elmt, char* types, void * ptr) {
    if(TM_PARF_CHECK_P(elmt, TM_T_LIST)) {
        return TM_ERR_PARF_NCHECK;
    }

    unsigned int sz = atoi(types + 1), szi, szp;

    if(tm_parf_list_length(elmt, &szi) != 0) {
        return TM_ERR_API;
    }

    if (sz != szi) {
        printf("error: key %s: expected size %d, got %d", elmt->key, sz, szi);
        return TM_ERR_PARF_LIST_SIZE;
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
    for(unsigned int i = 0; i < szi && error == 0; i++) {
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
    if(p == NULL || obj == NULL)
        return TM_ERR_PARAM_NULL;

    if(TM_PARF_CHECK_P(obj, TM_T_OBJECT))
        return TM_ERR_PARF_NCHECK;

    // setup valid keys
    struct valid_key keys[] = {
            // integers:
            {"seed", "i", &(p->seed)},
            {"output_freq", "i", &(p->output_freq)},
            {"print_freq", "i", &(p->print_freq)},

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
    if(p == NULL || f == NULL)
        return TM_ERR_PARAM_NULL;

    char* buffer;
    int r = tm_read_file(f, &buffer);
    if(r != TM_ERR_OK) {
        return r;
    }

    // getting object
    tm_parf_error e;
    tm_parf_t* obj = tm_parf_loads(buffer);
    free(buffer);

    if (obj == NULL) {
        tm_print_error_msg(__FILE__, __LINE__, "error while reading parameter file (on line %d): %s", e.line, e.what);
        return TM_ERR_PARF;
    }

    r = tm_simulation_parameter_fill(p, obj);
    tm_parf_delete(obj);
    return r;

}

/**
 * Delete a  \p tm_simulation_parameters structure.
 * @param p valid structure
 * @return \p TM_ERR_OK if the structure was delete, something else otherwise
 */
int tm_simulation_parameters_delete(tm_simulation_parameters* p) {
    if(p == NULL)
        return -1;

    if(p->path_output != NULL)
        free(p->path_output);

    if(p->path_coordinates != NULL)
        free(p->path_coordinates);

    free(p);
    return TM_ERR_OK;
}
