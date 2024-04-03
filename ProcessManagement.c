#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>
#include "ProcessManagement.h"
#include <limits.h>
#include <string.h>

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
    double totalSum = 0.0;
    int totalCount = 0;
    pid_t pids[pn];
    int segmentSizes[pn];
    int pipefds[pn][2];

    // Initialize the max value
    *globalMax = INT_MIN;

    // Open pipes and fork child processes
    for (int i = 0; i < pn; ++i) {
        segmentSizes[i] = size / pn + (i < size % pn);
        if (pipe(pipefds[i]) == -1) {
            perror("pipe");
            exit(EXIT_FAILURE);
        }

        // Allocate and assign data for each segment
        int startIndex = i * (size / pn) + (i < size % pn ? i : size % pn);
        

        pids[i] = fork();
        if (pids[i] == 0) {
            // Child process
            close(pipefds[i][0]); // Close read end of the pipe
            int max = INT_MIN;
            double avg = 0.0;
            int hiddenKeys = 0;

            // Perform work on the data segment
            performWork(data + startIndex, segmentSizes[i], &max, &avg, &hiddenKeys);

            // Send results to parent
            write(pipefds[i][1], &max, sizeof(max));
            write(pipefds[i][1], &avg, sizeof(avg));
            write(pipefds[i][1], &hiddenKeys, sizeof(hiddenKeys));
            
            // Close write end and exit
            close(pipefds[i][1]);
            exit(0);
        } else {
            // Parent process
            close(pipefds[i][1]); // Close write end of the pipe
        }
    }

    // Parent process waits for child processes to finish and reads results
    for (int i = 0; i < pn; ++i) {
        int status;
        waitpid(pids[i], &status, 0); // Wait for the specific child process to finish

        if (WIFEXITED(status)) {
            int max;
            double avg;
            int hiddenKeys;

            // Read results from the child process
            read(pipefds[i][0], &max, sizeof(max));
            read(pipefds[i][0], &avg, sizeof(avg));
            read(pipefds[i][0], &hiddenKeys, sizeof(hiddenKeys));

            // Aggregate results
            if (max > *globalMax) *globalMax = max;
            totalSum += avg * segmentSizes[i];
            totalCount += segmentSizes[i];
            *totalHiddenKeys += hiddenKeys;

            // Close read end
            close(pipefds[i][0]);
        } else {
            fprintf(stderr, "Child process exited with status %d\n", status);
        }
    }

    // Final calculation of the global average
    *globalAvg = totalSum / totalCount;
}
