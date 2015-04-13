#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#define HEADER_SIZE 22



int main(int argc, char * argv[]) {
    int opt, delay=8000, volume_scale=4;
    short header[HEADER_SIZE];
    short *echo_buffer, *orig_buffer, *output_buffer;
    size_t num_samples_orig=0;
    size_t num_samples_recent=0;
    size_t j; /*size_t counter*/
    int i; /*int counter*/
    unsigned int *sizeptr, *sizeptr2;
    
    int input_spot, output_spot;
    
    if ((argc != 3) && (argc!= 5) && (argc!= 7)) {
        perror("wrong number of args");
        exit(EXIT_FAILURE);
    }

    input_spot = argc - 2;
    output_spot = input_spot + 1; /*well defined */
    
    
    while ((opt = getopt(argc, argv, "d:v:")) != -1) {
        switch (opt) {
            case 'd':
                delay = atoi(optarg);
		if (delay < 0) {
		perror("delay cannot be negative");
		exit(EXIT_FAILURE);		
		}
                break;
            case 'v':
                volume_scale = atoi(optarg);
		if (volume_scale <= 0) {
		perror("volume scale cannot be non-positive");		
		}
                break;
            default:
                perror("Option not recognized");
                exit(EXIT_FAILURE);
        }
    }
    
    
    FILE *input = fopen(argv[input_spot], "r");

    if (input == NULL) {
        perror("input or output incorrect");
        exit(EXIT_FAILURE);
    }

    FILE *output = fopen(argv[output_spot], "w");
    if (output == NULL) {
        perror("input or output incorrect");
        exit(EXIT_FAILURE);
    }    



    echo_buffer = malloc(delay*sizeof(short));
    orig_buffer = malloc(delay*sizeof(short));
    output_buffer = malloc(delay*sizeof(short));
    if (echo_buffer == NULL || orig_buffer == NULL || output_buffer == NULL) {
        perror("malloc");
        exit(EXIT_FAILURE);
    }
    
    
    /*create the header, changing the appropriate bytes to let the file know
     its size*/
    fread(header, sizeof(short), 22, input);
    sizeptr = (unsigned int *)(header + 2);
    *sizeptr += delay*2;
    sizeptr2 = (unsigned int *)(header + 20);
    *sizeptr2 += delay*2;
    
    
    fwrite(header, sizeof(short), 22, output);
    
    /*write delay samples before writing echo_buffer*/
    num_samples_recent = fread(orig_buffer, sizeof(short), delay, input);
    num_samples_orig += num_samples_recent;
    for (i=0; i < num_samples_recent; i++) {
        echo_buffer[i] = orig_buffer[i]/volume_scale;
    }
    fwrite(orig_buffer, sizeof(short), num_samples_recent, output);
    
    
    /*add silence until we hit delay*/
    if (num_samples_recent < delay && num_samples_recent >0) {
        for (i=0 ; i < delay - num_samples_recent; i++) {
            orig_buffer[i] = 0;
        }
        fwrite(orig_buffer, sizeof(short), delay - num_samples_recent, output);
        fwrite(echo_buffer, sizeof(short), num_samples_recent, output);
    }
    
    
    /*now we mix the original with delay until original is almost fully read*/
    num_samples_recent = fread(orig_buffer, sizeof(short), delay, input);
    num_samples_orig += num_samples_recent;
    while (num_samples_recent == delay) {
        
        for (i=0; i < delay; i++) {
            output_buffer[i] = orig_buffer[i] + echo_buffer[i];
            echo_buffer[i] = orig_buffer[i]/volume_scale;
        }

        fwrite(output_buffer, sizeof(short), delay, output);
        num_samples_recent = fread(orig_buffer, sizeof(short), delay, input);
        num_samples_orig += num_samples_recent;
        
    }

    /* Take care of the leftover of original, less than delay pieces*/
    if (num_samples_recent >0) {
        for (i=0; i < num_samples_recent; i++) {
            output_buffer[i] = orig_buffer[i] + echo_buffer[i];
        }
        for (j = num_samples_recent; j < delay; j++) {
            output_buffer[j] = echo_buffer[j];
        }
        fwrite(output_buffer, sizeof(short), delay, output);
        
        for (i=0; i < num_samples_recent; i++) {
            output_buffer[i] = orig_buffer[i]/volume_scale;
        }
        fwrite(output_buffer, sizeof(short),num_samples_recent, output);
        
    }

    
    free(echo_buffer);
    free(output_buffer);
    free(orig_buffer);
    fclose(input);
    fclose(output);
    return 0;
}
