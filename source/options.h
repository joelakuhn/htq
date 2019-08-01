#ifndef OPTIONS
#define OPTIONS

#include "str_vec.h"

typedef struct {
    int nonl;
    char line_separator;
    int pretty;
    int text;
    int list;
    int file_prefix;
    int count;
    int indent_level;
    char* whitespace;
    str_vec_t* css_queries;
    str_vec_t* attributes;
    str_vec_t* files;
} options_t;

void print_usage();
int options_parse(options_t* options, int argc, char** argv);

#endif