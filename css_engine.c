#include <mycss/mycss.h>
#include <modest/finder/finder.h>
#include <myencoding/encoding.h>
#include <myhtml/serialization.h>

#include "readfile.c"

typedef struct {
    myhtml_t* myhtml;
    myhtml_tree_t* tree;
    mycss_t* mycss;
    mycss_entry_t* entry;
    modest_finder_t* finder;
} css_engine_t;

css_engine_t* css_engine_new() {
    mystatus_t status;
    css_engine_t* engine = malloc(sizeof(css_engine_t));

    engine->myhtml = myhtml_create();
    status = myhtml_init(engine->myhtml, MyHTML_OPTIONS_PARSE_MODE_SINGLE, 1, 0);

    engine->tree = myhtml_tree_create();
    myhtml_tree_init(engine->tree, engine->myhtml);

    engine->mycss = mycss_create();
    status = mycss_init(engine->mycss);

    engine->entry = mycss_entry_create();
    status = mycss_entry_init(engine->mycss, engine->entry);

    engine->finder = modest_finder_create_simple();
    status = modest_finder_init(engine->finder);

    return engine;
}

void css_engine_parse_html(css_engine_t* engine, struct file_contents* file) {

    mystatus_t status;

    // myencoding_t encoding;
    // const char** text;
    // size_t size;
    // myencoding_detect_and_cut_bom(file.data, file.len, &encoding, text, &size);

    // status = myhtml_parse(engine->tree, encoding, *text, size);

    status = myhtml_parse(engine->tree, MyENCODING_UTF_8, file->data, file->len);

}

mycss_selectors_list_t* css_engine_parse_selector(css_engine_t* engine, char* selector) {
    mystatus_t status;

    mycss_selectors_t* selectors = mycss_entry_selectors(engine->entry);
    size_t selector_len = strlen(selector);

    mycss_selectors_list_t* selectors_list = mycss_selectors_parse(selectors, MyENCODING_DEFAULT, selector, selector_len, &status);

    return selectors_list;
}

myhtml_collection_t* css_engine_query(css_engine_t* engine, mycss_selectors_list_t* selectors_list) {

    myhtml_collection_t* collection = 0;
    modest_finder_by_selectors_list(engine->finder, engine->tree->node_html, selectors_list, &collection);

    return collection;
}

char* css_engine_serialize(myhtml_tree_node_t* node) {
    mycore_string_raw_t str = {0};
    myhtml_serialization_node(node, &str);
    return str.data;
}

void css_engine_destroy(css_engine_t* engine) {
    myhtml_destroy(engine->myhtml);
    myhtml_tree_destroy(engine->tree);
    mycss_destroy(engine->mycss, true);
    mycss_entry_destroy(engine->entry, true);
    modest_finder_destroy(engine->finder, true);
}