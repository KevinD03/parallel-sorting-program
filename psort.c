#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <sys/wait.h>
#include "helper.h"

int main(int argc, char *argv[]) {
    extern char *optarg;
    int ch; 
    int num_process;
    char *input = NULL; 
    char *output = NULL;

    // Check if the input is valide
    if (argc != 7) {
        fprintf(stderr, "Usage: psort -n <number of processes> -f <inputfile> -o <outputfile>\n");
        exit(1);
    }

    // read in arguments
    while ((ch = getopt(argc, argv, "n:f:o:")) != -1) {
        switch(ch) {
        case 'n':
            num_process = (int) strtol(optarg, NULL, 10);
            if (num_process <= 0) {
                fprintf(stderr, "Usage: psort -n <number of processes> -f <inputfile> -o <outputfile>\n");
                exit(0);
            }
            break;
        case 'f':
            input = optarg;
            break;
        case 'o':
            output = optarg;
            break;
        default:
            fprintf(stderr, "Usage: psort -n <number of processes> -f <inputfile> -o <outputfile>\n");
            exit(1);
        }
    }

    int size_of_file = 0;
    if (get_file_size(input) < sizeof(struct rec)) {
        exit(0);
    } else {
        size_of_file = get_file_size(input) / sizeof(struct rec);
    }

    int cut_lens[num_process];

    split(size_of_file, num_process, cut_lens);

    int file_dpt[num_process][2];
    int child;
    int index = 0;
    for (int i = 0; i < num_process; i++) {
        // Get the index of where to read
        if (i != 0) { //check if it is the first iteration
            index += cut_lens[i - 1] * sizeof(struct rec);
        }
        if ((pipe(file_dpt[i])) == -1) {
            perror("pipe");
            exit(1);
        }
        if ((child = fork()) < 0) {
            perror("fork");
            exit(1);
        } else if (child > 0) { 
            if ((close(file_dpt[i][1])) == -1) { // close write pipe for parent
                perror("close");
                exit(1);
            }
        } else if (child == 0) { 
            // Close read pipe for children
            for (int j = 0; j <= i; j++) {
                if ((close(file_dpt[j][0])) == -1) {
                    perror("close read");
                    exit(1);
                }
            }
            
            struct rec *sorted_child = child_sort(input, index, cut_lens[i]);
            // Write the data to pipe
            for (int k = 0; k < cut_lens[i]; k++) {
                int n = write(file_dpt[i][1], &(sorted_child[k]), sizeof(struct rec));
                if (n == -1) {
                    perror("write");
                    exit(1);
                } else if (n == 0) {
                    fprintf(stderr, "Write to pipe failed");
                    exit(1);
                }
            }
            // close write 
            if ((close(file_dpt[i][1])) == -1) {
                perror("close wirte");
                exit(1);
            }
            free(sorted_child);
            exit(0);
        }

    }
    merge_sorted(file_dpt, num_process, size_of_file, cut_lens, output);
    
    // Wait for all children to exit first
    for (int i = 0; i < num_process; i++){
		int status;
		if (wait(&status) == -1){
			fprintf(stderr, "Child terminated abnormally \n");			
		}				
	}	
    return 0;
}
