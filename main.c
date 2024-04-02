#include <stdio.h>
#include <stdlib.h>
#include "generate_file.h" // For generating the input data file
#include "ProcessManagement.h" // Likely contains `createProcessTree`
#include "IPC.h" // Likely contains pipe-related functions
#include "DataProcessing.h" // Likely contains functions to read data and process segments

int main(int argc, char *argv[]) {
  if (argc < 4) {
    fprintf(stderr, "Usage: %s <L> <H> <PN>\n", argv[0]);
    return EXIT_FAILURE;
  }

  int L = atoi(argv[1]);
  int H = atoi(argv[2]);
  int PN = atoi(argv[3]); // Number of processes

  // Validate inputs
  if (L < 10000 || H < 30 || H > 60 || PN < 1) {
    fprintf(stderr, "Invalid arguments.\n");
    return EXIT_FAILURE;
  }

  // Generate the file with positive integers and hidden keys
  generateFile(L, H);

  // Read the generated file and get data size
  int size = getDataSize("input.txt"); // Implement getDataSize to get file size
  if (size <= 0) {
    fprintf(stderr, "Error reading data size.\n");
    return EXIT_FAILURE;
  }

  // Allocate memory for the data array
  int *data = (int*)malloc(size * sizeof(int));
  if (data == NULL) {
    perror("malloc failed");
    return EXIT_FAILURE;
  }

  // Read data from the file into the array
  if (readFileIntoArray("input.txt", data, size) != 0) {
    fprintf(stderr, "Error reading data from file.\n");
    free(data);
    return EXIT_FAILURE;
  }

  // Choose partitioning strategy (replace with your implementation)
  int isBFS = 1; // Change to 0 for DFS

  // Create the process tree and distribute work
  int globalMax = INT_MIN, totalHiddenKeys = 0;
  double globalAvg = 0.0;
  createProcessTree(data, size, PN, 0, 0, isBFS, &globalMax, &globalAvg, &totalHiddenKeys);

  // Free the allocated memory for the data array
  free(data);

  // Print or write final results (modify based on your needs)
  printf("Global Maximum: %d\n", globalMax);
  printf("Global Average: %.2lf\n", globalAvg);
  printf("Total Hidden Keys Found: %d\n", totalHiddenKeys);

  // You can write results to a file using fprintf or similar

  return EXIT_SUCCESS;
}
