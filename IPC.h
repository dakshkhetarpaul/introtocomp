// #ifndef IPC_H
// #define IPC_H

// void setupPipe(int fd[2]);

// int sendSegment(int fd[2], int *data, int size);

// int receiveSegment(int fd[2], int *data, int maxSize);

// void sendResults(int fd[2], childResult_t results);

// int receiveResults(int fd[2], childResult_t *results);

// #endif
//-----
#ifndef IPC_H
#define IPC_H

// Forward declaration of childResult_t
typedef struct childResult childResult_t;

void setupPipe(int fd[2]);
void sendResults(int fd[2], childResult_t results);
int receiveResults(int fd[2], childResult_t *results);

#endif // IPC_H

