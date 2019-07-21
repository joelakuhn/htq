#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>

#include <myhtml/myhtml.h>
#include <myhtml/serialization.h>

#include "str_arr.c"

struct options {
    int nonl;
    str_arr_t css_queries;
};

int main(int argc, char **argv) {

    struct options options;
    options.css_queries = *str_arr_new();

    str_arr_t files = *str_arr_new();

    static struct option long_options[] = {
        { "nonl", no_argument, 0, 'n' },
        { "css_query", no_argument, 0, 'c' }
    };

    int c = 0;
    int option_index = 0;

    while (1) {

        c = getopt_long(argc, argv, "nc:", long_options, &option_index);

        if (c == -1) break;

        switch (c)
        {
        case 'n':
            options.nonl = 1;
            break;
        case 'c':
            if (optarg) {
                str_arr_push(&options.css_queries, optarg);
            }
            break;
        }
    }

    for (int index = optind; index < argc; index++) {
        str_arr_push(&files, argv[index]);
    }

    if (options.css_queries.len == 0 && files.len > 0) {
        str_arr_push(&options.css_queries, str_arr_shift(&files));
    }



}
