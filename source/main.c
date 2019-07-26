#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "options.h"
#include "readfile.h"
#include "myhtml_wrapper.h"

#include "str_vec.h"


int main(int argc, char **argv) {

    const char* COLORS_MAGENTA = "\x1b[35m";
    const char* COLORS_RESET = "\x1b[0m";

    options_t options;
    if (!options_parse(&options, argc, argv)) {
        return 0;
    }

    if (!isatty(STDOUT_FILENO)) {
        COLORS_MAGENTA = "";
        COLORS_RESET = "";
    }

    myhtml_wrapper_t* engine = myhtml_wrapper_new();

    mycss_selectors_list_t** selectors = malloc((sizeof(mycss_selectors_list_t*) * options.css_queries->len));
    for (int i=0; i<options.css_queries->len; i++) {
        selectors[i] = myhtml_wrapper_parse_selector(engine, options.css_queries->strs[i]);
        if (selectors[i] == 0) {
            return 1;
        }
    }

    if (options.files->len == 0) {
        str_vec_push(options.files, (char*) -1);
    }

    for (int file_ind = 0; file_ind < options.files->len; file_ind++) {
        struct file_contents* file = read_file(options.files->strs[file_ind]);
        if (file == 0) { return 1; }

        myhtml_wrapper_parse_html(engine, file);

        for (int sel_ind = 0; sel_ind < options.css_queries->len; sel_ind++) {
            myhtml_collection_t* collection = myhtml_wrapper_query(engine, selectors[sel_ind]);

            if (collection) {
                if (options.list && collection->length > 0) {
                    if (options.files->strs[file_ind] == (char*) -1) {
                        printf("STDIN\n");
                    }
                    else {
                        printf("%s\n", options.files->strs[file_ind]);
                    }
                }
                else {
                    for (size_t i=0; i<collection->length; i++) {
                        if (options.file_prefix) {
                            if (options.files->strs[file_ind] == (char*) -1) {
                                printf("%s%s:%s", COLORS_MAGENTA, "STDIN", COLORS_RESET);
                            }
                            else {
                                printf("%s%s:%s", COLORS_MAGENTA, options.files->strs[file_ind], COLORS_RESET);
                            }
                        }
                        if (options.attributes->len > 0) {
                            for (int attr_ind = 0; attr_ind < options.attributes->len; attr_ind++) {
                                myhtml_tree_attr_t* attr = myhtml_attribute_by_key(collection->list[i], options.attributes->strs[attr_ind], strlen(options.attributes->strs[attr_ind]));
                                if (attr) {
                                    printf("%s%c", attr->value.data, options.line_separator);
                                }
                            }
                        }
                        else if (options.text) {
                            myhtml_wrapper_print_text(collection->list[i]);
                            putc(options.line_separator, stdout);
                        }
                        else if (options.pretty) {
                            myhtml_wrapper_print_pretty(collection->list[i], 0);
                            if (options.line_separator == '\0') putc('\0', stdout);
                        }
                        else {
                            char* str = myhtml_wrapper_serialize(collection->list[i]);
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

    myhtml_wrapper_destroy(engine);

    str_vec_destroy(options.css_queries);
    str_vec_destroy(options.attributes);
    str_vec_destroy(options.files);

}
