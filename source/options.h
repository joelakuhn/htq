#ifndef OPTIONS
#define OPTIONS

#include "str_arr.h"

typedef struct {
    int nonl;
    char line_separator;
    int pretty;
    int text;
    int list;
    int file_prefix;
    str_arr_t* css_queries;
    str_arr_t* attributes;
    str_arr_t* files;
} options_t;

void print_usage();
int options_parse(options_t* options, int argc, char** argv);

#endif