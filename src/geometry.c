#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#include "geometry.h"
#include "errors.h"

/**
 * Create a new geometry
 * @param N number of atoms
 * @return an initialized \p tm_geometry
 */
tm_geometry *tm_geometry_new(long N) {
    if(N < 0)
        return NULL;

    // create
    tm_geometry* g = malloc(sizeof(tm_geometry));
    if (g == NULL)
        return NULL;

    // assign
    g->N = N;
    g->positions = NULL;
    g->types = NULL;
    g->type_vals = NULL;

    // fill
    g->positions = malloc(3 * N * sizeof(double));
    if(g->positions == NULL) {
        tm_geometry_delete(g);
        return NULL;
    }

    g->types = malloc(N * sizeof(int));
    if (g->positions == NULL) {
        tm_geometry_delete(g);
        return NULL;
    }

    g->type_vals = calloc(N, sizeof(char*));
    if (g->positions == NULL) {
        tm_geometry_delete(g);
        return NULL;
    }

    return g;
}

/**
 * Get a given atom, stored in \p geometry
 * @pre \code{.c}
 * geometry != NULL && 0 <= n < geometry->N && type != NULL && position != NULL
 * \endcode
 * @param geometry the geometry
 * @param n the number of the atom
 * @param type (output) type of the atom
 * @param position (output) position of the atom
 * @post \p type and \p position are set
 * @return \p TM_ERR_OK
 */
int tm_geometry_get_atom(tm_geometry *geometry, int n, int *type, double **position) {
    assert(geometry != NULL && type != NULL && position != NULL);
    assert(n >= 0 && n < geometry->N);

    *type = geometry->types[n];
    (*position)[0] = geometry->positions[0 * geometry->N * n];
    (*position)[1] = geometry->positions[1 * geometry->N * n];
    (*position)[2] = geometry->positions[2 * geometry->N * n];

    return TM_ERR_OK;
}

/**
 * Delete \p geometry.
 * @pre \code{.c} geometry != NULL \endcode
 * @param geometry the geometry to delete
 * @return \p TM_ERR_OK
 */
int tm_geometry_delete(tm_geometry *geometry) {
    assert(geometry != NULL);

    if(geometry->positions != NULL)
        free(geometry->positions);

    if(geometry->type_vals != NULL) {
        for(int i = 0; i < geometry->N && geometry->type_vals[i] != NULL; i++) {
            free(geometry->type_vals[i]);
        }
        free(geometry->type_vals);
    }

    if(geometry->types != NULL)
        free(geometry->types);

    free(geometry);
    return TM_ERR_OK;
}
