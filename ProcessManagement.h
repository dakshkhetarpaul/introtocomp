#ifndef PROCESSMANAGEMENT_H
#define PROCESSMANAGEMENT_H

#include "IPC.h" // Include for communication functions

// Function to create a process tree (BFS or DFS) and distribute work
void createProcessTree(int *data, int size, int pn, int startIndex, int depth, int isBFS,
                       int *globalMax, double *globalAvg, int *totalHiddenKeys);

#endif // PROCESSMANAGEMENT_H
