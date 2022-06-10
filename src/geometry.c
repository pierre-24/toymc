#include <stdio.h>
#include <stdlib.h>

#include "geometry.h"
#include "errors.h"

/**
 * Create a new geometry
 * @param N number of atoms
 * @return an initialized \p tm_geometry
 */
tm_geometry *tm_geometry_new(int N) {
    if(N < 0)
        return NULL;

    // create
    tm_geometry* g = malloc(sizeof(tm_geometry));
    if (g == NULL)
        return NULL;

    // assign
    g->positions = NULL;
    g->types = NULL;

    // fill
    g->positions = malloc(4 * N * sizeof (float));
    if(g->positions == NULL) {
        tm_geometry_delete(g);
        return NULL;
    }

    g->types = malloc(N * sizeof (int));
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
 * @return \p TM_ERR_OK if the atom exists
 * @post \p type and \p position are set
 */
int tm_geometry_get_atom(tm_geometry *geometry, int n, int *type, float **position) {
    if(geometry == NULL || type == NULL || position == NULL)
        return TM_ERR_PARAM_NULL;

    if(n < 0 || n >= geometry->N)
        return TM_ERR_OUT_OF_BOUND;

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
 * @return \p TM_ERR_OK if the object was delete, something else otherwise
 */
int tm_geometry_delete(tm_geometry *geometry) {
    if(geometry == NULL)
        return TM_ERR_PARAM_NULL;

    if(geometry->positions != NULL)
        free(geometry->positions);

    if(geometry->types != NULL)
        free(geometry->types);

    free(geometry);
    return TM_ERR_OK;
}
