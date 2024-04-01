// How to RUN!!! 
//gcc -o generate_file generate_file.c
//./generate_file 10000 50
//This generates 10,000 integers(L) and 50 hidden keys(H)


#include <stdio.h>
#include <stdlib.h>
#include <time.h>

// Function to generate the file with positive integers and hidden keys
void generateFile(int L, int H) {
    int *numbers = (int *)malloc(sizeof(int) * L);
    if (numbers == NULL) {
        printf("Memory allocation failed.\n");
        exit(1);
    }

    srand(time(NULL));

    // Populate the array with L positive integers
    for (int i = 0; i < L; ++i) {
        numbers[i] = rand() % 1000 + 1; // Random positive integers from 1 to 1000
    }

    // Insert H hidden keys at random positions
    for (int i = 0; i < H; ++i) {
        int position = rand() % L;
        numbers[position] = -(rand() % 60 + 1); // Hidden keys: Random negative integers from -1 to -60
    }

    // Open the file for writing
    FILE *file = fopen("input.txt", "w");
    if (file == NULL) {
        printf("Error opening file.\n");
        free(numbers);
        exit(1);
    }

    // Write the array to the file
    for (int i = 0; i < L; ++i) {
        fprintf(file, "%d ", numbers[i]);
    }

    // Clean up
    fclose(file);
    free(numbers);
}

void writeResultsToFile(const char *filename, const char *results) {
    FILE *file = fopen(filename, "w");
    if (!file) {
        perror("Error opening file for writing");
        exit(EXIT_FAILURE);
    }
    fprintf(file, "%s", results);
    fclose(file);
}

int main(int argc, char *argv[]) {
    // Assuming L, H, and possibly PN are passed as command-line arguments
    // Check for the correct number of arguments
    if (argc < 3) {
        printf("Usage: %s <L> <H>\n", argv[0]);
        return 1;
    }

    int L = atoi(argv[1]);
    int H = atoi(argv[2]);

    // Validate L and H
    if (L < 10000 || H < 30 || H > 60) {
        printf("Invalid arguments. Ensure L >= 10000 and 30 <= H <= 60.\n");
        return 1;
    }

    generateFile(L, H);

    return 0;
}
