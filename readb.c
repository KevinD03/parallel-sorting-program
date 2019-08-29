#include <stdio.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdlib.h>
#include "helper.h"

/* Return the size of a file in terms of byte */
int get_file_size(char *filename) {
    struct stat sbuf;

    if ((stat(filename, &sbuf)) == -1) {
       perror("stat");
       exit(1);
    }

    return sbuf.st_size;
}

/* A comparison function to use for qsort */
int compare_freq(const void *rec1, const void *rec2) {

    struct rec *r1 = (struct rec *) rec1;
    struct rec *r2 = (struct rec *) rec2;

    if (r1->freq == r2->freq) {
        return 0;
    } else if (r1->freq > r2->freq) {
        return 1;
    } else {
        return -1;
    }
}

/* Read and print a binary file into readable text. */
void read_binary(char *binary_file) {
    FILE *fp_of_b;
    /* Open the binary file. */
    if ((fp_of_b = fopen(binary_file, "rb")) == NULL) {
        fprintf(stderr, "Could not open %s\n", binary_file);
        exit(1);
    }

    /* Calculate number of lines in the file. */
    int size = get_file_size(binary_file) / sizeof(struct rec);

    /* Read the file into output. */
    struct rec *output = malloc(sizeof(struct rec) * size);
    if (output == NULL) {
        perror("malloc");
        exit(1);
    }

    /* Read one struct each time. */
    for (int i = 0; i < size; i++) {
        if (fread(&output[i], sizeof(struct rec), 1, fp_of_b) != 1) {
            fprintf(stderr, "Could not read from%s\n", binary_file);
        }
    }
    /* Print all the words. */
    for (int j = 0; j < size; j++) {
        printf("%s %d\n", output[j].word, output[j].freq);
    }
    free(output);

    /* Close the file. */
    if (fclose(fp_of_b)) {
        perror("fclose");
        exit(1);
    }
}

int main(int argc, char *argv[]) {
    read_binary(argv[1]);
}