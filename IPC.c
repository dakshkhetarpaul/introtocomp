#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include "IPC.h"
#include "ProcessManagement.h"

void setupPipe(int fd[2]) {
  if (pipe(fd) == -1) {
    perror("pipe");
    exit(EXIT_FAILURE);
  }
}

int sendSegment(int fd[2], int *data, int size) {
  if (write(fd[1], &size, sizeof(int)) == -1) {
    perror("write");
    exit(EXIT_FAILURE);
  }

  if (write(fd[1], data, sizeof(int) * size) == -1) {
    perror("write");
    exit(EXIT_FAILURE);
  }
  close(fd[1]); // Close write end after sending data
  return 0;
}

int receiveSegment(int fd[2], int *data, int maxSize) {
  int receivedSize;
  if (read(fd[0], &receivedSize, sizeof(int)) == -1) {
    perror("read");
    exit(EXIT_FAILURE);
  }
  if (receivedSize > maxSize) {
    fprintf(stderr, "Received segment size (%d) exceeds maximum size (%d)\n", receivedSize, maxSize);
    return -1; // Handle potential size mismatch
  }
  if (read(fd[0], data, sizeof(int) * receivedSize) == -1) {
    perror("read");
    exit(EXIT_FAILURE);
  }
  close(fd[0]); // Close read end after receiving data
  return receivedSize; // Return the actual size of received data
}

void sendResults(int fd[2], childResult_t results) {
  if (write(fd[1], &results, sizeof(childResult_t)) == -1) {
    perror("write");
    exit(EXIT_FAILURE);
  }
  close(fd[1]); // Close write end after sending results
}

int receiveResults(int fd[2], childResult_t *results) {
  if (read(fd[0], results, sizeof(childResult_t)) == -1) {
    perror("read");
    exit(EXIT_FAILURE);
  }
  close(fd[0]); // Close read end after receiving results
  return 0;
}
