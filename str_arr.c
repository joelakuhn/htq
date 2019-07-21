typedef struct {
    char** strs;
    int len;
    int allocated;
} str_arr_t;

str_arr_t* str_arr_new() {
    str_arr_t* str_arr = malloc(sizeof(str_arr));
    str_arr->strs = malloc(sizeof(char*) * 10);
    str_arr->allocated = 10;
    str_arr->len = 0;
    return str_arr;
}

void str_arr_push(str_arr_t* str_arr, char* str) {
    if ((str_arr->len + 1) >= str_arr->allocated) {
        str_arr->strs = (char**) realloc(str_arr->strs, str_arr->allocated + 10);
        str_arr->allocated += 10;
    }
    str_arr->strs[str_arr->len] = str;
    str_arr->len++;
}

char* str_arr_pop(str_arr_t* str_arr) {
    if (str_arr->len > 0) {
        char* str = str_arr->strs[str_arr->len - 1];
        str_arr->strs[str_arr->len - 1] = 0;
        str_arr->len--;
        return str;
    }
    else {
        return (char*) 0;
    }
}

char* str_arr_shift(str_arr_t* str_arr) {
    if (str_arr->len > 0) {
        char* str = str_arr->strs[0];
        str_arr->strs++;
        str_arr->len--;
        str_arr->allocated--;
        return str;
    }
    else {
        return 0;
    }
}
