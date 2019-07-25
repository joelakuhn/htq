#ifndef STR_ARR_H
#define STR_ARR_H

typedef struct {
    const char** strs;
    int len;
    int allocated;
} str_arr_t;

void str_arr_destroy(str_arr_t *str_arr);
const char *str_arr_shift(str_arr_t *str_arr);
const char *str_arr_pop(str_arr_t *str_arr);
void str_arr_push(str_arr_t *str_arr,const char *str);
str_arr_t *str_arr_new();

#endif
