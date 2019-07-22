#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>

#include "readfile.c"
#include "css_engine.c"

#include "str_arr.c"

struct options {
    int nonl;
    char line_separator;
    int pretty;
    str_arr_t css_queries;
    str_arr_t attributes;
};

int main(int argc, char **argv) {

    struct options options;
    options.pretty = 0;
    options.line_separator = '\n';
    options.css_queries = *str_arr_new();

    str_arr_t files = *str_arr_new();

    static struct option long_options[] = {
        { "nonl", no_argument, 0, 'n' },
        { "print0", no_argument, 0, '0' },
        { "pretty", no_argument, 0, 'p' },
        { "css_query", no_argument, 0, 'c' },
        { "attribute", no_argument, 0, 'a' }
    };

    int c = 0;
    int option_index = 0;

    while (1) {

        c = getopt_long(argc, argv, "n0pc:a:", long_options, &option_index);

        if (c == -1) break;

        switch (c)
        {
        case 'n':
            options.nonl = 1;
            break;
        case 'p':
            options.pretty = 1;
            break;
        case 'c':
            if (optarg) {
                str_arr_push(&options.css_queries, optarg);
            }
            break;
        case 'a':
            if (optarg) {
                str_arr_push(&options.attributes, optarg);
            }
            break;
        case '0':
            options.line_separator = '\0';
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
        if (selectors[i] == 0) {
            return 1;
        }
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
                    if (options.attributes.len > 0) {
                        for (int attr_ind = 0; attr_ind < options.attributes.len; attr_ind++) {
                            myhtml_tree_attr_t* attr = myhtml_attribute_by_key(collection->list[i], options.attributes.strs[attr_ind], strlen(options.attributes.strs[attr_ind]));
                            if (attr) {
                                printf("%s%c", attr->value.data, options.line_separator);
                            }
                        }
                    }
                    else if (options.pretty) {
                        css_engine_print_pretty(collection->list[i], 0);
                    }
                    else {
                        char* str = css_engine_serialize(collection->list[i]);
                        printf("%s%c", str, options.line_separator);
                        free(str);
                    }
                }
            }
        }

        free_file(file);
    }

    css_engine_destroy(engine);

}
