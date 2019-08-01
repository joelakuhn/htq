#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "options.h"
#include "str_vec.h"


void print_usage() {
    printf("usage: htq [css_query] [option ...] [file ...]\n");
    printf("    -c QUERY, --css QUERY            Specify a css selector\n");
    printf("    -a ATTR, --attr ATTR             Extract an attribute value\n");
    printf("    -i SPACES, --indent SPACES       Number of spaces for indent\n");
    printf("    -p, --pretty                     Pretty print output\n");
    printf("    -t, --text                       Print text content only\n");
    printf("    -0, --print0                     Separate output by NULL\n");
    printf("    -l, --list                       Only print matching file names\n");
    printf("    -h, --prefix                     Print file name prefix\n");
    printf("    -H, --no-prefix                  Don't file name prefix\n");
    printf("    -C, --count                      Print the number of matches\n");
    printf("    --help                           Print help message\n");
}

int options_parse(options_t* options, int argc, char** argv) {

    options->pretty = 0;
    options->list = 0;
    options->text = 0;
    options->file_prefix = -1;
    options->count = 0;
    options->line_separator = '\n';
    options->css_queries = str_vec_new();
    options->attributes = str_vec_new();
    options->files = str_vec_new();
    options->whitespace = 0;
    options->indent_level = 2;

    static struct option long_options[] = {
        { "css", required_argument, 0, 'c' },
        { "attr", required_argument, 0, 'a' },
        { "indent", required_argument, 0, 'i' },
        { "pretty", no_argument, 0, 'p' },
        { "text", no_argument, 0, 't' },
        { "print0", no_argument, 0, '0' },
        { "list", no_argument, 0, 'l' },
        { "prefix", no_argument, 0, 'h' },
        { "no-prefix", no_argument, 0, 'H' },
        { "count", no_argument, 0, 'C' },
        { "help", no_argument, 0, '?' },
    };

    int c = 0;
    int option_index = 0;

    while (1) {

        c = getopt_long(argc, argv, "n0ptlhHC?i:c:a:", long_options, &option_index);

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
                str_vec_push(options->css_queries, optarg);
            }
            break;
        case 'a':
            if (optarg) {
                str_vec_push(options->attributes, optarg);
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
        case 'C':
            options->count = 1;
            break;
        case 'i':
            if (optarg) {
                int opt_len = strlen(optarg);
                int level = opt_len == 1 ? optarg[0] - '0' : 0;
                if (opt_len != 1 || level < 2 || level > 8) {
                    printf("enter an integer between 2-8");
                    return 0;
                }
                options->indent_level = level;
            }
            break;
        case '?':
            print_usage();
            return 0;
        }
    }

    for (int index = optind; index < argc; index++) {
        if (strcmp("-", argv[index]) == 0) {
            str_vec_push(options->files, "(standard input)");
        }
        else {
            str_vec_push(options->files, argv[index]);
        }
    }

    if (options->css_queries->len == 0 && options->files->len > 0) {
        str_vec_push(options->css_queries, str_vec_shift(options->files));
    }

    if (options->css_queries->len == 0) {
        printf("At least one css query is required.\n\n");
        print_usage();
        return 0;
    }

    if (options->file_prefix == -1) {
        options->file_prefix = options->files->len > 1;
    }

    options->whitespace = malloc(sizeof(char) * options->indent_level + 1);
    for (int i=0; i<options->indent_level; i++) {
        options->whitespace[i] = ' ';
    }
    options->whitespace[options->indent_level] = '\0';

    return 1;

}