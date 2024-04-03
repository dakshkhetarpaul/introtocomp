
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

void createProcessTree(int *data, int size, int pn, int startIndex, int depth, int isBFS, int *globalMax, double *globalAvg, int *totalHiddenKeys) {
    int segmentSize = size / pn;
    if (size % pn != 0) segmentSize++;

    for (int i = 0; i < pn; i++) {
        if (startIndex + i * segmentSize >= size) break; // Adjust for the last segment

        int pipefds[2];
        if (pipe(pipefds) == -1) {
            perror("pipe");
            exit(EXIT_FAILURE);
        }

        pid_t pid = fork();
        if (pid == -1) {
            perror("fork");
            exit(EXIT_FAILURE);
        } else if (pid == 0) { // Child process
            close(pipefds[1]); // Close the write end; child won't write to pipe

            // Read data from the parent
            int segmentData[segmentSize];
            read(pipefds[0], segmentData, sizeof(int) * segmentSize);
            close(pipefds[0]); // Close the read end

            // Perform actual work on the data segment
            int max;
            double avg;
            int hiddenKeys;
            performWork(segmentData, segmentSize, &max, &avg, &hiddenKeys);

            // Send results to parent
            write(pipefds[1], &max, sizeof(int));
            write(pipefds[1], &avg, sizeof(double));
            write(pipefds[1], &hiddenKeys, sizeof(int));
            
            close(pipefds[1]); // Close the write end
            exit(0); // Exit successfully
        } else { // Parent process
            close(pipefds[0]); // Close the read end; parent won't read from pipe
            
            // Send data to child
            write(pipefds[1], data + startIndex + i * segmentSize, sizeof(int) * segmentSize);
            close(pipefds[1]); // Close the write end

            if (!isBFS) {
                // DFS: Wait for each child to complete
                wait(NULL);
            }
            // Read results from child here...
        }
    }

    if (isBFS) {
        // For BFS, wait for all children after spawning
        for (int i = 0; i < pn; i++) {
            wait(NULL);
            // Read results from children here...
        }
    }

    // Code to aggregate results and update globalMax, globalAvg, and totalHiddenKeys
}
