#ifndef PROCESS_MANAGEMENT_H
#define PROCESS_MANAGEMENT_H

// Structure to store results from a child process
typedef struct childResult {
  int localMax;
  double localAvg;
  int hiddenKeys;
  int *hiddenKeyPositions; // Array to store hidden key positions (assuming unique)
  int numHiddenKeys;// Number of hidden keys found (optional, for efficiency)
} childResult_t;

// Function to create a process tree (BFS or DFS) and distribute work
void createProcessTree(int *data, int size, int pn, int startIndex, int depth, int isBFS,
                        int *globalMax, double *globalAvg, int *totalHiddenKeys);

#endif
