#include "input_file.h"

/* objects */
tm_infi_t* tm_infi_new(tm_infi_type t) {
    tm_infi_t* j = NULL;
    j = malloc(sizeof(tm_infi_t));
    if (j != NULL) {
        j->val_type = t;

        j->key = NULL;
        j->val_obj_or_list = NULL;
        j->last = NULL;
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

/* string */
tm_infi_t* tm_infi_string_new(char* val) {
    tm_infi_t* o = tm_infi_new(TM_T_STRING);
    if(o != NULL)
        tm_infi_string_set(o, val);

    return o;
}

int tm_infi_string_set(tm_infi_t* object, char* val) {
    if (TM_INFI_CHECK_P(object, TM_T_STRING))
        return -1;

    if (val == NULL)
        return -1;

    if (object->val_str != NULL) {
        free(object->val_str);
        object->val_str = NULL;
    }

    object->val_size = strlen(val);
    object->val_str = malloc((object->val_size + 1) * sizeof(char));

    if (object->val_str == NULL)
        return -1;

    strcpy(object->val_str, val);

    return 0;
}

int tm_infi_string_value(tm_infi_t* object, char** val) {
    if (TM_INFI_CHECK_P(object, TM_T_STRING))
        return -1;

    *val = object->val_str;
    return 0;
}

int tm_infi_string_length(tm_infi_t *object, unsigned int *s) {
    if (TM_INFI_CHECK_P(object, TM_T_STRING))
        return -1;

    if (object->val_str == NULL)
        return -1;

    *s = object->val_size;
    return 0;
}

/* array */
tm_infi_t* tm_infi_array_new() {
    tm_infi_t* o = tm_infi_new(TM_T_ARRAY);
    if(o != NULL)
        o->val_size = 0;

    return o;
}

int tm_infi_array_append(tm_infi_t* obj, tm_infi_t* val) {
    if (TM_INFI_CHECK_P(obj, TM_T_ARRAY))
        return -1;

    if(obj->val_obj_or_list == NULL) {
        obj->val_obj_or_list = val;
        obj->last = val;
    } else {
        obj->last->next = val;
        obj->last = val;
    }

    obj->val_size += 1;
    return 0;
}

int tm_infi_array_length(tm_infi_t* obj, unsigned int* sz) {
    if (TM_INFI_CHECK_P(obj, TM_T_ARRAY))
        return -1;

    *sz = obj->val_size;
    return 0;
}

int tm_infi_array_get(tm_infi_t* obj, int index, tm_infi_t** val) {
    if (TM_INFI_CHECK_P(obj, TM_T_ARRAY))
        return -1;

    if (index < 0)
        index = (int) obj->val_size + index;

    if (index < 0 || index >= (int) obj->val_size)
        return -2;

    tm_infi_t* o = obj->val_obj_or_list;
    for(int i=0; i < index; i++) {
        if (o == NULL)
            return -3;
        o = o->next;
    }

    *val = o;
    return 0;
}

