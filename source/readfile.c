#include <stdio.h>
#include <stdlib.h>

#include "readfile.h"

struct file_contents* read_stream(FILE* fh) {
    char* buffer = 0;
    char* buffer_head = 0;

    size_t chunks = 0;
    size_t len = 0;
    size_t bytes_read = 0;

    const size_t BUFFER_SIZE = 65536;

    do  {
        chunks++;
        if (buffer == 0) {
            buffer = malloc(BUFFER_SIZE);
            buffer_head = buffer;
        }
        else {
            char* realloc_buffer = realloc(buffer, chunks * BUFFER_SIZE);
            if (realloc_buffer == NULL) {
                free(buffer);
                fprintf(stderr, "STDIN too large");
                return NULL;
            }
            buffer = realloc_buffer;
            buffer_head = buffer + (chunks - 1) * BUFFER_SIZE;
        }
        bytes_read = fread(buffer_head, 1, BUFFER_SIZE, stdin);
        len += bytes_read;
    } while (bytes_read == BUFFER_SIZE);

    buffer[len] = '\0';

    struct file_contents* file = malloc(sizeof(struct file_contents));
    file->data = buffer;
    file->len = len;
    return file;
}

struct file_contents* read_regular_file(const char* filename) {
    char *data;
    long size;
    FILE *fh;
    fh = fopen(filename, "rb");if(fh == NULL) {
    fprintf(stderr, "Can't open html file: %s\n", filename);
        return NULL;
    }

    if(fseek(fh, 0L, SEEK_END) != 0) {
        fprintf(stderr, "Can't read file: %s\n", filename);
        return NULL;
    }

    size = ftell(fh);

    if (size < 0) {
        fprintf(stderr, "Can't read file: %s\n", filename);
        return NULL;
    }

    if(fseek(fh, 0L, SEEK_SET) != 0) {
        fprintf(stderr, "Can't read file: %s\n", filename);
        return NULL;
    }

    if(size == 0) {
        struct file_contents* file = malloc(sizeof(struct file_contents));
        file->data = malloc(sizeof(char*));
        file->data[0] = '\0';
        file->len = 0;
        return file;
    }

    data = (char*)malloc(size + 1);
    if(data == NULL) {
        fprintf(stderr, "File too large: %s\n", filename);
        return NULL;
    }

    size_t nread = fread(data, 1, size, fh);
    if (nread != size) {
        fprintf(stderr, "Can't read file: %s\n", filename);
        return NULL;
    }

    fclose(fh);

    struct file_contents* file = malloc(sizeof(struct file_contents));
    file->data = data;
    file->len = size;
    return file;
}

struct file_contents* read_file(const char* filename)
{
    if (filename == (const char*)-1) {
        return read_stream(stdin);
    }
    else {
        return read_regular_file(filename);
    }
}

void free_file(struct file_contents* file) {
    free(file->data);
    free(file);
}
