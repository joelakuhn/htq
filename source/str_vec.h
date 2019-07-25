#ifndef str_vec_H
#define str_vec_H

typedef struct {
    const char** strs;
    int len;
    int allocated;
} str_vec_t;

void str_vec_destroy(str_vec_t *str_vec);
const char *str_vec_shift(str_vec_t *str_vec);
const char *str_vec_pop(str_vec_t *str_vec);
void str_vec_push(str_vec_t *str_vec,const char *str);
str_vec_t *str_vec_new();

#endif
