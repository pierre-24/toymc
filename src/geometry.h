#ifndef TOYMC_GEOMETRY_H
#define TOYMC_GEOMETRY_H

/**
 * @brief Store the geometry, i.e., the position of each atoms
 * Fields are \code{.c}
 * int N; // number of atoms
 * float* positions; // positions, as array of size 3*N, {X, Y, Z} (each of size N)
 * int* types; // type of each atom, as array of size N
 * char** type_vals; // value of each type, as array of size N
 * \endcode
 */
typedef struct tm_geometry_ {
    long N;
    double * positions;
    int* types;
    char** type_vals;
} tm_geometry;

tm_geometry *tm_geometry_new(long N);
int tm_geometry_get_atom(tm_geometry* geometry, int n, int* type, double **position);
int tm_geometry_delete(tm_geometry* geometry);

#endif //TOYMC_GEOMETRY_H
