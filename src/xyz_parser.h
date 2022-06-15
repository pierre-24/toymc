#ifndef TOYMC_XYZ_PARSER_H
#define TOYMC_XYZ_PARSER_H

#include "lexer.h"
#include "errors.h"
#include "geometry.h"

tm_geometry* tm_xyz_loads(char* input);

#endif //TOYMC_XYZ_PARSER_H
