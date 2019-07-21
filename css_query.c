#include <modest/finder/finder.h>
#include <myhtml/myhtml.h>
#include <myhtml/serialization.h>
#include <mycss/mycss.h>
#include <mycss/selectors/init.h>
#include <mycss/selectors/serialization.h>

#define DIE(msg, ...) do { fprintf(stderr, msg, ##__VA_ARGS__); exit(EXIT_FAILURE); } while(0)
#define CHECK_STATUS(msg, ...) do {if(status) DIE(msg, ##__VA_ARGS__);} while(0)

#define MyCORE_FMT_Z "%zu"

mystatus_t serialization_callback(const char* data, size_t len, void* ctx)
{
    printf("%.*s", (int)len, data);
    return MyCORE_STATUS_OK;
}

mystatus_t serialization_bad_selectors(const char* buffer, size_t size, void* ctx)
{
    printf("%.*s", (int)size, buffer);
    return MyCORE_STATUS_OK;
}

myhtml_tree_t * parse_html(const char* data, size_t data_size)
{
    myhtml_t* myhtml = myhtml_create();
    mystatus_t status = myhtml_init(myhtml, MyHTML_OPTIONS_DEFAULT, 1, 0);

    CHECK_STATUS("Can't init MyHTML object\n");

    myhtml_tree_t* tree = myhtml_tree_create();
    status = myhtml_tree_init(tree, myhtml);

    CHECK_STATUS("Can't init MyHTML Tree object\n");

    status = myhtml_parse(tree, MyENCODING_UTF_8, data, data_size);
    CHECK_STATUS("Can't parse HTML:\n%s\n", data);

    return tree;
}

mycss_entry_t * create_css_parser(void)
{
    // base init
    mycss_t *mycss = mycss_create();
    mystatus_t status = mycss_init(mycss);

    CHECK_STATUS("Can't init MyCSS object\n");

    // currenr entry work init
    mycss_entry_t *entry = mycss_entry_create();
    status = mycss_entry_init(mycss, entry);

    CHECK_STATUS("Can't init MyCSS Entry object\n");

    return entry;
}

mycss_selectors_list_t * prepare_selector(mycss_entry_t *css_entry, const char* selector, size_t selector_size)
{
    mystatus_t out_status;
    mycss_selectors_list_t *list = mycss_selectors_parse(mycss_entry_selectors(css_entry),
                                                         MyENCODING_UTF_8,
                                                         selector, selector_size,
                                                         &out_status);
    /* check parsing errors */
    if(list == NULL || (list->flags & MyCSS_SELECTORS_FLAGS_SELECTOR_BAD)) {
        fprintf(stderr, "Bad CSS Selectors\n");

        if(list) {
            mycss_selectors_serialization_list(mycss_entry_selectors(css_entry), list,
                                               serialization_bad_selectors, NULL);
            printf("\n");
        }

        exit(EXIT_FAILURE);
    }

    return list;
}

void print_found_result(myhtml_tree_t* html_tree, myhtml_collection_t *collection)
{
    if(collection) {
        for(size_t i = 0; i < collection->length; i++) {
            printf("\n\t");
            myhtml_serialization_node_callback(collection->list[i], serialization_callback, NULL);
        }

        printf("\n");
    }
    else {
        printf("empty\n");
    }
}

struct res_html {
    char  *html;
    size_t size;
};

struct res_html load_html_file(const char* filename)
{
    FILE *fh = fopen(filename, "rb");
    if(fh == NULL) {
        fprintf(stderr, "Can't open html file: %s\n", filename);
        exit(EXIT_FAILURE);
    }

    if(fseek(fh, 0L, SEEK_END) != 0) {
        fprintf(stderr, "Can't set position (fseek) in file: %s\n", filename);
        exit(EXIT_FAILURE);
    }

    long size = ftell(fh);

    if(fseek(fh, 0L, SEEK_SET) != 0) {
        fprintf(stderr, "Can't set position (fseek) in file: %s\n", filename);
        exit(EXIT_FAILURE);
    }

    if(size <= 0) {
        fprintf(stderr, "Can't get file size or file is empty: %s\n", filename);
        exit(EXIT_FAILURE);
    }

    char *html = (char*)malloc(size + 1);
    if(html == NULL) {
        fprintf(stderr, "Can't allocate mem for html file: %s\n", filename);
        exit(EXIT_FAILURE);
    }

    size_t nread = fread(html, 1, size, fh);
    if (nread != size) {
        fprintf(stderr, "could not read %ld bytes (" MyCORE_FMT_Z " bytes done)\n", size, nread);
        exit(EXIT_FAILURE);
    }

    fclose(fh);

    struct res_html res = {html, (size_t)size};
    return res;
}