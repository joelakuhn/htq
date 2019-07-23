typedef struct {
    const char** strs;
    const char** _tail;
    int len;
    int allocated;
} str_arr_t;

str_arr_t* str_arr_new() {
    str_arr_t* str_arr = malloc(sizeof(str_arr_t));
    str_arr->strs = malloc(sizeof(const char*) * 10);
    str_arr->_tail = str_arr->strs;
    str_arr->allocated = 10;
    str_arr->len = 0;
    return str_arr;
}

void str_arr_push(str_arr_t* str_arr, const char* str) {
    if ((str_arr->len + 1) >= str_arr->allocated) {
        str_arr->strs = (const char**) realloc(str_arr->strs, (str_arr->allocated + 10) * sizeof(const char*));
        str_arr->allocated += 10;
    }
    str_arr->strs[str_arr->len] = str;
    str_arr->len++;
}

const char* str_arr_pop(str_arr_t* str_arr) {
    if (str_arr->len > 0) {
        const char* str = str_arr->strs[str_arr->len - 1];
        str_arr->strs[str_arr->len - 1] = 0;
        str_arr->len--;
        return str;
    }
    else {
        return (const char*) 0;
    }
}

const char* str_arr_shift(str_arr_t* str_arr) {
    if (str_arr->len > 0) {
        const char* str = str_arr->strs[0];
        str_arr->strs++;
        str_arr->len--;
        str_arr->allocated--;
        return str;
    }
    else {
        return 0;
    }
}

void str_arr_destroy(str_arr_t* str_arr) {
    free(str_arr->_tail);
    free(str_arr);
}
