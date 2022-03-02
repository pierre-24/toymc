#ifndef TOYMC_PARAM_FILE_H
#define TOYMC_PARAM_FILE_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define TM_parf_IS(o,t) ((o)->val_type == (t))
#define TM_PARF_CHECK_P(o,t) ((o) == NULL || (o)->val_type != (t))

typedef enum tm_parf_type_ {
    TM_T_LIST,
    TM_T_OBJECT,
    TM_T_STRING,
    TM_T_INTEGER,
    TM_T_BOOLEAN,
    TM_T_REAL,

    TM_T_LAST
} tm_parf_type;

typedef struct tm_parf_t_ {
    tm_parf_type val_type;
    char* key;

    struct tm_parf_t_* val_obj_or_list;
    struct tm_parf_t_* last;
    long val_int;
    double val_real;
    char* val_str;
    unsigned int val_size;

    struct tm_parf_t_* next;
} tm_parf_t;

int tm_parf_delete(tm_parf_t* obj);

// object
tm_parf_t* tm_parf_object_new();
int tm_parf_object_set(tm_parf_t* obj, char* key, tm_parf_t* val);
int tm_parf_object_get(tm_parf_t* obj, char* key, tm_parf_t** val);

// integer
tm_parf_t* tm_parf_integer_new(long val);
int tm_parf_integer_set(tm_parf_t* obj, long val);
int tm_parf_integer_value(tm_parf_t* obj, long *val);

// boolean
tm_parf_t* tm_parf_boolean_new(int val);
int tm_parf_boolean_set(tm_parf_t* obj, int val);
int tm_parf_boolean_value(tm_parf_t* obj, int* val);

// real
tm_parf_t* tm_parf_real_new(double val);
int tm_parf_real_set(tm_parf_t* obj, double val);
int tm_parf_real_value(tm_parf_t* obj, double* val);

// string
tm_parf_t* tm_parf_string_new(char* val);
int tm_parf_string_set(tm_parf_t* obj, char* val);
int tm_parf_string_value(tm_parf_t* object, char **val);
int tm_parf_string_length(tm_parf_t* object, unsigned int *s);

// list
tm_parf_t* tm_parf_list_new();
int tm_parf_list_append(tm_parf_t* obj, tm_parf_t* val);
int tm_parf_list_get(tm_parf_t* obj, int index, tm_parf_t** val);
int tm_parf_list_length(tm_parf_t* obj, unsigned int* sz);

// iterator
typedef struct tm_parf_iterator_ {
    tm_parf_t * obj;
    tm_parf_t* next;
} tm_parf_iterator;

tm_parf_iterator* tm_parf_iterator_new(tm_parf_t* obj);
int tm_parf_iterator_delete(tm_parf_iterator* it);
int tm_parf_iterator_has_next(tm_parf_iterator* it);
int tm_parf_iterator_next(tm_parf_iterator* it, tm_parf_t** obj);

#endif //TOYMC_PARAM_FILE_H
