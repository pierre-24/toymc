#ifndef TOYMC_SIMULATION_PARAMETERS_H
#define TOYMC_SIMULATION_PARAMETERS_H

#include <stdio.h>

#include "param_file_objects.h"
#include "param_file_parser.h"
#include <time.h>

typedef struct tm_simulation_parameters_ {
    // duration
    long n_steps; // 1 step = 1 sweep over all atoms

    // output
    char* path_output;
    long output_freq;
    long print_freq;

    // calculation (NVT)
    long seed;
    char* path_coordinates;
    double box_length[3];
    double VdW_cutoff;
    double temperature;
    double delta_displacement;

    // calculation (NpT)
    int use_NpT;
    double target_pressure;
    double delta_volume;
    long pressure_freq;
} tm_simulation_parameters;

tm_simulation_parameters* tm_simulation_parameters_new();
int tm_simulation_parameters_read(tm_simulation_parameters* p, FILE* f);
int tm_simulation_parameters_delete(tm_simulation_parameters* p);

#endif //TOYMC_SIMULATION_PARAMETERS_H
