#include <stdio.h>

#ifndef READFILE
#define READFILE

struct file_contents {
    char* data;
    size_t len;
};

struct file_contents* read_file(const char* filename)
{
    FILE *fh = fopen(filename, "rb");
    if(fh == NULL) {
        fprintf(stderr, "Can't open html file: %s\n", filename);
        return 0;
    }

    if(fseek(fh, 0L, SEEK_END) != 0) {
        fprintf(stderr, "Can't set position (fseek) in file: %s\n", filename);
        return 0;
    }

    long size = ftell(fh);

    if(fseek(fh, 0L, SEEK_SET) != 0) {
        fprintf(stderr, "Can't set position (fseek) in file: %s\n", filename);
        return 0;
    }

    if(size <= 0) {
        fprintf(stderr, "Can't get file size or file is empty: %s\n", filename);
        return 0;
    }

    char *data = (char*)malloc(size + 1);
    if(data == NULL) {
        fprintf(stderr, "Can't allocate mem for data file: %s\n", filename);
        return 0;
    }

    size_t nread = fread(data, 1, size, fh);
    if (nread != size) {
        fprintf(stderr, "could not read %ld bytes (%zu bytes done)\n", size, nread);
        return 0;
    }

    fclose(fh);

    struct file_contents* file = malloc(sizeof(struct file_contents));
    file->data = data;
    file->len = size;
    return file;
}

void free_file(struct file_contents* file) {
    free(file->data);
}

#endif
