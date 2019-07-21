#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>

#include "css_query.c"

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

    char* selector = options.css_queries.strs[0];

    struct res_html res = load_html_file(files.strs[0]);

    // const char *selector = "div > :nth-child(2n+1):not(:has(a))";

    /* init MyHTML and parse HTML */
    myhtml_tree_t *html_tree = parse_html(res.html, strlen(res.html));

    /* create css parser and finder for selectors */
    mycss_entry_t *css_entry = create_css_parser();
    modest_finder_t *finder = modest_finder_create_simple();

    /* parse selectors */
    mycss_selectors_list_t *selectors_list = prepare_selector(css_entry, selector, strlen(selector));

    /* find nodes by selector */
    myhtml_collection_t *collection = NULL;
    modest_finder_by_selectors_list(finder, html_tree->node_html, selectors_list, &collection);

    /* print result */
    /* print selector */
    printf("Incoming selector:\n\t");
    mycss_selectors_serialization_list(mycss_entry_selectors(css_entry),
                                       selectors_list, serialization_callback, NULL);
    printf("\n\n");

    /* print tree */
    // printf("Incoming tree:\n\t");
    // myhtml_serialization_tree_callback(html_tree->node_html, serialization_callback, NULL);

    /* print found result */
    printf("\n\nFound nodes:");
    print_found_result(html_tree, collection);

    /* destroy all */
    myhtml_collection_destroy(collection);

    /* destroy selector list */
    mycss_selectors_list_destroy(mycss_entry_selectors(css_entry), selectors_list, true);

    /* destroy Modest Finder */
    modest_finder_destroy(finder, true);

    /* destroy MyCSS */
    mycss_t *mycss = css_entry->mycss;
    mycss_entry_destroy(css_entry, true);
    mycss_destroy(mycss, true);

    /* destroy MyHTML */
    myhtml_t* myhtml = html_tree->myhtml;
    myhtml_tree_destroy(html_tree);
    myhtml_destroy(myhtml);


}
