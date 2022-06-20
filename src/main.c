#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "timer.h"
#include <string.h>


int init_positions(double* positions, int N, double L) {
    int ppL = (int) ceil(pow(N, 1./3));
    double dist = L / ppL;
    printf("ppL = %d, dist = %f\n", ppL, dist);
    int x, y = 0, z = 0;
    
    for(int i=0; i < N; i++) {
        if(i != 0 && (i % (ppL * ppL) == 0)) {
            z += 1;
            x = y = 0;
        } else if (i != 0 && (i % ppL == 0)) {
            y += 1;
            x = 0;
        } else {
            x = i % ppL;
        }
        
        positions[0 * N + i] = x * dist;
        positions[1 * N + i] = y * dist;
        positions[2 * N + i] = z * dist;
    }
}

void compute_LJ(double r2, double rc2, double* U, double* vir) {
    if (r2 < rc2) {
        double r6i = 1. / (r2 * r2 * r2);
        *U += 4. * (r6i * (r6i - 1));
        *vir += 16. * (r6i * (r6i - .5));
    }
}

double rnd() { // yeah, it is bad, I know
    return ((double) rand()) / RAND_MAX;
}

void compute_Ui(double* positions, int N, double L, int i, double rc2, double* U_i, double* vir_i) {
    double Ui = .0f, hL = L/2, q, dq;
    double* restrict q1;
    
    for(int j=0; j < N; j++)
        positions[3 * N + j] = .0; 

    for(int k=0; k < 3; k++) {
        q1 = positions + k * N;
        #pragma omp for simd
        for(int j=0; j < N; j++) {
            dq = *(q1 + j) - *(q1 + i);
            dq += (dq>hL) * (-L) + (dq<-hL) * L;
            positions[3 * N + j] += dq * dq;
        }
    }
    
    for(int j=0; j < N; j++) {
        if (j != i)
            compute_LJ(positions[3 * N + j], rc2, U_i, vir_i);
    }
}

void compute_U(double* positions, int N, double L, double rc2, double* U, double* vir) {
    double q, dq, hL = L / 2;
    double* restrict q1;
    *U = 0;
    *vir = 0;
    
    for(int i=0; i < N - 1; i++) {
        for(int j=i + 1; j < N; j++)
            positions[3 * N + j] = .0; 

        for(int k=0; k < 3; k++) {
            q1 = positions + k * N;
            #pragma omp for simd
            for(int j=i+1; j < N; j++) {
                dq = *(q1 + j) - *(q1 + i);
                dq += (dq>hL) * (-L) + (dq<-hL) * L;
                positions[3 * N + j] += dq * dq;
            }
        }
        
        for(int j=i + 1; j < N; j++) {
            compute_LJ(positions[3 * N + j], rc2, U, vir);
        }
    }
}

