#ifndef IPC_H
#define IPC_H

#include <sys/wait.h> // For wait

// Function to create a pipe for communication
void setupPipe(int fd[2]);

// Function to send a data segment from parent to child
void sendSegment(int fd[2], int *data, int size);

// Function for child to receive data segment from parent
int receiveSegment(int fd[2], int *data, int maxSize);

// Function for child to send results (localMax, localAvg, hiddenKeys) back to parent
void sendResults(int fd[2], childResult_t results);

// Function for parent to receive results from child
int receiveResults(int fd[2], childResult_t *results);

#endif // IPC_H
