#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "generate_file.h"

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
