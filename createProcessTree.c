#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>

// Function to create a process tree (BFS or DFS) and distribute work
void createProcessTree(int *data, int size, int pn, int startIndex, int depth, int isBFS) {
  // Calculate segment size (adjust for potential remainder)
  int segmentSize = size / pn;
  if (size % pn != 0) {
    segmentSize++;
  }

  // Create pipes for communication (parent and child)
  int pipefds[2];
  if (pipe(pipefds) == -1) {
    perror("pipe");
    exit(EXIT_FAILURE);
  }

  pid_t pid = fork();

  if (pid == -1) {
    perror("fork failed");
    exit(EXIT_FAILURE);
  } else if (pid == 0) { // Child process
    close(pipefds[1]); // Close write end of the pipe

    // Receive data segment from parent
    int childSegmentSize;
    if (read(pipefds[0], &childSegmentSize, sizeof(int)) == -1) {
      perror("read");
      exit(EXIT_FAILURE);
    }
    int childData[childSegmentSize];
    if (read(pipefds[0], childData, sizeof(int) * childSegmentSize) == -1) {
      perror("read");
      exit(EXIT_FAILURE);
    }
    close(pipefds[0]);

    // Process the assigned data segment
    int localMax = INT_MIN, localAvg = 0;
    int hiddenKeys = 0;
    for (int i = 0; i < childSegmentSize; i++) {
      localMax = fmax(localMax, childData[i]);
      localAvg += childData[i];
      if (childData[i] < 0) {
        hiddenKeys++;
      }
    }
    localAvg /= childSegmentSize;

    // Implement logic to potentially send results back to parent using pipes

    // Generate unique return code based on depth and position (BFS)
    if (isBFS) {
      int numNodesAtLevel = (int)pow(2, depth);
      exit(depth * numNodesAtLevel + (getpid() % numNodesAtLevel));
    } else {
      exit(depth); // Simple DFS code (replace with unique code if needed)
    }
  } else { // Parent process
    close(pipefds[0]); // Close read end of the pipe

    // Send data segment size and data to child process
    int childSegmentSize = (startIndex + segmentSize) > size ? size - startIndex : segmentSize;
    if (write(pipefds[1], &childSegmentSize, sizeof(int)) == -1) {
      perror("write");
      exit(EXIT_FAILURE);
    }
    if (write(pipefds[1], data + startIndex, sizeof(int) * childSegmentSize) == -1) {
      perror("write");
      exit(EXIT_FAILURE);
    }
    close(pipefds[1]);

    // Wait for child to finish and potentially collect results
    waitpid(pid, NULL, 0);

    // Decide if further process creation is needed based on BFS or DFS strategy
    if (isBFS && depth < (int)log2(pn)) {
      for (int i = 0; i < 2; i++) { // Create two child processes for BFS
        createProcessTree(data, size, pn, startIndex + segmentSize * i, depth + 1, 1);
      }
    } else if (depth < pn - 1) { // Simple DFS (replace with BFS logic if needed)
      createProcessTree(data, size, pn, startIndex + segmentSize, depth + 1, 0);
    }
  }
}
