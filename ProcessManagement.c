// ProcessManagement.c
#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
#include <stdlib.h>

void forkProcesses(int depth) {
    if (depth <= 0) return;

    pid_t pid = fork();

    if (pid == -1) {
        perror("fork failed");
        exit(EXIT_FAILURE);
    }

    if (pid == 0) {
        // Child process
        printf("Child process: %d\n", getpid());
        // Child process logic goes here
        exit(0);
    } else {
        // Parent process waits for child to complete
        wait(NULL);
        printf("Parent process: %d\n", getpid());
        // Additional parent logic can go here
    }
}
