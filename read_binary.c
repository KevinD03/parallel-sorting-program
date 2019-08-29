#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "helper.h"

int get_file_size(char *filename);
int compare_freq(const void *rec1, const void *rec2);


int main(int argc, char *argv[]) {

	FILE *file;
	char *file_name;
	if (argc != 2){
		fprintf(stderr, "Not enough argument!");
		exit(1);		
	}
	file_name = argv[1];
	file = fopen(file_name, "rb");
	
	struct rec rec_ptr;
	while (fread(&rec_ptr, sizeof(struct rec), 1, file) == 1) {
		printf("%s, %d\n", rec_ptr.word, rec_ptr.freq);
	}
	
	return 0;
	
}
