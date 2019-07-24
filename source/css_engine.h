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
} css_engine_t;

void css_engine_destroy(css_engine_t *engine);
void css_engine_print_text(myhtml_tree_node_t *node);
void css_engine_print_pretty(myhtml_tree_node_t *node,int level);
char *css_engine_serialize(myhtml_tree_node_t *node);
myhtml_collection_t *css_engine_query(css_engine_t *engine,mycss_selectors_list_t *selectors_list);
mycss_selectors_list_t *css_engine_parse_selector(css_engine_t *engine,const char *selector);
void css_engine_parse_html(css_engine_t *engine,struct file_contents *file);
css_engine_t *css_engine_new();

#endif
