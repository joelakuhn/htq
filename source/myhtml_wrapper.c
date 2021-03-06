#include <stdlib.h>

#include <mycss/mycss.h>
#include <modest/finder/finder.h>
#include <myencoding/encoding.h>
#include <myhtml/serialization.h>

#include "myhtml_wrapper.h"
#include "readfile.h"

myhtml_wrapper_t* myhtml_wrapper_new() {
    mystatus_t status;
    myhtml_wrapper_t* engine = malloc(sizeof(myhtml_wrapper_t));

    engine->myhtml = myhtml_create();
    status = myhtml_init(engine->myhtml, MyHTML_OPTIONS_PARSE_MODE_SINGLE, 1, 0);

    engine->tree = myhtml_tree_create();
    myhtml_tree_init(engine->tree, engine->myhtml);

    myhtml_tree_parse_flags_set(engine->tree, MyHTML_TREE_PARSE_FLAGS_SKIP_WHITESPACE_TOKEN);
    myhtml_tree_parse_flags_set(engine->tree, MyHTML_TREE_PARSE_FLAGS_CLEAN);

    engine->mycss = mycss_create();
    status = mycss_init(engine->mycss);

    engine->entry = mycss_entry_create();
    status = mycss_entry_init(engine->mycss, engine->entry);

    engine->finder = modest_finder_create_simple();
    status = modest_finder_init(engine->finder);

    return engine;
}

void myhtml_wrapper_parse_html(myhtml_wrapper_t* engine, struct file_contents* file) {

    mystatus_t status;

    // myencoding_t encoding;
    // const char** text;
    // size_t size;
    // myencoding_detect_and_cut_bom(file.data, file.len, &encoding, text, &size);

    // status = myhtml_parse(engine->tree, encoding, *text, size);

    status = myhtml_parse(engine->tree, MyENCODING_UTF_8, file->data, file->len);

}

mycss_selectors_list_t* myhtml_wrapper_parse_selector(myhtml_wrapper_t* engine, const char* selector) {
    mystatus_t status;

    mycss_selectors_t* selectors = mycss_entry_selectors(engine->entry);
    size_t selector_len = strlen(selector);

    mycss_selectors_list_t* selectors_list = mycss_selectors_parse(selectors, MyENCODING_DEFAULT, selector, selector_len, &status);

    return selectors_list;
}

myhtml_collection_t* myhtml_wrapper_query(myhtml_wrapper_t* engine, mycss_selectors_list_t* selectors_list) {

    myhtml_collection_t* collection = 0;
    modest_finder_by_selectors_list(engine->finder, engine->tree->node_html, selectors_list, &collection);

    return collection;
}

char* myhtml_wrapper_serialize(myhtml_tree_node_t* node) {
    mycore_string_raw_t str = {0};
    myhtml_serialization(node, &str);
    return str.data;
}

void myhtml_wrapper_print_pretty(FILE* output, myhtml_tree_node_t* node, int level, char* whitespace) {
    size_t size;
    mystatus_t status;
    const char* text;
    char* firstchar;
    char* lastchar;

    mycore_string_raw_t str = {0};
    myhtml_tree_node_t* child;

    switch(node->tag_id) {
    case (MyHTML_TAG__TEXT):
        text = myhtml_node_text(node, &size);
        firstchar = (char*)text;
        while (*firstchar == ' ' || *firstchar == '\t' || *firstchar == '\n' || *firstchar == '\r') {
            firstchar++;
        }
        lastchar = firstchar + strlen(firstchar) - 1;
        while (*lastchar == ' ' || *lastchar == '\t' || *lastchar == '\n' || *lastchar == '\r') {
            lastchar--;
        }
        *++lastchar = '\0';

        if (strlen(firstchar) > 0) {
            for (int i=0; i<level; i++) fprintf(output, "%s", whitespace);
            fprintf(output, "%s\n", firstchar);
        }
        break;
    case (MyHTML_TAG__COMMENT):
        for (int i=0; i<level; i++) fprintf(output, "%s", whitespace);
        fprintf(output, "%s", "<!--");
        text = myhtml_node_text(node, &size);
        fprintf(output, "%s", text);
        fprintf(output, "%s\n", "-->");
        break;
    case (MyHTML_TAG__UNDEF):
        break;
    default:
        for (int i=0; i<level; i++) fprintf(output, "%s", whitespace);
        status = myhtml_serialization_node(node, &str);
        fprintf(output, "%s\n", str.data);
        child = node->child;
        while (child != 0) {
            myhtml_wrapper_print_pretty(output, child, level + 1, whitespace);
            child = child->next;
        }
        for (int i=0; i<level; i++) fprintf(output, "%s", whitespace);
        text = myhtml_tag_name_by_id(node->tree, node->tag_id, &size);
        fprintf(output, "</%s>\n", text);
        free(str.data);
        break;
    }
}

void myhtml_wrapper_print_text(FILE* output, myhtml_tree_node_t* node) {
    if (node->tag_id == MyHTML_TAG__TEXT) {
        size_t size;
        const char* text = myhtml_node_text(node, &size);
        fprintf(output, "%s", text);
    }
    else {
        myhtml_tree_node_t* child = node->child;
        while (child) {
            myhtml_wrapper_print_text(output, child);
            child = child->next;
        }
    }
}

void myhtml_wrapper_destroy(myhtml_wrapper_t* engine) {
    myhtml_destroy(engine->myhtml);
    myhtml_tree_destroy(engine->tree);
    mycss_destroy(engine->mycss, true);
    mycss_entry_destroy(engine->entry, true);
    modest_finder_destroy(engine->finder, true);

    free(engine);
}
