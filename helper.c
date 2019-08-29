#include <stdio.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdlib.h>
#include "helper.h"


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

// merge the sorted children using pipe
void merge_sorted(int (*file_dps)[2], int num_process, int file_size, int *cut_lens, char *output) {
    FILE *outfp = NULL;
    if ((outfp = fopen(output, "wb")) == NULL) {
        fprintf(stderr, "File openning failed: %s\n", output);
        exit(1);
    }
    struct rec temp_freq[num_process];
    int i = 0;
    while (i < num_process) {
        if (cut_lens[i] > 0) {
            int n = read(file_dps[i][0], &(temp_freq[i]), sizeof(struct rec));
            if (n  == -1) {
                perror("read");
                exit(1);
            } else if (n == 0) {
                fprintf(stderr, "Cannot read from pipe");
                exit(1);
            }
        }
        i++;
    }
    struct rec cur;
    int cur_child_process;
    for (int j = 0; j < file_size; j++) {
        for (int p = 0; p < num_process; p++) {
            if (cut_lens[p] > 0) {
                cur = temp_freq[p];
                cur_child_process = p;
            }
        }
        for (int k = 0; k < num_process; k++) {
            if (temp_freq[k].freq < cur.freq) {
                if(cut_lens[k] > 0){    
                    cur = temp_freq[k];
                    cur_child_process = k;
                }
            }
        }
        if ((fwrite(&cur, sizeof(struct rec), 1, outfp)) != 1) {
            fprintf(stderr, "Cannot write to %s\n", output);
        }
        cut_lens[cur_child_process] -= 1;
        
        if (cut_lens[cur_child_process] == 0) { 
            if ((close(file_dps[cur_child_process][0])) == -1) {
                perror("close");
                exit(1);
            }
        }else if (cut_lens[cur_child_process] > 0) {
            int n = read(file_dps[cur_child_process][0], &(temp_freq[cur_child_process]), sizeof(struct rec));
            if (n == -1) {
                perror("read");
                exit(1);
            } else if (n == 0) {
                fprintf(stderr, "Cannot read from pipe");
                exit(1);
            }
        } 
    }
    if (fclose(outfp)) {
        perror("fclose");
        exit(1);
    }
}

// Sort the data
struct rec* child_sort(char *filename, long int location_ping, int cut_len) {
    FILE *fp_binary = NULL;
    if ((fp_binary = fopen(filename, "rb")) == NULL) {
        fprintf(stderr, "File openning failed: %s \n", filename);
        exit(1);
    }
    if ((fseek(fp_binary, location_ping, SEEK_SET)) != 0) {
        perror("fseek");
        exit(1);
    }
    struct rec *output = malloc(sizeof(struct rec) * cut_len);
    if (output == NULL) {
        perror("malloc");
        exit(1);
    }
    int i = 0;
    while (i < cut_len){
        if (fread(&(output[i]), sizeof(struct rec), 1, fp_binary) != 1) {
            fprintf(stderr, "Read failed: %s \n", filename);
        }
        i++;
    }
    qsort(output, cut_len, sizeof(struct rec), compare_freq);
    
    if (fclose(fp_binary)) {
        perror("fclose");
        exit(1);
    }
    return output;
}

// Get the size of each part we split
void split(int file_size, int num_process,int *array) {
	int i = 0;
	for (int j = 0; j < num_process; j++){
		array[j] = 0; 	
	}
	while (file_size != 0){
		array[i] += 1;
		file_size -= 1;
        i += 1;
		if (i == num_process){
			i = 0;		
		}
	}
}
