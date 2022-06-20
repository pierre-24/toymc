//
// Created by pierre on 6/17/22.
//

#include <assert.h>
#include <stdio.h>

/**
 * Compute the adimensional Lennard-Jones (i.e., 12-6 potential) potential between two atoms
 * \f$ U = 4 (r^{-12}-r^{6}) \f$.
 * @param r2 square of the distance between two atoms
 * @param rc2 square of the threshold distance
 * @param epsilon epsilon value
 * @param [out] U the potential value
 * @param [out] vir the virial value
 * @post results for the potential and virial are added to \p U and \p vir.
 */
void tm_potential_LJ(double r2, double epsilon, double rc2, double *U, double *vir) {
    assert(U != NULL && vir != NULL);

    if (r2 < rc2) {
        double r6i = 1. / (r2 * r2 * r2);
        *U += 4. * epsilon * (r6i * (r6i - 1));
        *vir += 16. * epsilon * (r6i * (r6i - .5));
    }
}

/**
 * Compute the adimensional Lennard-Jones potential on multiple distance
 * @param N number of distances
 * @param [in,out] rv 2*N array, in which the square of each distance are provided in \p rv[0:N],
 * individidual epsilon are in  \p rv[N:2*N].
 * @param rc2 threshold distance
 * @param [out] U the total potential value
 * @param [out] vir the total virial value
 * @post individual potential contribution are in \p rv[0:N], individual virial contribution are in \p rv[N:2*N].
 * Total results for the potential and virial are added to \p U and \p vir.
 */
void tm_potential_LJ_N(long N, double* rv, double rc2, double* U, double* vir) {
    assert(rv != NULL && U != NULL && vir != NULL);

    // compute 1/r^6
    for(int i=0; i < N; i++)
        rv[i] = rv[i] < rc2 ? 1. / (rv[i] * rv[i] * rv[i]) : 0;

    // compute potential and virial
    double eps;
    for(int i=0; i < N; i++) {
        eps = rv[N + i];
        rv[N + i] = 16. * eps * (rv[i] * (rv[i] - .5));
        rv[i] = 4. * eps * (rv[i] * (rv[i] - 1));
    }

    // sum up
    for(int i=0; i < N; i++) {
        *U += rv[i];
        *vir += rv[N + i];
    }
}
