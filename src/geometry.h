#ifndef TOYMC_GEOMETRY_H
#define TOYMC_GEOMETRY_H

/**
 * @brief Store the geometry, i.e., the position of each atoms
 * Fields are \code{.c}
 * int N; // number of atoms
 * float* positions; // positions, as array of size 4*N, {X, Y, Z, TMP} (each of size N)
 * int* types; // type of each atom, as array of size N
 * \endcode
 */
typedef struct tm_geometry_ {
    int N;
    double * positions;
    int* types;
} tm_geometry;

tm_geometry *tm_geometry_new(int N);
int tm_geometry_get_atom(tm_geometry* geometry, int n, int* type, float* position[3]);
int tm_geometry_delete(tm_geometry* geometry);

#endif //TOYMC_GEOMETRY_H
