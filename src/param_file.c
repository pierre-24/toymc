#include "param_file.h"

/* objects */

/**
 * Create an input file object of type \p t.
 * @param t the type
 * @post object is initialized with type \p t.
 * @return the initialized object, or \p NULL if malloc failed
 */
tm_parf_t* tm_parf_new(tm_parf_type t) {
    tm_parf_t* j = NULL;
    j = malloc(sizeof(tm_parf_t));
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

/**
 * Delete \p obj.
 * @pre \code{.c} obj != NULL \endcode
 * @param obj the object to delete
 * @return 0 if the object was delete, something else otherwise
 */
int tm_parf_delete(tm_parf_t* obj) {
    if(obj == NULL)
        return -1;

    int r = 0;
    if(obj->next != NULL)
        r = tm_parf_delete(obj->next);

    if (r < 0)
        return r;

    if(obj->key != NULL)
        free(obj->key);

    if(TM_parf_IS(obj, TM_T_STRING) && obj->val_str != NULL)
        free(obj->val_str);

    if ((TM_parf_IS(obj, TM_T_OBJECT) || TM_parf_IS(obj, TM_T_LIST)) && obj->val_obj_or_list != NULL)
        r = tm_parf_delete(obj->val_obj_or_list);

    if (r < 0)
        return r;

    free(obj);

    return 0;
}

/**
 * Create an input file object of type \p TM_T_OBJECT
 * @return the initialized object, or \p NULL if malloc failed
 */
tm_parf_t* tm_parf_object_new() {
    tm_parf_t* o = tm_parf_new(TM_T_OBJECT);
    if(o != NULL)
        o->val_size = 0;

    return o;

}

/**
 * Set value at \p key to \p val
 * @pre \code{.c}
 * obj != NULL && TM_PARF_CHECK_P(obj, TM_T_OBJECT)
 * && key != NULL
 * \endcode
 * @param obj the object
 * @param key the key
 * @param val the new value
 * @post value at key \p key is set to \p val
 * @return 0 if the value is set, something else otherwise
 */
int tm_parf_object_set(tm_parf_t* obj, char* key, tm_parf_t* val) {
    if (TM_PARF_CHECK_P(obj, TM_T_OBJECT))
        return -1;

    if(key == NULL || val == NULL)
        return -2;

    // key
    val->key = malloc(sizeof(char) * (strlen(key) + 1));
    if(val->key == NULL)
        return -3;

    strcpy(val->key, key);


    // value
    tm_parf_t* o = obj->val_obj_or_list, *prev = NULL;
    int set = 0;
    while (o != NULL) { // look first if it does exist
        if(strcmp(o->key, key) == 0) {
            val->next = o->next;
            o->next = NULL;
            tm_parf_delete(o);
            if(prev == NULL)
                obj->val_obj_or_list = val;
            else
                prev->next = val;

            if(val->next == NULL)
                obj->last = val;

            set = 1;
            break;
        }

        prev = o;
        o = o->next;
    }
    if(!set) { // if not, adds it
        if(obj->val_obj_or_list == NULL) {
            obj->val_obj_or_list = val;
            obj->last = val;
        } else {
            obj->last->next = val;
            obj->last = val;
        }

        obj->val_size += 1;
    }
    return 0;
}

/**
 * Get the value with key \p key
 * @pre \code{.c}
 * obj != NULL && TM_PARF_CHECK_P(obj, TM_T_OBJECT)
 * && key != NULL
 * \endcode
 * @param object the object
 * @param key the key
 * @param val the value
 * @post \p val point on the value
 * @return 0 if the object was found, something else otherwise
 */
int tm_parf_object_get(tm_parf_t* obj, char* key, tm_parf_t** val) {
    if (TM_PARF_CHECK_P(obj, TM_T_OBJECT))
        return -1;

    tm_parf_t* o = obj->val_obj_or_list;
    while (o != NULL) {
        if (strcmp(o->key, key) == 0) {
            *val = o;
            return 0;
        }

        o = o->next;
    }

    return -2;
}

/* boolean */

/**
 * Create an input file object of type \p TM_T_BOOLEAN
 * @return the initialized object, or \p NULL if malloc failed
 */
tm_parf_t* tm_parf_boolean_new(int val) {
    tm_parf_t* o = tm_parf_new(TM_T_BOOLEAN);
    if(o != NULL)
        tm_parf_boolean_set(o, val);

    return o;
}

/**
 * Change the value
 * @pre \code{.c}
 * obj != NULL && TM_PARF_CHECK_P(obj, TM_T_BOOLEAN)
 * \endcode
 * @param object the boolean object
 * @param val new value
 * @post object is set to \p val
 * @return 0 if the value is set, something else otherwise
 */
int tm_parf_boolean_set(tm_parf_t* object, int val) {
    if (TM_PARF_CHECK_P(object, TM_T_BOOLEAN))
        return -1;

    object->val_int = val;
    return 0;
}

/**
 * Get the value
 * @pre \code{.c}
 * obj != NULL && TM_PARF_CHECK_P(obj, TM_T_BOOLEAN)
 * \endcode
 * @param object the boolean object
 * @param value the value
 * @post \p value contains the value
 * @return 0 if everything went well, something else otherwise
 */
int tm_parf_boolean_value(tm_parf_t* object, int* value) {
    if (TM_PARF_CHECK_P(object, TM_T_BOOLEAN))
        return -1;

    *value = object->val_int;
    return 0;
}

/* integers */

/**
 * Create an input file object of type \p TM_T_INTEGER
 * @return the initialized object, or \p NULL if malloc failed
 */
tm_parf_t* tm_parf_integer_new(long val) {
    tm_parf_t* o = tm_parf_new(TM_T_INTEGER);
    if(o != NULL)
        tm_parf_integer_set(o, val);
    return o;
}

/**
 * Change the value
 * @pre \code{.c}
 * obj != NULL && TM_PARF_CHECK_P(obj, TM_T_INTEGER)
 * \endcode
 * @param obj the integer object
 * @param val new value
 * @post object is set to \p val
 * @return 0 if the value is set, something else otherwise
 */
int tm_parf_integer_set(tm_parf_t* obj, long val) {
    if (TM_PARF_CHECK_P(obj, TM_T_INTEGER))
        return -1;

    obj->val_int = val;
    return 0;
}

/**
 * Get the value
 * @pre \code{.c}
 * obj != NULL && TM_PARF_CHECK_P(obj, TM_T_INTEGER)
 * \endcode
 * @param obj the integer object
 * @param val the value
 * @post \p value contains the value
 * @return 0 if everything went well, something else otherwise
 */
int tm_parf_integer_value(tm_parf_t* obj, long *val) {
    if (TM_PARF_CHECK_P(obj, TM_T_INTEGER))
        return -1;

    *val = obj->val_int;
    return 0;
}

/* real */

/**
 * Create an input file object of type \p TM_T_REAL
 * @return the initialized object, or \p NULL if malloc failed
 */
tm_parf_t* tm_parf_real_new(double val) {
    tm_parf_t *o = tm_parf_new(TM_T_REAL);
    if (o != NULL)
        tm_parf_real_set(o, val);
    return o;
}

/**
 * Change the value
 * @pre \code{.c}
 * obj != NULL && TM_PARF_CHECK_P(obj, TM_T_REAL)
 * \endcode
 * @param object the real object
 * @param val new value
 * @post object is set to \p val
 * @return 0 if the value is set, something else otherwise
 */
int tm_parf_real_set(tm_parf_t* object, double val) {
    if (TM_PARF_CHECK_P(object, TM_T_REAL))
        return -1;

    object->val_real = val;
    return 0;
}

/**
 * Get the value
 * @pre \code{.c}
 * obj != NULL && TM_PARF_CHECK_P(obj, TM_T_REAL)
 * \endcode
 * @param object the real object
 * @param value the value
 * @post value contains the value
 * @return 0 if everything went well, something else otherwise
 */
int tm_parf_real_value(tm_parf_t* object, double * value) {
    if (TM_PARF_CHECK_P(object, TM_T_REAL))
        return -1;

    *value = object->val_real;
    return 0;
}

/* string */

/**
 * Create an input file object of type \p TM_T_STRING. Copy the string.
 * @pre \code{.c}
 * val != NULL
 * \endcode
 * @return the initialized object, or \p NULL if malloc failed
 */
tm_parf_t* tm_parf_string_new(char* val) {
    tm_parf_t* o = tm_parf_new(TM_T_STRING);
    if(o != NULL)
        tm_parf_string_set(o, val);

    return o;
}

/**
 * Change the value of the string. Copy the string.
 * @pre \code{.c}
 * obj != NULL && TM_PARF_CHECK_P(obj, TM_T_STRING)
 * && val != NULL
 * \endcode
 * @param object the string object
 * @param val new value of the string
 * @post object is set to \p val
 * @return 0 if the value is set, something else otherwise
 */
int tm_parf_string_set(tm_parf_t* object, char* val) {
    if (TM_PARF_CHECK_P(object, TM_T_STRING))
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

/**
 * Get the value of the string, as a pointer
 * @pre \code{.c}
 * obj != NULL && TM_PARF_CHECK_P(obj, TM_T_STRING)
 * \endcode
 * @param object the string object
 * @param val a pointer to the value
 * @post \p val contains the value
 * @return 0 if everything went well, something else otherwise
 */
int tm_parf_string_value(tm_parf_t* object, char **val) {
    if (TM_PARF_CHECK_P(object, TM_T_STRING))
        return -1;

    *val = object->val_str;
    return 0;
}
/**
 * Get the length of the string
 * @pre \code{.c}
 * obj != NULL && TM_PARF_CHECK_P(obj, TM_T_STRING)
 * \endcode
 * @param obj the object
 * @param s the size
 * @post \p s is set to the size of the string
 * @return 0 if everything went well, something else otherwise
 */
int tm_parf_string_length(tm_parf_t *object, unsigned int *s) {
    if (TM_PARF_CHECK_P(object, TM_T_STRING))
        return -1;

    if (object->val_str == NULL)
        return -1;

    *s = object->val_size;
    return 0;
}

/* list */

/**
 * Create an input file object of type \p TM_T_LIST
 * @return the initialized object, or \p NULL if malloc failed.
 */
tm_parf_t* tm_parf_list_new() {
    tm_parf_t* o = tm_parf_new(TM_T_LIST);
    if(o != NULL)
        o->val_size = 0;

    return o;
}

/**
 * Append an element at the end of the list
 * @pre \code{.c}
 * obj != NULL && TM_PARF_CHECK_P(obj, TM_T_LIST)
 * \endcode
 * @param obj the list
 * @param val the value to add
 * @post \p val is added to the list
 * @return 0 if the element was added, something else otherwise
 */
int tm_parf_list_append(tm_parf_t* obj, tm_parf_t* val) {
    if (TM_PARF_CHECK_P(obj, TM_T_LIST))
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

/**
 * Get the length of the list
 * @pre \code{.c}
 * obj != NULL && TM_PARF_CHECK_P(obj, TM_T_LIST)
 * \endcode
 * @param obj the object
 * @param sz the size
 * @post \p sz is set to the size of the list
 * @return 0 if everything went well, something else otherwise
 */
int tm_parf_list_length(tm_parf_t* obj, unsigned int* sz) {
    if (TM_PARF_CHECK_P(obj, TM_T_LIST))
        return -1;

    *sz = obj->val_size;
    return 0;
}

/**
 * Get element \p index. If \p index is negative, start from the last element.
 * @pre \code{.c}
 * obj != NULL
 * && TM_PARF_CHECK_P(obj, TM_T_LIST)
 * && (0 <= index < tm_parf_list_length(obj) || -tm_parf_list_length(obj) <= index < 0)
 * \endcode
 * @param obj the list object
 * @param index the index
 * @param val the value
 * @post \p val point to the object at index \p index, if there is such index.
 * @return 0 if there is an object at the requested index, something else otherwise
 */
int tm_parf_list_get(tm_parf_t* obj, int index, tm_parf_t** val) {
    if (TM_PARF_CHECK_P(obj, TM_T_LIST))
        return -1;

    if (index < 0)
        index = (int) obj->val_size + index;

    if (index < 0 || index >= (int) obj->val_size)
        return -2;

    tm_parf_t* o = obj->val_obj_or_list;
    for(int i=0; i < index; i++) {
        if (o == NULL)
            return -3;
        o = o->next;
    }

    *val = o;
    return 0;
}

/* iterator */

/**
 * Create an iterator
 * @pre \code{.c}
 * TM_PARF_CHECK_P(obj, TM_T_LIST) || TM_PARF_CHECK_P(obj, TM_T_OBJECT)
 * \endcode
 * @return the initialized object, or \p NULL if malloc failed
 */
tm_parf_iterator* tm_parf_iterator_new(tm_parf_t* obj) {
    if (!TM_PARF_CHECK_P(obj, TM_T_LIST) && !TM_PARF_CHECK_P(obj, TM_T_OBJECT))
        return NULL;

    tm_parf_iterator* it = malloc(sizeof(tm_parf_iterator));
    if(it != NULL) {
        it->obj = obj;
        it->next = obj->val_obj_or_list;
    }

    return it;
}

/**
 * Delete iterator \p it.
 * @pre \code{.c} it != NULL\endcode
 * @param it the iterator
 * @return 0 if the iterator was delete, something else otherwise
 */
int tm_parf_iterator_delete(tm_parf_iterator* it) {
    if(it == NULL)
        return -1;

    free(it);
    return 0;
}

/**
 *
 * @param it
 * @return 0 if \p it is \p NULL or if there is no next value, 1 otherwise
 * @example \code{.c}
 * tm_parf_iterator* it = tm_iterator_new(obj);
 * tm_parf_t* elmt;
 * while(tm_parf_operator_has_next(it)) {
 *  tm_parf_iterator_next(it, &obj);
 * }
 * \endcode
 */
int tm_parf_iterator_has_next(tm_parf_iterator* it) {
    if(it == NULL)
        return 0;

    return it->next != NULL;
}

/**
 * Get the next element (if there is one)
 * @pre \code{.c} it != NULL && tm_parf_iterator_has_next(it)\endcode
 * @param it the iterator
 * @param obj the next element
 * @post \p it is iterated, \p obj is set if there was an element
 * @return 0 if there is an element, something else otherwise.
 */
int tm_parf_iterator_next(tm_parf_iterator* it, tm_parf_t** obj) {
    if(it == NULL)
        return -1;

    if(it->next == NULL)
        return -1;

    *obj = it->next;
    it->next = it->next->next;

    return 0;
}

