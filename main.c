#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "generate_file.h"
#include "data_processing.h"
#include "ProcessManagement.h"

int main(int argc, char *argv[]) {
  // Assuming L, H, and PN are passed as command-line arguments
  // Check for the correct number of arguments
  if (argc < 4) {
    printf("Usage: %s <L> <H> <PN>\n", argv[0]);
    return 1;
  }

  int L = atoi(argv[1]);
  int H = atoi(argv[2]);
  int pn = atoi(argv[3]);

  // Validate L, H, and PN
  if (L < 10000 || H < 30 || H > 60 || pn < 1) {
    printf("Invalid arguments. Ensure L >= 10000, 30 <= H <= 60, and PN >= 1.\n");
    return 1;
  }

  // Generate the data file with L positive integers and H hidden keys
  generateFile(L, H);

  // Get the size of the data file
  int dataSize = getDataSize("input.txt");
  if (dataSize == -1) {
    perror("getDataSize");
    return 1;
  }

  // Allocate memory for the data array
  int *data = (int *)malloc(sizeof(int) * dataSize);
  if (data != NULL) {

    // Read data from the file into the array
    if (readFileIntoArray("input.txt", data, dataSize) != dataSize) {
      perror("readFileIntoArray");
      free(data);
      return 1;
    }

    // Initialize variables for global results
    int globalMax = data[0];
    double globalAvg = 0;
    int totalHiddenKeys = 0;

    // Create a process tree for distributed processing (BFS by default)
    createProcessTree(data, dataSize, pn, 0, 0, 1, &globalMax, &globalAvg, &totalHiddenKeys);

    // Free allocated memory
    free(data);

    // Print final results
    printf("Global Maximum: %d\n", globalMax);
    printf("Global Average (rounded to two decimal places): %.2f\n", globalAvg);
    printf("Total Hidden Keys Found: %d\n", totalHiddenKeys);
  } else {
    perror("malloc");
    return 1;
  }

  return 0;
}
