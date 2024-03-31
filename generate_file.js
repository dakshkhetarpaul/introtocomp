#include <stdio.h>
#include <stdlib.h>
#include <time.h>

void generateFile(int L, int H) {
    FILE *file = fopen("input.txt", "w");
    if (file == NULL) {
        printf("Error opening file.\n");
        exit(1);
    }

    // Generate L positive integers
    srand(time(NULL));
    for (int i = 0; i < L; i++) {
        fprintf(file, "%d ", rand() % 1000); // Adjust the range of random integers as needed
    }

    // Insert H hidden keys
    for (int i = 0; i < H; i++) {
        int position = rand() % L;
        fseek(file, 0, SEEK_SET);
        int val;
        for (int j = 0; j < position; j++) {
            fscanf(file, "%d", &val);
        }
        fprintf(file, "%d ", -(rand() % 60 + 1));
    }

    fclose(file);
}
