// #include <stdio.h>
// #include <unistd.h>
// #include <stdlib.h>
// #include "IPC.h"
// #include "ProcessManagement.h"

// void setupPipe(int fd[2]) {
//   if (pipe(fd) == -1) {
//     perror("pipe");
//     exit(EXIT_FAILURE);
//   }
// }

// int sendSegment(int fd[2], int *data, int size) {
//   if (write(fd[1], &size, sizeof(int)) == -1) {
//     perror("write");
//     exit(EXIT_FAILURE);
//   }

//   if (write(fd[1], data, sizeof(int) * size) == -1) {
//     perror("write");
//     exit(EXIT_FAILURE);
//   }
//   close(fd[1]); // Close write end after sending data
//   return 0;
// }

// int receiveSegment(int fd[2], int *data, int maxSize) {
//   int receivedSize;
//   if (read(fd[0], &receivedSize, sizeof(int)) == -1) {
//     perror("read");
//     exit(EXIT_FAILURE);
//   }
//   if (receivedSize > maxSize) {
//     fprintf(stderr, "Received segment size (%d) exceeds maximum size (%d)\n", receivedSize, maxSize);
//     return -1; // Handle potential size mismatch
//   }
//   if (read(fd[0], data, sizeof(int) * receivedSize) == -1) {
//     perror("read");
//     exit(EXIT_FAILURE);
//   }
//   close(fd[0]); // Close read end after receiving data
//   return receivedSize; // Return the actual size of received data
// }

// void sendResults(int fd[2], childResult_t results) {
//     // Example of sending individual fields of `childResult_t`
//     if (write(fd[1], &(results.localMax), sizeof(results.localMax)) == -1) {
//         perror("write localMax");
//         exit(EXIT_FAILURE);
//     }

//     if (write(fd[1], &(results.localAvg), sizeof(results.localAvg)) == -1) {
//         perror("write localAvg");
//         exit(EXIT_FAILURE);
//     }

//     // Send count of hidden keys before sending their positions
//     if (write(fd[1], &(results.numHiddenKeys), sizeof(results.numHiddenKeys)) == -1) {
//         perror("write numHiddenKeys");
//         exit(EXIT_FAILURE);
//     }

//     // If there are hidden keys, send their positions
//     for (int i = 0; i < results.numHiddenKeys; ++i) {
//         if (write(fd[1], &(results.hiddenKeyPositions[i]), sizeof(int)) == -1) {
//             perror("write hiddenKeyPosition");
//             exit(EXIT_FAILURE);
//         }
//     }
// }


// int receiveResults(int fd[2], childResult_t *results) {
//     if (read(fd[0], &(results->localMax), sizeof(results->localMax)) == -1) {
//         perror("read localMax");
//         exit(EXIT_FAILURE);
//     }

//     if (read(fd[0], &(results->localAvg), sizeof(results->localAvg)) == -1) {
//         perror("read localAvg");
//         exit(EXIT_FAILURE);
//     }

//     if (read(fd[0], &(results->numHiddenKeys), sizeof(results->numHiddenKeys)) == -1) {
//         perror("read numHiddenKeys");
//         exit(EXIT_FAILURE);
//     }

//     // Allocate space for hidden key positions if there are any
//     if (results->numHiddenKeys > 0) {
//         results->hiddenKeyPositions = malloc(sizeof(int) * results->numHiddenKeys);
//         if (results->hiddenKeyPositions == NULL) {
//             perror("malloc hiddenKeyPositions");
//             exit(EXIT_FAILURE);
//         }

//         for (int i = 0; i < results->numHiddenKeys; ++i) {
//             if (read(fd[0], &(results->hiddenKeyPositions[i]), sizeof(int)) == -1) {
//                 perror("read hiddenKeyPosition");
//                 exit(EXIT_FAILURE);
//             }
//         }
//     }

//     return 0; // Success
// }

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include "IPC.h"
#include "ProcessManagement.h"

void setupPipe(int fd[2]) {
    if (pipe(fd) == -1) {
        perror("pipe setup error");
        exit(EXIT_FAILURE);
    }
}

// Functions for sending and receiving segments can remain unchanged if they are used elsewhere

// This function is updated to send the individual fields, not structures.
void sendResults(int write_fd, int max, double avg, int hiddenKeys) {
    if (write(write_fd, &max, sizeof(max)) == -1) {
        perror("write localMax error");
        exit(EXIT_FAILURE);
    }

    if (write(write_fd, &avg, sizeof(avg)) == -1) {
        perror("write localAvg error");
        exit(EXIT_FAILURE);
    }

    if (write(write_fd, &hiddenKeys, sizeof(hiddenKeys)) == -1) {
        perror("write numHiddenKeys error");
        exit(EXIT_FAILURE);
    }

    // If there are hidden key positions to send, they would need to be sent here,
    // using a protocol to determine how many there are, if not fixed.

    // Closing the write-end of the pipe after sending all results
    close(write_fd);
}

// This function is updated to receive the individual fields, not structures.
int receiveResults(int read_fd, int *max, double *avg, int *hiddenKeys) {
    if (read(read_fd, max, sizeof(*max)) == -1) {
        perror("read localMax error");
        return -1;
    }

    if (read(read_fd, avg, sizeof(*avg)) == -1) {
        perror("read localAvg error");
        return -1;
    }

    if (read(read_fd, hiddenKeys, sizeof(*hiddenKeys)) == -1) {
        perror("read numHiddenKeys error");
        return -1;
    }

    // Code to receive hidden key positions would go here if needed.

    // Closing the read-end of the pipe after receiving all results
    close(read_fd);

    return 0;
}
