#ifndef READFILE_H
#define READFILE_H

#include <stdio.h>

struct file_contents {
    char* data;
    size_t len;
};

void free_file(struct file_contents *file);
struct file_contents *read_file(const char *filename);
struct file_contents *read_regular_file(const char *filename);
struct file_contents *read_stream(FILE *fh);

#endif