int main(int argc, char* argv[]) {
    double rho = 0.8, rc= 4.f, *positions = NULL, U = .0, vir=.0, delta=0.1f, U_old, U_new, vir_old, vir_new, p_old[3], p_new[3], T=0.9, e;
    int N = 512, trials=100, accepted=0;
    int seed = time(NULL);
    char* out = "out.xyz";
    
    // read args
    if(argc > 1) {
        for (int i=0; i < argc; i++) {
            if(strcmp(argv[i], "-N") == 0) {
                if((i+1) == argc) { // `-N`, but no number provided :(
                    return EXIT_FAILURE;
                } else {
                    N = atoi(argv[i + 1]);
                    if(N < 1)
                        return EXIT_FAILURE;
                }
            } else if(strcmp(argv[i], "-d") == 0) {
                if((i+1) == argc) { // `-d`, but no number provided :(
                    return  EXIT_FAILURE;
                } else {
                    char* end;
                    delta = strtod(argv[i + 1], &end);
                    if(argv[i + 1] == end)
                        return EXIT_FAILURE;
              }
            } else if(strcmp(argv[i], "-n") == 0) {
                if((i+1) == argc) { // `-n`, but no number provided :(
                    return EXIT_FAILURE;
                } else {
                    trials = atoi(argv[i + 1]);
                    if(trials < 1)
                        return EXIT_FAILURE;
                }
            } else if(strcmp(argv[i], "-T") == 0) {
                if((i+1) == argc) { // `-T`, but no number provided :(
                    return  EXIT_FAILURE;
                } else {
                    char* end;
                    T = strtod(argv[i + 1], &end);
                    if(argv[i + 1] == end)
                        return EXIT_FAILURE;
              }
            } else if(strcmp(argv[i], "-r") == 0) {
                if((i+1) == argc) { // `-r`, but no number provided :(
                    return  EXIT_FAILURE;
                } else {
                    char* end;
                    rho = strtod(argv[i + 1], &end);
                    if(argv[i + 1] == end)
                        return EXIT_FAILURE;
              }
            } else if(strcmp(argv[i], "-R") == 0) {
                if((i+1) == argc) { // `-R`, but no number provided :(
                    return  EXIT_FAILURE;
                } else {
                    char* end;
                    rc = strtod(argv[i + 1], &end);
                    if(argv[i + 1] == end)
                        return EXIT_FAILURE;
              }
            } else if(strcmp(argv[i], "-s") == 0) {
                if((i+1) == argc) { // `-s`, but no number provided :(
                    return EXIT_FAILURE;
                } else {
                    seed = atoi(argv[i + 1]);
                    if(trials < 1)
                        return EXIT_FAILURE;
                }
            } else if(strcmp(argv[i], "-o") == 0) {
                if((i+1) == argc) { // `-o`, but nothing!
                    return -1;
                } else {
                    out = argv[i + 1];
                }
            }
        }
    }
    
    srand(seed);
    printf("seed = %d\n", seed);
    
    // prepare box
    double V = N / rho;
    double L = pow(V, 1./3);
    
    printf("rho = %.3f, box volume = %.3f\nbox length = %.3f\n", rho, V, L); 
    
    positions = malloc(4 * N * sizeof(double));
    if (positions == NULL) {
        printf("cannot allocate positions :(");
        return EXIT_FAILURE;
    }
    
    init_positions(positions, N, L);
    
    // compute tail correction
    printf("rc = %.3f\n", rc);
    double rc2 = rc * rc;
    double irc3 = 1. / (rc * rc * rc);
    double U_tail = N * 8. * M_PI * rho * (irc3 * (irc3 * irc3 / 9 - 1./3));
    double P_tail = 16./3*M_PI*rho*rho*(irc3 * (2*irc3*irc3/3-1.));
    printf("U_tail = %f, P_tail=%.3f\n", U_tail, P_tail);
    
    // compute the energy of that box
    struct timespec t;
    double time, total_time = 0;
    
    compute_U(positions, N, L, rc2, &U, &vir);
    printf("U = %.3f\n", U + U_tail);
    
    // iterate through the thing
    double sq_delta = delta / pow(3, .5);
    printf("delta = %.3f, sq_delta = %.3f\n", delta, sq_delta);
    for(int i=0; i < trials; i++) { 
        for(int p=0; p < N; p++) { // sweep through all particles
            U_old = U_new = vir_old = vir_new = 0;
            compute_Ui(positions, N, L, p, rc2, &U_old, &vir_old);
        
            // new position
            for(int k=0; k <3; k++) {
                p_old[k] = positions[k * N + p];
                positions[k * N + p] += (1 - 2 * rnd()) * sq_delta; 
                
                // boundary
                if(positions[k * N + p] < 0)
                    positions[k * N + p] += L;
                else if(positions[k * N + p] > L)
                    positions[k * N + p] -= L;
            }
            
            compute_Ui(positions, N, L, p, rc2, &U_new, &vir_new);
            e = exp(-(U_new - U_old) / T);
            
            if (rnd() < e) {
                accepted++;
                U += U_new - U_old;
                vir += vir_new - vir_old;
            } else {
                for(int k=0; k <3; k++) {
                    positions[k * N + p] = p_old[k];
                }
            }
        }
        
        printf("%4d: U = %.3f, p=%.3f\n", i, U + U_tail, vir/V + rho * T + P_tail);
    }
    
    printf("r=%d, acceptance = %.1f\%\n", accepted, ((double) accepted ) / (N * trials) * 100.0f);
    
    // write positions
    FILE*f = NULL;
    f = fopen(out, "w");
    if(f == NULL) {
        printf("error while opening %s\n", out);
        return EXIT_FAILURE;
    }
    
    fprintf(f, "%d\nE=%.3f, p=%.3f\n", N, U + U_tail, vir/V + rho * T + P_tail);
    for(int p=0; p < N; p++) {
        fprintf(f, "He %9.5f %9.5f %9.5f\n", positions[0 * N + p],positions[1 * N + p], positions[2 * N + p]);
    }
    
    fclose(f);
    
    // done!
    free(positions);
    return EXIT_SUCCESS;
}
