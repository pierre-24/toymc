//
// Created by pierre on 26/02/2022.
//

#ifndef TOYMC_INPUT_FILE_H
#define TOYMC_INPUT_FILE_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define TM_INFI_IS(o,t) ((o)->val_type == (t))
#define TM_INFI_CHECK_P(o,t) ((o) == NULL || (o)->val_type != (t))

typedef enum tm_infi_type_ {
    TM_T_ARRAY,
    TM_T_OBJECT,
    TM_T_STRING,
    TM_T_INTEGER,
    TM_T_BOOLEAN,
    TM_T_REAL,

    TM_T_LAST
} tm_infi_type;

typedef struct tm_infi_t_ {
    tm_infi_type val_type;
    char* key;

    struct tm_infi_t_* val_obj_or_list;
    struct tm_infi_t_* last;
    int val_int;
    double val_real;
    char* val_str;
    unsigned int val_size;

    struct tm_infi_t_* next;
} tm_infi_t;

int tm_infi_delete(tm_infi_t* obj);

// object
tm_infi_t* tm_infi_object_new();
int tm_infi_object_set(tm_infi_t* obj, char* key, tm_infi_t* val);
int tm_infi_object_get(tm_infi_t* obj, char* key, tm_infi_t** val);

// integer
tm_infi_t* tm_infi_integer_new(int val);
int tm_infi_integer_set(tm_infi_t* obj, int val);
int tm_infi_integer_value(tm_infi_t* obj, int* val);

// boolean
tm_infi_t* tm_infi_boolean_new(int val);
int tm_infi_boolean_set(tm_infi_t* obj, int val);
int tm_infi_boolean_value(tm_infi_t* obj, int* val);

// real
tm_infi_t* tm_infi_real_new(double val);
int tm_infi_real_set(tm_infi_t* obj, double val);
int tm_infi_real_value(tm_infi_t* obj, double* val);

// string
tm_infi_t* tm_infi_string_new(char* val);
int tm_infi_string_set(tm_infi_t* obj, char* val);
int tm_infi_string_value(tm_infi_t* obj, char** val);
int tm_infi_string_length(tm_infi_t* object, unsigned int *s);

// array
tm_infi_t* tm_infi_array_new();
int tm_infi_array_append(tm_infi_t* obj, tm_infi_t* val);
int tm_infi_array_get(tm_infi_t* obj, int index, tm_infi_t** val);
int tm_infi_array_length(tm_infi_t* obj, unsigned int* sz);

// iterator
typedef struct tm_infi_iterator_ {
    tm_infi_t * obj;
    tm_infi_t* current;
} tm_infi_iterator;

tm_infi_iterator* tm_infi_iterator_new(tm_infi_t* obj);
int tm_infi_iterator_delete(tm_infi_iterator* it);
int tm_infi_iterator_has_next(tm_infi_iterator* it);
int tm_infi_iterator_next(tm_infi_iterator* it, tm_infi_t** obj);

#endif //TOYMC_INPUT_FILE_H
