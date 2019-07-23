#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>

#include "readfile.c"
#include "css_engine.c"

#include "str_arr.c"

void print_usage() {
    printf("usage: htq [css_query] [options] [file ...]\n");
    printf("    -c QUERY, --css=QUERY            Specify a css selector\n");
    printf("    -a ATTR, --attr=ATTR             Extract an attribute value\n");
    printf("    -p, --pretty                     Pretty print output\n");
    printf("    -t, --text                       Print text content only\n");
    printf("    -0, --print0                     Separate output by NULL\n");
    printf("    -l, --list-files                 List matching files without matches\n");
    printf("    -h, --help                       Print help message\n");
}

struct options {
    int nonl;
    char line_separator;
    int pretty;
    int text;
    int list;
    str_arr_t* css_queries;
    str_arr_t* attributes;
};

int main(int argc, char **argv) {

    struct options options;
    options.pretty = 0;
    options.list = 0;
    options.text = 0;
    options.line_separator = '\n';
    options.css_queries = str_arr_new();
    options.attributes = str_arr_new();

    str_arr_t* files = str_arr_new();

    static struct option long_options[] = {
        { "nonl", no_argument, 0, 'n' },
        { "print0", no_argument, 0, '0' },
        { "pretty", no_argument, 0, 'p' },
        { "text", no_argument, 0, 't' },
        { "list", no_argument, 0, 'l' },
        { "css_query", no_argument, 0, 'c' },
        { "attribute", no_argument, 0, 'a' },
        { "help", no_argument, 0, 'h' }
    };

    int c = 0;
    int option_index = 0;

    while (1) {

        c = getopt_long(argc, argv, "n0ptlhc:a:", long_options, &option_index);

        if (c == -1) break;

        switch (c)
        {
        case 'n':
            options.nonl = 1;
            break;
        case 'p':
            options.pretty = 1;
            break;
        case 't':
            options.text = 1;
            break;
        case 'l':
            options.list = 1;
            break;
        case 'c':
            if (optarg) {
                str_arr_push(options.css_queries, optarg);
            }
            break;
        case 'a':
            if (optarg) {
                str_arr_push(options.attributes, optarg);
            }
            break;
        case '0':
            options.line_separator = '\0';
            break;
        case 'h':
            print_usage();
            return 0;
        }
    }

    for (int index = optind; index < argc; index++) {
        if (strcmp("-", argv[index]) == 0) {
            str_arr_push(files, (char*) -1);
        }
        else {
            str_arr_push(files, argv[index]);
        }
    }

    if (options.css_queries->len == 0 && files->len > 0) {
        str_arr_push(options.css_queries, str_arr_shift(files));
    }

    if (options.css_queries->len == 0) {
        printf("At least one css query is required.\n\n");
        print_usage();
        return 0;
    }

    css_engine_t* engine = css_engine_new();

    mycss_selectors_list_t** selectors = malloc((sizeof(mycss_selectors_list_t*) * options.css_queries->len));
    for (int i=0; i<options.css_queries->len; i++) {
        selectors[i] = css_engine_parse_selector(engine, options.css_queries->strs[i]);
        if (selectors[i] == 0) {
            return 1;
        }
    }

    if (files->len == 0) {
        str_arr_push(files, (char*) -1);
    }

    for (int file_ind = 0; file_ind < files->len; file_ind++) {
        struct file_contents* file = read_file(files->strs[file_ind]);
        if (file == 0) { return 1; }

        css_engine_parse_html(engine, file);

        for (int sel_ind = 0; sel_ind < options.css_queries->len; sel_ind++) {
            myhtml_collection_t* collection = css_engine_query(engine, selectors[sel_ind]);

            if (collection) {
                if (options.list && collection->length > 0) {
                    if (files->strs[file_ind] == (char*) -1) {
                        printf("STDIN\n");
                    }
                    else {
                        printf("%s\n", files->strs[file_ind]);
                    }
                }
                else {
                    for (size_t i=0; i<collection->length; i++) {
                        if (options.attributes->len > 0) {
                            for (int attr_ind = 0; attr_ind < options.attributes->len; attr_ind++) {
                                myhtml_tree_attr_t* attr = myhtml_attribute_by_key(collection->list[i], options.attributes->strs[attr_ind], strlen(options.attributes->strs[attr_ind]));
                                if (attr) {
                                    printf("%s%c", attr->value.data, options.line_separator);
                                }
                            }
                        }
                        else if (options.text) {
                            css_engine_print_text(collection->list[i]);
                            putc('\n', stdout);
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
                myhtml_collection_destroy(collection);
            }
        }

        free_file(file);
    }

    free(selectors);

    css_engine_destroy(engine);

    str_arr_destroy(options.css_queries);
    str_arr_destroy(options.attributes);
    str_arr_destroy(files);

}
