#include <string.h>
#include <stdlib.h>

#include "str_vec.h"

str_vec_t* str_vec_new() {
    str_vec_t* str_vec = malloc(sizeof(str_vec_t));
    str_vec->strs = malloc(sizeof(const char*) * 10);
    str_vec->allocated = 10;
    str_vec->len = 0;
    return str_vec;
}

void str_vec_push(str_vec_t* str_vec, const char* str) {
    if ((str_vec->len + 1) >= str_vec->allocated) {
        str_vec->strs = (const char**) realloc(str_vec->strs, (str_vec->allocated + 10) * sizeof(const char*));
        str_vec->allocated += 10;
    }
    str_vec->strs[str_vec->len] = str;
    str_vec->len++;
}

const char* str_vec_pop(str_vec_t* str_vec) {
    if (str_vec->len > 0) {
        const char* str = str_vec->strs[str_vec->len - 1];
        str_vec->strs[str_vec->len - 1] = 0;
        str_vec->len--;
        return str;
    }
    else {
        return (const char*) 0;
    }
}

const char* str_vec_shift(str_vec_t* str_vec) {
    if (str_vec->len > 0) {
        const char* str = str_vec->strs[0];
        const char** new_strs = malloc(sizeof(const char*) * str_vec->allocated);

        memcpy(new_strs, str_vec->strs + 1, (str_vec->allocated - 1) * sizeof(const char*));
        free(str_vec->strs);
        str_vec->strs = new_strs;

        str_vec->len--;
        return str;
    }
    else {
        return 0;
    }
}

void str_vec_destroy(str_vec_t* str_vec) {
    free(str_vec->strs);
    free(str_vec);
}
