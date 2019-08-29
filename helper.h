#ifndef _HELPER_H
#define _HELPER_H

#define SIZE 44

struct rec {
    int freq;
    char word[SIZE];
};

int get_file_size(char *filename);
int compare_freq(const void *rec1, const void *rec2);
void split(int file_size, int num_process,int *array);
struct rec* child_sort(char *filename, long int location_ping, int cut_len);
void merge_sorted(int (*file_dps)[2], int num_process, int file_size, int *cut_lens, char *output);

#endif /* _HELPER_H */
