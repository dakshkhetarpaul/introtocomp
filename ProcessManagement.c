
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>
#include "ProcessManagement.h"
#include <limits.h>

void performWork(int *data, int segmentSize, int *max, double *avg, int *hiddenKeys) {
    // Placeholder for actual data processing
    *max = INT_MIN;
    *avg = 0.0;
    *hiddenKeys = 0;
    for (int i = 0; i < segmentSize; i++) {
        if (data[i] > *max) *max = data[i];
        *avg += data[i];
        if (data[i] < 0) (*hiddenKeys)++;
    }
    *avg /= segmentSize;
}
void createProcessTree(int *data, int size, int pn, int startIndex, int depth, int isBFS,
                       int *globalMax, double *globalAvg, int *totalHiddenKeys) {
    int segmentSize = size / pn;
    if (size % pn != 0) segmentSize++;

    int childMax, childHiddenKeys;
    double childAvg;
    int *pipefds[2 * pn]; // Array to store file descriptors for all pipes

    // Open pipes for each child process
    for (int i = 0; i < pn; ++i) {
        pipefds[i] = (int *)malloc(2 * sizeof(int));
        if (pipe(pipefds[i]) == -1) {
            perror("pipe");
            exit(EXIT_FAILURE);
        }
    }

    // Spawn child processes
    for (int i = 0; i < pn; ++i) {
        if (startIndex + i * segmentSize >= size) break; // Adjust for the last segment

        pid_t pid = fork();
        if (pid == -1) {
            perror("fork");
            exit(EXIT_FAILURE);
        } else if (pid == 0) { // Child process
            // Close reading ends for all pipes except the current one
            for (int j = 0; j < pn; ++j) {
                if (j != i) {
                    close(pipefds[j][0]);
                }
                close(pipefds[j][1]); // Close writing ends for all pipes
            }
            
            // Read data from the parent
            int *segmentData = malloc(segmentSize * sizeof(int));
            read(pipefds[i][0], segmentData, segmentSize * sizeof(int));

            // Perform actual work on the data segment
            performWork(segmentData, segmentSize, &childMax, &childAvg, &childHiddenKeys);
            free(segmentData);

            // Send results to parent
            write(pipefds[i][1], &childMax, sizeof(childMax));
            write(pipefds[i][1], &childAvg, sizeof(childAvg));
            write(pipefds[i][1], &childHiddenKeys, sizeof(childHiddenKeys));
            
            // Clean up and exit child process
            close(pipefds[i][0]);
            close(pipefds[i][1]);
            exit(0);
        }
    }

    // Parent process
    for (int i = 0; i < pn; ++i) {
        close(pipefds[i][1]); // Close writing end for parent
    }

    if (isBFS) {
        // In BFS, wait and collect results after spawning all children
        for (int i = 0; i < pn; ++i) {
            // Wait for child process to complete
            waitpid(-1, NULL, 0);

            // Read results from child process
            read(pipefds[i][0], &childMax, sizeof(childMax));
            read(pipefds[i][0], &childAvg, sizeof(childAvg));
            read(pipefds[i][0], &childHiddenKeys, sizeof(childHiddenKeys));

            // Aggregate results
            if (childMax > *globalMax) *globalMax = childMax;
            *globalAvg += childAvg * segmentSize;
            *totalHiddenKeys += childHiddenKeys;

            // Clean up
            close(pipefds[i][0]);
            free(pipefds[i]);
        }
    } else {
        // In DFS, wait and collect results as each child completes
        for (int i = 0; i < pn; ++i) {
            waitpid(-1, NULL, 0);

            // Read results from child process
            read(pipefds[i][0], &childMax, sizeof(childMax));
            read(pipefds[i][0], &childAvg, sizeof(childAvg));
            read(pipefds[i][0], &childHiddenKeys, sizeof(childHiddenKeys));

            // Same aggregation logic as above...

            // Clean up
            close(pipefds[i][0]);
            free(pipefds[i]);
        }
    }

    // Final calculation of the global average
    *globalAvg /= size; // Divide the accumulated total by the number of elements to get the average
}


