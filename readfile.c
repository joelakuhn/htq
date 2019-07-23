#include <stdio.h>

#ifndef READFILE
#define READFILE

struct file_contents {
    char* data;
    size_t len;
};

struct file_contents* read_file(const char* filename)
{
    if (filename == (const char*)-1) {
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
                buffer = realloc(buffer, chunks * BUFFER_SIZE);
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
    else {
        char *data;
        long size;
        FILE *fh;
        fh = fopen(filename, "rb");if(fh == NULL) {
        fprintf(stderr, "Can't open html file: %s\n", filename);
            return 0;
        }

        if(fseek(fh, 0L, SEEK_END) != 0) {
            fprintf(stderr, "Can't set position (fseek) in file: %s\n", filename);
            return 0;
        }

        size = ftell(fh);

        if(fseek(fh, 0L, SEEK_SET) != 0) {
            fprintf(stderr, "Can't set position (fseek) in file: %s\n", filename);
            return 0;
        }

        if(size <= 0) {
            fprintf(stderr, "Can't get file size or file is empty: %s\n", filename);
            return 0;
        }

        data = (char*)malloc(size + 1);
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
}

void free_file(struct file_contents* file) {
    free(file->data);
    free(file);
}

#endif
