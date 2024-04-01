// IPC.c
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

void setupIPC() {
    int pipefds[2];
    if (pipe(pipefds) == -1) {
        perror("pipe");
        exit(EXIT_FAILURE);
    }

    pid_t pid = fork();

    if (pid == -1) {
        perror("fork");
        exit(EXIT_FAILURE);
    }

    if (pid == 0) {
        close(pipefds[1]); // Close the write-end of the pipe, child doesn't write.
        // Child process reads from the pipe
        char buffer[1024];
        read(pipefds[0], buffer, sizeof(buffer));
        printf("Child reads: %s\n", buffer);
        close(pipefds[0]);
    } else {
        close(pipefds[0]); // Close the read-end of the pipe, parent doesn't read.
        // Parent process writes to the pipe
        const char *msg = "Hello from your parent!\n";
        write(pipefds[1], msg, strlen(msg) + 1);
        close(pipefds[1]);
        wait(NULL); // Wait for child process to finish
    }
}
