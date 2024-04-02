#include <stdio.h>
#include "data_processing.h"

int getDataSize(const char *filename) {
  FILE *file = fopen(filename, "r");
  if (file == NULL) {
    return -1; // Error opening file
  }

  int count = 0;
  while (fscanf(file, "%*d") != EOF) {
    count++;
  }

  fclose(file);
  return count;
}

int readFileIntoArray(const char *filename, int *data, int size) {
  FILE *file = fopen(filename, "r");
  if (file == NULL) {
    return -1; // Error opening file
  }

  int numRead = 0;
  for (int i = 0; i < size && fscanf(file, "%d", &data[i]) != EOF; ++i) {
    numRead++;
  }

  fclose(file);
  return numRead; // May be less than size if EOF is reached before reading all elements
}

