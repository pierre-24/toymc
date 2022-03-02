#ifndef TOYMC_SIMULATION_PARAMETERS_H
#define TOYMC_SIMULATION_PARAMETERS_H

#include <stdio.h>

#include "param_file.h"
#include "param_file_parser.h"
#include <time.h>

typedef struct tm_simulation_parameters_ {
    // output
    char* output;
    int output_freq;
    int print_freq;

    // calculation (NVT)
    long seed;
    char* coordinates;
    double box_length[3];
    double VdW_cutoff;
    double temperature;
    double delta_displacement;

    // calculation (NpT)
    int use_NpT;
    double target_pressure;
    double delta_volume;
} tm_simulation_parameters;

tm_simulation_parameters* tm_simulation_parameters_new();
int tm_simulation_parameters_read(tm_simulation_parameters* p, FILE* f);
int tm_simulation_parameters_delete(tm_simulation_parameters* p);

#endif //TOYMC_SIMULATION_PARAMETERS_H
