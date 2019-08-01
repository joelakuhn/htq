#ifndef CSS_ENGINE_H
#define CSS_ENGINE_H

#include <mycss/mycss.h>
#include <modest/finder/finder.h>
#include <myencoding/encoding.h>
#include <myhtml/serialization.h>

#include "readfile.h"

typedef struct {
    myhtml_t* myhtml;
    myhtml_tree_t* tree;
    mycss_t* mycss;
    mycss_entry_t* entry;
    modest_finder_t* finder;
} myhtml_wrapper_t;

void myhtml_wrapper_destroy(myhtml_wrapper_t *engine);
void myhtml_wrapper_print_text(myhtml_tree_node_t *node);
void myhtml_wrapper_print_pretty(myhtml_tree_node_t *node, int level, char* whitespace);
char *myhtml_wrapper_serialize(myhtml_tree_node_t *node);
myhtml_collection_t *myhtml_wrapper_query(myhtml_wrapper_t *engine,mycss_selectors_list_t *selectors_list);
mycss_selectors_list_t *myhtml_wrapper_parse_selector(myhtml_wrapper_t *engine,const char *selector);
void myhtml_wrapper_parse_html(myhtml_wrapper_t *engine,struct file_contents *file);
myhtml_wrapper_t *myhtml_wrapper_new();

#endif
