#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "options.h"
#include "str_arr.h"


void print_usage() {
    printf("usage: htq [css_query] [options] [file ...]\n");
    printf("    -c QUERY, --css=QUERY            Specify a css selector\n");
    printf("    -a ATTR, --attr=ATTR             Extract an attribute value\n");
    printf("    -p, --pretty                     Pretty print output\n");
    printf("    -t, --text                       Print text content only\n");
    printf("    -0, --print0                     Separate output by NULL\n");
    printf("    -l, --list-files                 Only print matching file names\n");
    printf("    -h, --prefix                     Print file name prefix\n");
    printf("    -H, --no-prefix                  Don't file name prefix\n");
    printf("    --help                           Print help message\n");
}

int options_parse(options_t* options, int argc, char** argv) {

    options->pretty = 0;
    options->list = 0;
    options->text = 0;
    options->file_prefix = -1;
    options->line_separator = '\n';
    options->css_queries = str_arr_new();
    options->attributes = str_arr_new();
    options->files = str_arr_new();

    static struct option long_options[] = {
        { "nonl", no_argument, 0, 'n' },
        { "print0", no_argument, 0, '0' },
        { "pretty", no_argument, 0, 'p' },
        { "text", no_argument, 0, 't' },
        { "list", no_argument, 0, 'l' },
        { "css", no_argument, 0, 'c' },
        { "attr", no_argument, 0, 'a' },
        { "prefix", no_argument, 0, 'h' },
        { "no-prefix", no_argument, 0, 'H' },
        { "help", no_argument, 0, '?' }
    };

    int c = 0;
    int option_index = 0;

    while (1) {

        c = getopt_long(argc, argv, "n0ptlhH?c:a:", long_options, &option_index);

        if (c == -1) break;

        switch (c)
        {
        case 'n':
            options->nonl = 1;
            break;
        case 'p':
            options->pretty = 1;
            break;
        case 't':
            options->text = 1;
            break;
        case 'l':
            options->list = 1;
            break;
        case 'c':
            if (optarg) {
                str_arr_push(options->css_queries, optarg);
            }
            break;
        case 'a':
            if (optarg) {
                str_arr_push(options->attributes, optarg);
            }
            break;
        case '0':
            options->line_separator = '\0';
            break;
        case 'h':
            options->file_prefix = 1;
            break;
        case 'H':
            options->file_prefix = 0;
            break;
        case '?':
            print_usage();
            return 0;
        }
    }

    for (int index = optind; index < argc; index++) {
        if (strcmp("-", argv[index]) == 0) {
            str_arr_push(options->files, (char*) -1);
        }
        else {
            str_arr_push(options->files, argv[index]);
        }
    }

    if (options->css_queries->len == 0 && options->files->len > 0) {
        str_arr_push(options->css_queries, str_arr_shift(options->files));
    }

    if (options->css_queries->len == 0) {
        printf("At least one css query is required.\n\n");
        print_usage();
        return 0;
    }

    if (options->file_prefix == -1) {
        options->file_prefix = options->files->len > 1;

    }

    return 1;

}