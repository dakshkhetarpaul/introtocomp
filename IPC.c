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

int sendSegment(int fd[2], int *data, int size) {
    if (write(fd[1], &size, sizeof(int)) == -1) {
        perror("write size error");
        return -1; // Return error without exiting, to allow caller to handle
    }

    if (write(fd[1], data, sizeof(int) * size) == -1) {
        perror("write data error");
        return -1;
    }

    // Do not close the file descriptor here; leave it to the caller to manage lifecycle
    return 0;
}

int receiveSegment(int fd[2], int *data, int maxSize) {
    int receivedSize;
    if (read(fd[0], &receivedSize, sizeof(int)) == -1) {
        perror("read size error");
        return -1;
    }
    if (receivedSize > maxSize) {
        fprintf(stderr, "Received segment size (%d) exceeds maximum size (%d)\n", receivedSize, maxSize);
        return -1;
    }
    if (read(fd[0], data, sizeof(int) * receivedSize) == -1) {
        perror("read data error");
        return -1;
    }

    // Do not close the file descriptor here; leave it to the caller to manage lifecycle
    return receivedSize;
}

void sendResults(int fd[2], childResult_t results) {
    if (write(fd[1], &(results.localMax), sizeof(results.localMax)) == -1) {
        perror("write localMax error");
        exit(EXIT_FAILURE);
    }

    if (write(fd[1], &(results.localAvg), sizeof(results.localAvg)) == -1) {
        perror("write localAvg error");
        exit(EXIT_FAILURE);
    }

    if (write(fd[1], &(results.numHiddenKeys), sizeof(results.numHiddenKeys)) == -1) {
        perror("write numHiddenKeys error");
        exit(EXIT_FAILURE);
    }

    for (int i = 0; i < results.numHiddenKeys; ++i) {
        if (write(fd[1], &(results.hiddenKeyPositions[i]), sizeof(int)) == -1) {
            perror("write hiddenKeyPosition error");
            exit(EXIT_FAILURE);
        }
    }

    // Closing the write-end of the pipe after sending all results
    close(fd[1]);
}

int receiveResults(int fd[2], childResult_t *results) {
    if (read(fd[0], &(results->localMax), sizeof(results->localMax)) == -1) {
        perror("read localMax error");
        return -1;
    }

    if (read(fd[0], &(results->localAvg), sizeof(results->localAvg)) == -1) {
        perror("read localAvg error");
        return -1;
    }

    if (read(fd[0], &(results->numHiddenKeys), sizeof(results->numHiddenKeys)) == -1) {
        perror("read numHiddenKeys error");
        return -1;
    }

    if (results->numHiddenKeys > 0) {
        results->hiddenKeyPositions = (int*)malloc(sizeof(int) * results->numHiddenKeys);
        if (results->hiddenKeyPositions == NULL) {
            perror("malloc hiddenKeyPositions error");
            return -1;
        }

        for (int i = 0; i < results->numHiddenKeys; ++i) {
            if (read(fd[0], &(results->hiddenKeyPositions[i]), sizeof(int)) == -1) {
                perror("read hiddenKeyPosition error");
                return -1;
            }
        }
    }

    // Closing the read-end of the pipe after receiving all results
    close(fd[0]);

    return 0;
}

