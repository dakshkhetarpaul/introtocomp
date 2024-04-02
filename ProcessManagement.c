#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>

// Structure to store results from a child process
typedef struct childResult {
  int localMax;
  double localAvg;
  int hiddenKeys;
  // Add more fields if needed (e.g., an array for hidden key positions)
} childResult_t;

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
    localAvg /= (double)childSegmentSize; // Cast to double for accurate average

    // Prepare data to send back to parent (consider using childResult_t)
    childResult_t childResults = {localMax, localAvg, hiddenKeys};

    // Implement logic to send results back to parent using pipes (example)
    if (write(pipefds[1], &childResults, sizeof(childResult_t)) == -1) {
      perror("write");
      exit(EXIT_FAILURE);
    }

    // Generate unique return code (improved DFS logic)
    int uniqueCode;
    if (isBFS) {
      int numNodesAtLevel = (int)pow(2, depth);
      uniqueCode = depth * numNodesAtLevel + (getpid() % numNodesAtLevel);
    } else {
      // Improved DFS code: assign a unique code based on process ID within depth
      uniqueCode = depth * pn + (getpid() % pn);
    }
    exit(uniqueCode);
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

    // Wait for child to finish and collect results
    int status;
    waitpid(pid, &status, 0);

    // Extract results from child process (modify based on your chosen communication method)
    childResult_t childResults;
    if (read(pipefds[0], &childResults, sizeof(childResult_t)) == -1) {
      perror("read");
      exit(EXIT)
