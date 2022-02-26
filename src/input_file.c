#include "input_file.h"

/* objects */
tm_infi_t* tm_infi_new(tm_infi_type t) {
    tm_infi_t* j = NULL;
    j = malloc(sizeof(tm_infi_t));
    if (j != NULL) {
        j->val_type = t;

        j->key = NULL;
        j->val_obj_or_list = NULL;
        j->val_str = NULL;
        j->next = NULL;
        j->val_size = 0;
    }

    return j;
}

int tm_infi_delete(tm_infi_t* obj) {
    if(obj == NULL)
        return -1;

    int r = 0;
    if(obj->next != NULL)
        r = tm_infi_delete(obj->next);

    if (r < 0)
        return r;

    if(obj->key != NULL)
        free(obj->key);

    if(TM_INFI_IS(obj, TM_T_STRING) && obj->val_str != NULL)
        free(obj->val_str);

    if ((TM_INFI_IS(obj, TM_T_OBJECT) || TM_INFI_IS(obj, TM_T_ARRAY)) && obj->val_obj_or_list != NULL)
        r = tm_infi_delete(obj->val_obj_or_list);

    if (r < 0)
        return r;

    free(obj);

    return 0;
}

/* boolean */
tm_infi_t* tm_infi_boolean_new(int val) {
    tm_infi_t* o = tm_infi_new(TM_T_BOOLEAN);
    if(o != NULL)
        tm_infi_boolean_set(o, val);

    return o;
}

int tm_infi_boolean_set(tm_infi_t* object, int val) {
    if (TM_INFI_CHECK_P(object, TM_T_BOOLEAN))
        return -1;

    object->val_int = val;
    return 0;
}

int tm_infi_boolean_value(tm_infi_t* object, int* value) {
    if (TM_INFI_CHECK_P(object, TM_T_BOOLEAN))
        return -1;

    *value = object->val_int;
    return 0;
}

/* integers */
tm_infi_t* tm_infi_integer_new(int val) {
    tm_infi_t* o = tm_infi_new(TM_T_INTEGER);
    if(o != NULL)
        tm_infi_integer_set(o, val);
    return o;
}

int tm_infi_integer_set(tm_infi_t* object, int val) {
    if (TM_INFI_CHECK_P(object, TM_T_INTEGER))
        return -1;

    object->val_int = val;
    return 0;
}

int tm_infi_integer_value(tm_infi_t* object, int* value) {
    if (TM_INFI_CHECK_P(object, TM_T_INTEGER))
        return -1;

    *value = object->val_int;
    return 0;
}

/* real */
tm_infi_t* tm_infi_real_new(double val) {
    tm_infi_t* o = tm_infi_new(TM_T_REAL);
    if(o != NULL)
        tm_infi_real_set(o, val);
    return o;
}

int tm_infi_real_set(tm_infi_t* object, double val) {
    if (TM_INFI_CHECK_P(object, TM_T_REAL))
        return -1;

    object->val_real = val;
    return 0;
}

int tm_infi_real_value(tm_infi_t* object, double * value) {
    if (TM_INFI_CHECK_P(object, TM_T_REAL))
        return -1;

    *value = object->val_real;
    return 0;
}

