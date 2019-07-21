#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>

#include "readfile.c"
#include "css_engine.c"

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

    css_engine_t* engine = css_engine_new();

    mycss_selectors_list_t** selectors = malloc((sizeof(mycss_selectors_list_t*) * options.css_queries.len));
    for (int i=0; i<options.css_queries.len; i++) {
        selectors[i] = css_engine_parse_selector(engine, options.css_queries.strs[i]);
    }

    for (int file_ind = 0; file_ind < files.len; file_ind++) {
        char* selector = options.css_queries.strs[0];

        struct file_contents* file = read_file(files.strs[file_ind]);
        if (file == 0) { return 1; }

        css_engine_parse_html(engine, file);

        for (int sel_ind = 0; sel_ind < options.css_queries.len; sel_ind++) {
            myhtml_collection_t* collection = css_engine_query(engine, selectors[sel_ind]);

            if (collection) {
                for (size_t i=0; i<collection->length; i++) {
                    char* str = css_engine_serialize(collection->list[i]);
                    printf("%s\n", str);
                    free(str);
                }
            }
        }

        free_file(file);
    }

    css_engine_destroy(engine);

}
