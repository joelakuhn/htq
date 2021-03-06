#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "options.h"
#include "readfile.h"
#include "myhtml_wrapper.h"

#include "str_vec.h"



const char* COLORS_MAGENTA = "\x1b[35m";
const char* COLORS_RESET = "\x1b[0m";

#define fprintf_highlighted(file, format, args...) fprintf(file, "%s" format "%s", COLORS_MAGENTA, args, COLORS_RESET)
void fprintf_highlighted_check_tty(options_t* options) {
    if (options->output_file != stdout || !isatty(STDOUT_FILENO)) {
        COLORS_MAGENTA = "";
        COLORS_RESET = "";
    }
}



void print_results(options_t* options, myhtml_collection_t* collection, const char* file) {
    if (options->quiet) return;

    if (options->count) {
        if (options->file_prefix == 1) {
            fprintf_highlighted(options->output_file, "%s: %zu%c", file, collection->length, options->line_separator);
        }
    }
    else if (options->list && collection->length > 0) {
        fprintf(options->output_file, "%s%c", file, options->line_separator);
    }
    else {
        for (size_t i=0; i<collection->length; i++) {
            if (options->file_prefix) {
                fprintf_highlighted(options->output_file, "%s:", file);
            }
            if (options->attributes->len > 0) {
                for (int attr_ind = 0; attr_ind < options->attributes->len; attr_ind++) {
                    myhtml_tree_attr_t* attr = myhtml_attribute_by_key(collection->list[i], options->attributes->strs[attr_ind], strlen(options->attributes->strs[attr_ind]));
                    if (attr) {
                        fprintf(options->output_file, "%s%c", attr->value.data, options->line_separator);
                    }
                }
            }
            else if (options->text) {
                myhtml_wrapper_print_text(options->output_file, collection->list[i]);
                fprintf(options->output_file, "%c", options->line_separator);
            }
            else if (options->pretty) {
                myhtml_wrapper_print_pretty(options->output_file, collection->list[i], 0, options->whitespace);
                if (options->line_separator == '\0') fprintf(options->output_file, "%c", '\0');
            }
            else {
                char* str = myhtml_wrapper_serialize(collection->list[i]);
                fprintf(options->output_file, "%s%c", str, options->line_separator);
                free(str);
            }
        }
    }
}



int main(int argc, char **argv) {

    options_t options;
    if (!options_parse(&options, argc, argv)) {
        return 1;
    }

    fprintf_highlighted_check_tty(&options);

    myhtml_wrapper_t* engine = myhtml_wrapper_new();

    mycss_selectors_list_t** selectors = malloc((sizeof(mycss_selectors_list_t*) * options.css_queries->len));
    for (int i=0; i<options.css_queries->len; i++) {
        selectors[i] = myhtml_wrapper_parse_selector(engine, options.css_queries->strs[i]);
        if (selectors[i] == 0) {
            return 1;
        }
    }

    if (options.files->len == 0) {
        str_vec_push(options.files, "(standard input)");
    }

    int total_matches = 0;

    for (int file_ind = 0; file_ind < options.files->len; file_ind++) {
        struct file_contents* file = read_file(options.files->strs[file_ind]);
        if (file == 0) { return 1; }

        myhtml_wrapper_parse_html(engine, file);

        for (int sel_ind = 0; sel_ind < options.css_queries->len; sel_ind++) {
            myhtml_collection_t* collection = myhtml_wrapper_query(engine, selectors[sel_ind]);

            if (collection) {
                print_results(&options, collection, options.files->strs[file_ind]);
                total_matches += collection->length;
                myhtml_collection_destroy(collection);
            }
        }

        free_file(file);
    }

    if (!options.quiet && options.count && !options.file_prefix) {
        fprintf(options.output_file, "%i", total_matches);
    }

    free(selectors);

    myhtml_wrapper_destroy(engine);

    str_vec_destroy(options.css_queries);
    str_vec_destroy(options.attributes);
    str_vec_destroy(options.files);
    free(options.whitespace);

    if (options.output_file != stdout) {
        fclose(options.output_file);
    }

    return total_matches > 0 ? 0 : 1;

}
